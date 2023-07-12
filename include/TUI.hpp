#pragma once

#include <termios.h>

#include <atomic>
#include <cmath>
#include <condition_variable>
#include <exception>
#include <thread>

#include "Core.hpp"
#include "Core/CoreErrors.hpp"
#include "Core/Intrinsics.hpp"
#include "Core/Queue.hpp"
#include "Core/String.hpp"

struct Pixel;
void set(Pixel& pixel, const char* utf8_chr);
void set(Pixel& pixel, const char chr);

struct Pixel {
    char utf8_chr[4];

    static inline Pixel create(const char chr) {
        Pixel pixel;
        set(pixel, chr);
        return pixel;
    }

    static inline Pixel create(const char* utf8_chr) {
        Pixel pixel;
        set(pixel, utf8_chr);
        return pixel;
    }
};

void set(Pixel& pixel, const char* utf8_chr) {
    if (strlen(utf8_chr) > 4) panic("set(Pixel,const char*) len is too big");
    strcpy(pixel.utf8_chr, utf8_chr);
}

void set(Pixel& pixel, const char chr) {
    pixel.utf8_chr[0] = chr;
    pixel.utf8_chr[1] = '\0';
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

            let space = Pixel::create(' ');
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

Error set(Canvas& canvas, usize x, usize y, const char* utf8_chr) {
    if (y >= canvas.height || x >= canvas.width) return CoreError::OutOfRange;
    set(canvas[y][x], utf8_chr);
    return Correct;
}

Error set(Canvas& canvas, usize x, usize y, const char chr) {
    if (y >= canvas.height || x >= canvas.width) return CoreError::OutOfRange;
    set(canvas[y][x], chr);
    return Correct;
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

    return Correct;
}

/*
 * TODO(Ferenc): Make TUI utf8 complaient
 */
struct TUI {
    usize             nr_lines_printed = 0;
    struct termios    old_termios_state;
    std::thread       input_thread;
    SharedQueue<char> charachter_queue;
    std::atomic_bool  running;
};

void set_termios_flags(TUI& tui);

void init(TUI& tui) {
    set_termios_flags(tui);

    // hide cursor
    std::cout << "\e[?25l" << std::flush;

    init(tui.charachter_queue);
    let input_function = [&tui]() {
        while (tui.running) {
            char ch;
            read(0, &ch, 1);
            enque(tui.charachter_queue, ch);
        }
    };
    tui.input_thread = std::thread(input_function);
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
            std::cout << canvas[y][x].utf8_chr;
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

namespace TUIError {
Error NoKeyPressed = declare_error();
}

Errorable<char> poll_key(TUI& tui) {
    if (!empty(tui.charachter_queue)) return {Correct, deque(tui.charachter_queue)};
    return {TUIError::NoKeyPressed};
}

/*
 * Currently busy wait
 * TODO(Ferenc): Implement none busy wait
 */
char poll_key_wait(TUI& tui) {
    while (true) {
        if (!empty(tui.charachter_queue)) return deque(tui.charachter_queue);
    }
}
