#pragma once

#include "TUICore.hpp"

struct BorderContext {
    Pixel top_left;
    Pixel bottom_left;
    Pixel top_right;
    Pixel bottom_right;

    Pixel horizontal;
    Pixel vertical;
};

Canvas border_canvas(Canvas& canvas, BorderContext border_context, Context context) {
    Canvas cnvs;
    init(cnvs, canvas.width + 2, canvas.height + 2, context);

    draw_canvas_into_canvas(cnvs, 1, 1, canvas);

    for (usize i = 0; i < canvas.width; i++) {
        set(cnvs, i + 1, 0, border_context.horizontal);
        set(cnvs, i + 1, cnvs.height - 1, border_context.horizontal);
    }

    for (usize i = 0; i < canvas.height; i++) {
        set(cnvs, 0, i + 1, border_context.vertical);
        set(cnvs, cnvs.width - 1, i + 1, border_context.vertical);
    }
    set(cnvs, 0, 0, border_context.top_left);
    set(cnvs, cnvs.width - 1, 0, border_context.top_right);
    set(cnvs, 0, cnvs.height - 1, border_context.bottom_left);
    set(cnvs, cnvs.width - 1, cnvs.height - 1, border_context.bottom_right);

    return cnvs;
}

Canvas border_canvas(Canvas& canvas, Context context) {
    static BorderContext border_context;
    set(border_context.top_left, "╭");
    set(border_context.top_right, "╮");
    set(border_context.bottom_left, "╰");
    set(border_context.bottom_right, "╯");
    set(border_context.vertical, "│");
    set(border_context.horizontal, "─");

    return border_canvas(canvas, border_context, context);
}
