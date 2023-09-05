#pragma once

#include <bits/types/struct_timeval.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <termios.h>

#include <atomic>
#include <cerrno>
#include <condition_variable>
#include <csignal>
#include <memory>
#include <mutex>
#include <ostream>
#include <thread>

#include "../Core.hpp"

enum class Color : uint8 {
    None = 0,
    Black = 30,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
};

struct Pixel {
    // bold | dim | italic | underline | blinking | inverse | hidden | strike_trough
    uint8 tags = 0;

    Color foreground = Color::None;
    Color background = Color::None;

    UTF8Char utf8_char;
};

void set_bold(Pixel& pixel) {
    pixel.tags |= 0b1 << 0;
}
void unset_bold(Pixel& pixel) {
    pixel.tags &= ~(0b1 << 0);
}
bool is_bold(Pixel& pixel) {
    return pixel.tags & (0b1 << 0);
}

void set_dim(Pixel& pixel) {
    pixel.tags |= 0b1 << 1;
}
void unset_dim(Pixel& pixel) {
    pixel.tags &= ~(0b1 << 1);
}
bool is_dim(Pixel& pixel) {
    return pixel.tags & (0b1 << 1);
}

void set_italic(Pixel& pixel) {
    pixel.tags |= 0b1 << 2;
}
void unset_italic(Pixel& pixel) {
    pixel.tags &= ~(0b1 << 2);
}
bool is_italic(Pixel& pixel) {
    return pixel.tags & (0b1 << 2);
}

void set_underline(Pixel& pixel) {
    pixel.tags |= 0b1 << 3;
}
void unset_underline(Pixel& pixel) {
    pixel.tags &= ~(0b1 << 3);
}
bool is_underline(Pixel& pixel) {
    return pixel.tags & (0b1 << 3);
}

void set_blink(Pixel& pixel) {
    pixel.tags |= 0b1 << 4;
}
void unset_blink(Pixel& pixel) {
    pixel.tags &= ~(0b1 << 4);
}
bool is_blink(Pixel& pixel) {
    return pixel.tags & (0b1 << 4);
}

void set_inverse(Pixel& pixel) {
    pixel.tags |= 0b1 << 5;
}
void unset_inverse(Pixel& pixel) {
    pixel.tags &= ~(0b1 << 5);
}
bool is_inverse(Pixel& pixel) {
    return pixel.tags & (0b1 << 5);
}

void set_hidden(Pixel& pixel) {
    pixel.tags |= 0b1 << 6;
}
void unset_hidden(Pixel& pixel) {
    pixel.tags &= ~(0b1 << 6);
}
bool is_hidden(Pixel& pixel) {
    return pixel.tags & (0b1 << 6);
}

void set_strikethrough(Pixel& pixel) {
    pixel.tags |= 0b1 << 7;
}
void unset_strikethrough(Pixel& pixel) {
    pixel.tags &= ~(0b1 << 7);
}
bool is_strikethrough(Pixel& pixel) {
    return pixel.tags & (0b1 << 7);
}

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
    IAllocator allocator;

    Pixel* operator[](usize pos) {
        return matrix[pos];
    }
};

void init(Canvas& canvas, usize height, usize width, Context context) {
    canvas.width = width;
    canvas.height = height;

    canvas.allocator = context.allocator;
    let try_matrix = IAllocator_::allocate_array<Pixel*>(canvas.allocator, height);
    canvas.matrix = Errorable_::unwrap(try_matrix);

    for (usize i = 0; i < height; i++) {
        let try_line = IAllocator_::allocate_array<Pixel>(canvas.allocator, width);
        canvas.matrix[i] = Errorable_::unwrap(try_line);

        Pixel space;
        set(space, ' ');
        typed_memset(canvas.matrix[i], space, width);
    }
}

void destroy(Canvas& canvas) {
    for (usize i = 0; i < canvas.height; i++) {
        IAllocator_::free_array(canvas.allocator, canvas.matrix[i], canvas.width);
    }
    IAllocator_::free_array(canvas.allocator, canvas.matrix, canvas.height);
}

Error set(Canvas& canvas, usize y, usize x, UTF8Char utf8_chr) {
    if (y >= canvas.height || x >= canvas.width) return CoreError::OutOfRange;
    set(canvas[y][x], utf8_chr);
    return CoreError::Correct;
}

Error set(Canvas& canvas, usize y, usize x, const char* utf8_chr) {
    if (y >= canvas.height || x >= canvas.width) return CoreError::OutOfRange;
    set(canvas[y][x], utf8_chr);
    return CoreError::Correct;
}

Error set(Canvas& canvas, usize y, usize x, const char chr) {
    if (y >= canvas.height || x >= canvas.width) return CoreError::OutOfRange;
    set(canvas[y][x], chr);
    return CoreError::Correct;
}

Error set(Canvas& canvas, usize y, usize x, Pixel pixel) {
    if (y >= canvas.height || x >= canvas.width) return CoreError::OutOfRange;
    canvas[y][x] = pixel;
    return CoreError::Correct;
}

void fill(Canvas& canvas, const char chr) {
    for (usize i = 0; i < canvas.height; i++) {
        for (usize j = 0; j < canvas.width; j++) {
            set(canvas[i][j], chr);
        }
    }
}

Error draw_canvas_into_canvas(Canvas& canvas, usize x, usize y, Canvas& to_be_drawn) {
    if (x + to_be_drawn.width > canvas.width) return CoreError::OutOfRange;
    if (y + to_be_drawn.height > canvas.height) return CoreError::OutOfRange;

    for (usize i = 0; i < to_be_drawn.height; i++) {
        for (usize j = 0; j < to_be_drawn.width; j++) {
            canvas[y + i][x + j] = to_be_drawn[i][j];
        }
    }

    return CoreError::Correct;
}

// TODO(Ferenc): Give them better names
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

namespace TUI {

struct TUI {
    usize            nr_lines_printed = 0;
    Queue<Event>     event_queue;
    std::thread      input_thread;
    std::atomic_bool input_running = true; /* NOTE(Ferenc): This creates a kinda busy
                                               waiting maybe dup2 stdin and close it
                                               to stop input thread */
    bool in_altscreen = false;

    std::mutex              event_mutex;  // These to used for polling
    std::condition_variable event_cond_var;

    // 0 read, 1 write
    int signal_pipe[2];

    struct termios old_termios_state;
    struct winsize size;
};

TUI tui;

void set_termios_flags();
void unset_termios_flags();

void init(Context context) {
    set_termios_flags();

    // hide cursor
    std::cout << "\e[?25l" << std::flush;

    init(tui.event_queue, context);

    {  // set up resize handler
        // window size
        ioctl(0, TIOCGWINSZ, &tui.size);
        pipe(tui.signal_pipe);
        std::signal(SIGWINCH, [](int sig) {
            ioctl(0, TIOCGWINSZ, &tui.size);
            bool t = true;
            write(tui.signal_pipe[1], &t, 1);
        });
    }

    let input_function = []() {
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
            if (retval < 0) panic("Error at select");

            if (FD_ISSET(std_in, &copy_set)) {
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
                        let is_named = (int)Key::Null <= ch[0] && ch[0] <= (int)Key::UnitSeparator;
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
                UTF8Char utf8_char;
                set(utf8_char, ch);

                let event = Event{
                    .type = Event::Type::KeyPress,
                    .key = key,
                    .utf8_char = utf8_char,
                };

                {
                    std::unique_lock<std::mutex> locker(tui.event_mutex);
                    enque(tui.event_queue, event);
                }
                tui.event_cond_var.notify_one();
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

                {
                    std::unique_lock<std::mutex> locker(tui.event_mutex);
                    enque(tui.event_queue, event);
                }
                tui.event_cond_var.notify_one();
            }
        }
    };
    tui.input_thread = std::thread(input_function);
}

void destroy() {
    unset_termios_flags();
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
void set_termios_flags() {
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
void unset_termios_flags() {
    struct termios old = tui.old_termios_state;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("unset_termios_flags: Termios tcsetattr");
}

void print_canvas(Canvas& canvas) {
    for (usize y = 0; y < canvas.height; y++) {
        for (usize x = 0; x < canvas.width; x++) {
            let pixel = canvas[y][x];

            if (is_bold(pixel)) std::cout << "\033[1m";
            if (is_dim(pixel)) std::cout << "\033[2m";
            if (is_italic(pixel)) std::cout << "\033[3m";
            if (is_underline(pixel)) std::cout << "\033[4m";
            if (is_blink(pixel)) std::cout << "\033[5m";
            if (is_inverse(pixel)) std::cout << "\033[7m";
            if (is_hidden(pixel)) std::cout << "\033[8m";
            if (is_strikethrough(pixel)) std::cout << "\033[9m";
            if (pixel.foreground != Color::None)
                std::cout << "\033[" << (uint32)pixel.foreground << "m";
            if (pixel.background != Color::None)
                std::cout << "\033[" << (uint32)pixel.background + 10 << "m";

            std::cout << pixel.utf8_char << "\033[0m";
        }
        std::cout << std::endl;
    }
    tui.nr_lines_printed += canvas.height;
}

void print_string(String& string) {
    std::cout << string << std::flush;
    for (usize i = 0; i < size(string); i++) {
        if (string[i] == '\n') tui.nr_lines_printed++;
    }
}

void println_string(String& string) {
    print_string(string);
    std::cout << std::endl;
    tui.nr_lines_printed++;
}

void clear_screen() {
    static let clear_line = []() { std::cout << "\033[2K"; };
    static let move_up = []() { std::cout << "\033[F"; };

    for (usize i = 0; i < tui.nr_lines_printed; i++) {
        move_up();
        clear_line();
    }
    std::cout << std::flush;

    tui.nr_lines_printed = 0;
}

void go_alt_screen() {
    tui.in_altscreen = true;
    std::cout << "\033[?1049h" << std::flush;
    std::cout << "\033[H" << std::flush;
}

void leave_alt_screen() {
    tui.in_altscreen = false;
    std::cout << "\033[?1049l" << std::flush;
}

/*
 * return row, collumn
 */
Pair<usize, usize> get_size() {
    return {tui.size.ws_row, tui.size.ws_col};
}

Error NoKeyPressed = declare_error();

Errorable<Event> poll_event() {
    if (!empty(tui.event_queue)) return {CoreError::Correct, deque(tui.event_queue)};
    return {NoKeyPressed};
}

Event poll_event_wait() {
    std::unique_lock<std::mutex> locker(tui.event_mutex);
    tui.event_cond_var.wait(locker, [&]() { return !empty(tui.event_queue); });

    if (empty(tui.event_queue)) panic("poll_event_wait: queue should not be empty");

    return deque(tui.event_queue);
}
};  // namespace TUI
