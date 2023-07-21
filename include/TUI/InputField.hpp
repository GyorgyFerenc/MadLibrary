#pragma once

#include "TUICore.hpp"

/*
 * A simple field for input
 */
struct InputField {
    List<UTF8Char> list;
    StringBuilder  builder;
    Context        context;
    usize          pos = 0;
    usize          low_bound = 0;
};

void init(InputField& input_field, Context context) {
    input_field.list = List<UTF8Char>::create(context);
    input_field.builder = StringBuilder::create(context);
    input_field.context = context;
}

void init(InputField& input_field) {
    init(input_field, default_context());
}

void destroy(InputField& input_field) {
    destroy(input_field.list);
    destroy(input_field.builder);
}

void update(InputField& input_field, Event event) {
    if (event.type != Event::Type::KeyPress) return;
    if (event.key == Key::Left) {
        if (input_field.pos > 0) input_field.pos--;
        return;
    }
    if (event.key == Key::Right) {
        if (input_field.pos < input_field.list.size) input_field.pos++;
        return;
    }
    if (event.key == Key::Del) {
        if (input_field.pos == 0) return;

        remove(input_field.list, input_field.pos - 1);
        input_field.pos--;
        return;
    }

    if (event.key != Key::Ascii && event.key != Key::UTF8) return;

    if (input_field.pos == input_field.list.size) {
        add(input_field.list, event.utf8_char);
    } else {
        insert(input_field.list, input_field.pos, event.utf8_char);
    }
    input_field.pos++;
}

String get_string(InputField& input_field) {
    clear(input_field.builder);

    for_each(iter(input_field.list),
             [&](UTF8Char utf8_char) { add(input_field.builder, utf8_char); });

    return build(input_field.builder);
}

/*
 * It supports only 1 row thickness
 */
Canvas draw(InputField& input_field, usize column, bool show_cursor) {
    Canvas canvas;
    init(canvas, 1, column, input_field.context);
    fill(canvas, ' ');

    usize printable_collumn = column - 1;  // need one space for cursor in the end

    if (input_field.low_bound > input_field.pos) {
        input_field.low_bound = input_field.pos;
    } else if (input_field.low_bound + printable_collumn < input_field.pos) {
        input_field.low_bound = input_field.pos - printable_collumn;
    }

    usize nr_character = input_field.list.size - input_field.low_bound;
    for (usize i = 0; i < nr_character; i++) {
        set(canvas, 0, i, input_field.list[input_field.low_bound + i]);
    }

    if (show_cursor) {
        let cursor_pos = input_field.pos - input_field.low_bound;
        set_inverse(canvas[0][cursor_pos]);
    }

    return canvas;
}
