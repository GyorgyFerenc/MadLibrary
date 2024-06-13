#pragma once

#include "../core.hpp"

struct Pixel{
    enum Style: u8{
        Bold         = 1 << 0,
        Dim          = 1 << 1,
        Italic       = 1 << 2,
        Underline    = 1 << 3,
        Blinking     = 1 << 4,
        Inverse      = 1 << 5,
        Hidden       = 1 << 6,
        Striketrough = 1 << 7,
    };
    enum Color_Mode: u8{
        None,
        Color_Mode_8,
        Color_Mode_16,
        Color_Mode_256,
        Color_Mode_RGB,
    };
    enum struct Color_8: u8{
        Black = 30,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        Default,
        Reset,
    };

    enum struct Color_16: u8{
        Black = 30,
        Red,
        Green,
        Yellow,
        Blue,
        Magenta,
        Cyan,
        White,
        Default,
        Reset,

        Bright_Black = 90,
        Bright_Red,
        Bright_Green,
        Bright_Yellow,
        Bright_Blue,
        Bright_Magenta,
        Bright_Cyan,
        Bright_White,
    };    

    Rune rune = 0;
    u8 style_flags = 0;

    struct {
        Color_Mode color_mode = None;
        union{
            Color_8 color_8;
            Color_16 color_16;
            u8 color_256;
            struct {
                u8 r;
                u8 g;
                u8 b;
            } color_rgb;
        };
    } background, foreground;
};

inline
void set_style(Pixel* p, Pixel::Style style){
    p->style_flags |= cast(u8) style;
}

inline
void unset_style(Pixel* p, Pixel::Style style){
    p->style_flags &= ~(cast(u8) style);
}

inline
bool check_style(Pixel p, Pixel::Style style){
   return p.style_flags & cast(u8) style;
}

inline
bool same_style(Pixel p1, Pixel p2, Pixel::Style style){
    return !(check_style(p1, style) ^ check_style(p2, style));

}

void toggle_style(Pixel* p, Pixel::Style style){
    p->style_flags ^= cast(u8) style;
}

constexpr Pixel DEFAULT_PIXEL = {
    .rune = ' ',
};

struct Canvas{
    enum Kind: u8{
        Full,
        Slice,
    } kind = Full;

    Allocator allocator;

    // indexed as [x][y]
    Pixel** pixels;
    usize width;
    usize height;

    struct {
        usize x;
        usize y;
    } slice; // only if it is a Slice
};

Canvas create_canvas(Allocator allocator, usize width, usize height, Pixel pixel = DEFAULT_PIXEL){
    Canvas c;
    c.kind = Canvas::Full;
    c.width = width;
    c.height = height;
    c.allocator = allocator; 

    c.pixels = allocate_array_non_zero<Pixel*>(allocator, width);
    For_Each(range(0, width)){
        let buffer = allocate_array_non_zero<Pixel>(allocator, height);
        typed_memset(buffer, pixel, height);
        c.pixels[it.value] = buffer;
    }

    return c;
}

void destroy(Canvas c){
    assert(c.kind == Canvas::Full);

    For_Each(range(0, c.height)){
        free(c.allocator, c.pixels[it.value]);
    }
    free(c.allocator, c.pixels);
}

inline
Option<Canvas> canvas_from(Canvas c, usize x, usize y, usize width, usize height){
    if (x + width > c.width || y + height > c.height){
        return {{}, false};
    }

    Canvas result;
    result.kind = Canvas::Slice;
    result.pixels = c.pixels;
    result.width = width;
    result.height = height;
    result.slice.x = x;
    result.slice.y = y;

    return {result, true};
}

inline
Pixel* get(Canvas c, usize x, usize y){
    if (c.kind == Canvas::Slice){
        x += c.slice.x;
        y += c.slice.y;
    }

    return &c.pixels[x][y];
}

inline
Option<Pixel*> get_safe(Canvas c, usize x, usize y){
    switch(c.kind){
    case Canvas::Full: {
        if (x >= c.width || y >= c.height){
            return {{}, false};
        }

        return {&c.pixels[x][y], true};
    } break;
    case Canvas::Slice: {
        if (x >= c.width || y >= c.height){
            return {{}, false};
        }

        return {&c.pixels[c.slice.x + x][c.slice.y + y], true};
    } break;
    }
}

inline
void fill(Canvas c, Pixel p){
    for (usize x = 0; x < c.width; x++){
        for (usize y = 0; y < c.height; y++){
            *get(c, x, y) = p;
        }
    }
}
