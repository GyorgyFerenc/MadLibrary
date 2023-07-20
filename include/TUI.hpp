#pragma once

#include <bits/types/struct_timeval.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>

#include <atomic>
#include <csignal>
#include <thread>

#include "Core.hpp"
#include "Core/Format.hpp"
#include "Core/Intrinsics.hpp"
#include "Core/Memory.hpp"
#include "Core/Queue.hpp"
#include "Core/String.hpp"

struct Pixel {
    UTF8Char utf8_char;
};

void set(Pixel& pixel, UTF8Char utf8_char) {
    pixel.utf8_char = utf8_char;
}

void set(Pixel& pixel, const char* utf8_chr) {
    set(pixel.utf8_char, utf8_chr);
}

void set(Pixel& pixel, const char chr) {
    set(pixel.utf8_char, chr);
}

struct Canvas {
    Pixel**    matrix;
    usize      width;
    usize      height;
    Allocator* allocator;

    static Canvas create(usize width, usize height, Context context) {
        Canvas canvas;
        canvas.width = width;
        canvas.height = height;

        canvas.allocator = context.allocator;
        let try_matrix = canvas.allocator->allocate_array<Pixel*>(height);
        canvas.matrix = unwrap(try_matrix);

        for (usize i = 0; i < height; i++) {
            let try_line = canvas.allocator->allocate_array<Pixel>(width);
            canvas.matrix[i] = unwrap(try_line);

            Pixel space;
            set(space, ' ');
            typed_memset(canvas.matrix[i], space, width);
        }

        return canvas;
    }

    static Canvas create(usize width, usize height) {
        return create(width, height, default_context());
    }

    Pixel* operator[](usize pos) {
        return matrix[pos];
    }
};

void destroy(Canvas& canvas) {
    for (usize i = 0; i < canvas.height; i++) {
        canvas.allocator->free_array(canvas.matrix[i], canvas.width);
    }
    canvas.allocator->free_array(canvas.matrix, canvas.height);
}

Error set(Canvas& canvas, usize x, usize y, UTF8Char utf8_chr) {
    if (y >= canvas.height || x >= canvas.width) return CoreError::OutOfRange;
    set(canvas[y][x], utf8_chr);
    return CoreError::Correct;
}

Error set(Canvas& canvas, usize x, usize y, const char* utf8_chr) {
    if (y >= canvas.height || x >= canvas.width) return CoreError::OutOfRange;
    set(canvas[y][x], utf8_chr);
    return CoreError::Correct;
}

Error set(Canvas& canvas, usize x, usize y, const char chr) {
    if (y >= canvas.height || x >= canvas.width) return CoreError::OutOfRange;
    set(canvas[y][x], chr);
    return CoreError::Correct;
}

void fill(Canvas& canvas, const char chr) {
    for (usize i = 0; i < canvas.height; i++) {
        for (usize j = 0; j < canvas.width; j++) {
            set(canvas[i][j], chr);
        }
    }
}

Error draw(Canvas& canvas, usize x, usize y, Canvas& to_be_drawn) {
    if (x + to_be_drawn.width > canvas.width) return CoreError::OutOfRange;
    if (y + to_be_drawn.height > canvas.height) return CoreError::OutOfRange;

    for (usize i = 0; i < to_be_drawn.height; i++) {
        for (usize j = 0; j < to_be_drawn.width; j++) {
            canvas[y + i][x + j] = to_be_drawn[i][j];
        }
    }

    return CoreError::Correct;
}

enum class Key {

    // --- Special Ascii characters ---
    Null = 0,
    StartOfHeading,
    StartOfText,
    EndOfText,
    EndOfTransmition,
    Enquiry,
    Acknowledge,
    Bell,
    Backspace,
    Tab,  // horizontal tab
    LineFeed,
    VerticalTab,
    FormFeed,
    CarriageReturn,
    ShiftOut,
    ShiftIn,
    DataLinkEscape,
    DeviceControl1,
    DeviceControl2,
    DeviceControl3,
    DeviceControl4,
    NegativeAcknowledge,
    SynchronousIdle,
    EndOfTransBlock,
    Cancel,
    EndOfMedium,
    Substitute,
    Escape,
    FileSeparator,
    GroupSeparator,
    RecordSeparator,
    UnitSeparator,
    Space,

    Del = 127,

    // --- /Special Ascii characters ---

    Ascii,
    UTF8,

    // --- Specific keys on the keyboard ---
    Up,
    Down,
    Right,
    Left,
    Insert,
    Delete,  // del key in keyboard
    Home,
    End,
    PageUp,
    PageDown,
    BackTab,  // shift+tab
    // --- /Specific keys on the keyboard ---

    NewLine = LineFeed,
    Enter = LineFeed,  // Aliasing to enter
    NewPage = FormFeed,

};

struct Event {
    enum class Type {
        Resize,
        KeyPress,
    } type;

    Key      key;
    UTF8Char utf8_char;
};

struct TUI {
    usize              nr_lines_printed = 0;
    SharedQueue<Event> event_queue;
    std::thread        input_thread;
    std::atomic_bool   input_running; /* NOTE(Ferenc): This creates a kinda busy
                                         waiting maybe dup2 stdin and close it
                                         to stop input thread */

    // 0 read, 1 write
    int signal_pipe[2];

    struct termios old_termios_state;
    struct winsize size;
};

// TODO(Ferenc): This is a hack do it better
TUI* tui_ptr;

void set_termios_flags(TUI& tui);

void init(TUI& tui) {
    set_termios_flags(tui);

    // hide cursor
    std::cout << "\e[?25l" << std::flush;

    init(tui.event_queue);

    {  // set up resize handler
       // TODO(Ferenc): clean this up
       // this is experimental

        // window size
        ioctl(0, TIOCGWINSZ, &tui.size);

        tui_ptr = &tui;

        pipe(tui_ptr->signal_pipe);

        std::signal(SIGWINCH, [](int sig) {
            ioctl(0, TIOCGWINSZ, &tui_ptr->size);
            bool t = true;
            write(tui_ptr->signal_pipe[1], &t, 1);
        });
    }

    tui.input_running = true;
    let input_function = [&tui]() {
        // NOTE(Ferenc):
        // Maybe dup2 to have a copy of the
        // input fd so I can close that one
        // in the destroy function

        int    std_in = 0;
        int    pipe_read_end = tui.signal_pipe[0];
        fd_set stdin_and_signal_pipe;
        FD_ZERO(&stdin_and_signal_pipe);
        FD_SET(std_in, &stdin_and_signal_pipe);
        FD_SET(pipe_read_end, &stdin_and_signal_pipe);
        int max_fd = pipe_read_end;
        if (max_fd < std_in) max_fd = std_in;  // NOTE(Ferenc): This shoul always be pipe_read_end

        timeval time;
        time.tv_usec = 100 * 1000;  // 100 miliseconds
        time.tv_sec = 0;

        while (tui.input_running) {
            fd_set  copy_set = stdin_and_signal_pipe;
            timeval copy_time = time;

            int retval = select(max_fd + 1, &copy_set, NULL, NULL, &copy_time);
            if (retval < 0) {
                panic("Error at select");
            }
            if (FD_ISSET(std_in, &copy_set)) {
                // TODO(Ferenc): Add ANSI Keys like up_arrow etc.

                char ch[4];
                typed_memset(ch, '\0', 4);

                let key = Key::Ascii;

                // Note(Ferenc): Maybe look into /dev/tty so you can hook into that
                let retval = read(std_in, &ch[0], 4);
                switch (retval) {
                    case 0:
                    case -1: {
                        panic("Read error in thread");
                    } break;
                    case 1: {
                        key = Key::Ascii;
                        let is_named = (int)Key::Null <= ch[0] && ch[0] <= (int)Key::Space;
                        if (is_named) key = Key{ch[0]};
                        if (ch[0] == (int)Key::Del) key = Key::Del;
                    } break;
                    case 2: {
                        key = Key::UTF8;
                    } break;
                    case 3: {
                        key = Key::UTF8;
                    } break;
                    case 4: {
                        key = Key::UTF8;
                    } break;
                }

                if (ch[0] == '\x1b' && ch[1] == '[') {
                    switch (ch[2]) {
                        case 'A': {
                            key = Key::Up;
                        } break;
                        case 'B': {
                            key = Key::Down;
                        } break;
                        case 'C': {
                            key = Key::Right;
                        } break;
                        case 'D': {
                            key = Key::Left;
                        } break;
                        case 'F': {
                            key = Key::End;
                        } break;
                        case 'H': {
                            key = Key::Home;
                        } break;
                        case 'Z': {
                            key = Key::BackTab;
                        } break;
                        case '2': {
                            if (ch[3] == '~') key = Key::Insert;
                        } break;
                        case '3': {
                            if (ch[3] == '~') key = Key::Delete;
                        } break;
                        case '5': {
                            if (ch[3] == '~') key = Key::PageUp;
                        } break;
                        case '6': {
                            if (ch[3] == '~') key = Key::PageDown;
                        } break;
                    }
                }
                std::cout << (int)ch[0] << " " << (int)ch[1] << " " << (int)ch[2] << " "
                          << (int)ch[3] << " " << std::endl;

                UTF8Char utf8_char;
                set(utf8_char, ch);

                let event = Event{
                    .type = Event::Type::KeyPress,
                    .key = key,
                    .utf8_char = utf8_char,
                };

                enque(tui.event_queue, event);
            }
            if (FD_ISSET(pipe_read_end, &copy_set)) {
                bool t;
                // If read unblocks we have a
                // resize signal
                let err = read(pipe_read_end, &t, 1);
                if (err == -1) panic("Resize signal thread error");

                let event = Event{
                    .type = Event::Type::Resize,
                };

                enque(tui.event_queue, event);
            }
        }
    };
    tui.input_thread = std::thread(input_function);
}

void unset_termios_flags(TUI& tui);

void destroy(TUI& tui) {
    unset_termios_flags(tui);
    // show cursor
    std::cout << "\e[?25h" << std::flush;

    tui.input_running = false;
    tui.input_thread.join();
}

/*
 * Sets the termios flags
 * so the terminal can be behave
 * in the expected way
 */
void set_termios_flags(TUI& tui) {
    if (tcgetattr(0, &tui.old_termios_state) < 0) perror("Tset_termios_flags: ermios tcgetattr()");
    struct termios new_state = tui.old_termios_state;
    new_state.c_lflag &= ~ICANON;
    new_state.c_lflag &= ~ECHO;
    new_state.c_lflag &= ~ISIG;
    new_state.c_cc[VMIN] = 1;
    new_state.c_cc[VTIME] = 0;

    if (tcsetattr(0, TCSANOW, &new_state) < 0) perror("set_termios_flags: Termios tcsetattr()");
}

/*
 * Reset the termios state
 * for the default one
 */
void unset_termios_flags(TUI& tui) {
    struct termios old = tui.old_termios_state;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("unset_termios_flags: Termios tcsetattr");
}

void print_canvas(TUI& tui, Canvas& canvas) {
    for (usize y = 0; y < canvas.height; y++) {
        for (usize x = 0; x < canvas.width; x++) {
            std::cout << canvas[y][x].utf8_char;
        }
        std::cout << std::endl;
    }
    tui.nr_lines_printed += canvas.height;
}

void print_string(TUI& tui, String& string) {
    std::cout << string << std::flush;
    for (usize i = 0; i < size(string); i++) {
        if (string[i] == '\n') tui.nr_lines_printed++;
    }
}

void println_string(TUI& tui, String& string) {
    print_string(tui, string);
    std::cout << std::endl;
    tui.nr_lines_printed++;
}

void clear_screen(TUI& tui) {
    // TODO(Ferenc): Make it so it clears only the characters printed

    for (usize i = 0; i < tui.nr_lines_printed; i++) {
        std::cout << "\33[2K\033[A\r";
    }
    std::cout << std::flush;

    tui.nr_lines_printed = 0;
}

/*
 * return collumn, row
 */
Pair<usize, usize> get_size(TUI& tui) {
    return {tui.size.ws_col, tui.size.ws_row};
}

namespace TUIError {
Error NoKeyPressed = declare_error();
}

Errorable<Event> poll_event(TUI& tui) {
    if (!empty(tui.event_queue)) return {CoreError::Correct, deque(tui.event_queue)};
    return {TUIError::NoKeyPressed};
}

/*
 * Currently busy wait
 * TODO(Ferenc): Implement none busy wait
 */
Event poll_event_wait(TUI& tui) {
    while (true) {
        if (!empty(tui.event_queue)) return deque(tui.event_queue);
    }
}
