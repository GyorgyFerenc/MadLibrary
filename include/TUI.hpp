#pragma once

#include "Core.hpp"
#include "Core/Intrinsics.hpp"
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

struct TUIContext {
    usize nr_lines_printed = 0;
};

void print_canvas(TUIContext& tui_context, Canvas& canvas) {
    for (usize y = 0; y < canvas.height; y++) {
        for (usize x = 0; x < canvas.width; x++) {
            std::cout << canvas[y][x].utf8_chr;
        }
        std::cout << std::endl;
    }
    tui_context.nr_lines_printed += canvas.height;
}

void print_string(TUIContext& tui_context, String& string) {
    std::cout << string << std::flush;
    for (usize i = 0; i < size(string); i++) {
        if (string[i] == '\n') tui_context.nr_lines_printed++;
    }
}

void println_string(TUIContext& tui_context, String& string) {
    print_string(tui_context, string);
    std::cout << std::endl;
    tui_context.nr_lines_printed++;
}

void clear_screen(TUIContext& tui_context) {
    // TODO(Ferenc): Make it so it clears only the characters printed
    // std::cout << "\033[H\033[2J\033[3J" << std::flush;

    for (usize i = 0; i < tui_context.nr_lines_printed; i++) {
        std::cout << "\33[2K\033[A\r";
    }
    std::cout << std::flush;

    tui_context.nr_lines_printed = 0;
}
