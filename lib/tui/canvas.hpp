#pragma once

#include "../core.hpp"

struct Pixel{
    Rune rune;
};

Pixel pixel_from(Rune r){
    return {.rune = r};
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

void fill(Canvas c, Pixel p){
    for (usize x = 0; x < c.width; x++){
        for (usize y = 0; y < c.height; y++){
            *get(c, x, y) = p;
        }
    }
}

//struct Canvas_Iter{
//    Pixel** pixels;
//    usize width;
//    usize height;
//
//    usize x;
//    usize y;
//    Pixel  value;
//    Pixel* value_ptr;
//};


