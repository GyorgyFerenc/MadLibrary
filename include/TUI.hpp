#pragma once

#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include <atomic>
#include <cmath>
#include <condition_variable>
#include <csignal>
#include <exception>
#include <fstream>
#include <thread>

#include "Core.hpp"
#include "Core/Format.hpp"
#include "Core/Intrinsics.hpp"
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

struct Event {
    enum class Type {
        Resize,
        KeyPress,
    } type;

    UTF8Char utf8_char;
};

template <class T>
void log(T obj) {
    // TODO(Ferenc): Add Log into the Context api
    std::ofstream out{"log", std::ofstream::app};
    out << obj << std::endl;
}

struct TUI {
    usize              nr_lines_printed = 0;
    std::thread        input_thread;
    SharedQueue<Event> event_queue;

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
    let input_function = [&tui]() {
        // NOTE(Ferenc):
        // Maybe dup2 to have a copy of the
        // input fd so I can close that one
        // in the destroy function
        // utf-8 specification: https://datatracker.ietf.org/doc/html/rfc3629

        int std_in = 0;

        while (true) {
            char ch[4];
            read(std_in, &ch[0], 1);
            let nr_of_octets = len_of_utf8_from_first_byte(ch[0]);

            switch (nr_of_octets) {
                case 2: {
                    read(std_in, &ch[1], 1);
                } break;
                case 3: {
                    read(std_in, &ch[1], 1);
                    read(std_in, &ch[2], 1);
                } break;
                case 4: {
                    read(std_in, &ch[1], 1);
                    read(std_in, &ch[2], 1);
                    read(std_in, &ch[3], 1);
                } break;
            }

            UTF8Char utf8_char;
            set(utf8_char, ch);

            let event = Event{
                .type = Event::Type::KeyPress,
                .utf8_char = utf8_char,
            };

            enque(tui.event_queue, event);
            log("KeyPress event fired");
        }
    };
    tui.input_thread = std::thread(input_function);

    {  // set up resize handler
        // TODO(Ferenc): clean this up
        // this is experimental

        // NOTE(Ferenc): this can be multiplexed with the character input thread

        // window size
        ioctl(0, TIOCGWINSZ, &tui.size);

        tui_ptr = &tui;

        pipe(tui_ptr->signal_pipe);

        std::signal(SIGWINCH, [](int sig) {
            ioctl(0, TIOCGWINSZ, &tui_ptr->size);
            bool t = true;
            write(tui_ptr->signal_pipe[1], &t, 1);
        });

        std::thread t{[&tui]() {
            while (true) {
                bool t;
                // If read unblocks we have a
                // resize signal
                let err = read(tui.signal_pipe[0], &t, 1);
                if (err == -1) panic("Resize signal thread error");

                let event = Event{
                    .type = Event::Type::Resize,
                };

                enque(tui.event_queue, event);
            }
        }};
        t.detach();
    }
}

void unset_termios_flags(TUI& tui);

void destroy(TUI& tui) {
    unset_termios_flags(tui);
    // show cursor
    std::cout << "\e[?25h" << std::flush;

    // Note(Ferenc): this is a hack I did detach it here not when creating because this needs
    // to be replaced with proper closing of the thread
    // but I don't know any way of doing it yet
    // This should close and stop the thread
    tui.input_thread.detach();
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
    // std::cout << "\033[H\033[2J\033[3J" << std::flush;

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
