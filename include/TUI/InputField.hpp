#pragma once
#include "TUICore.hpp"

struct InputField {
    List<UTF8Char> list;
};

void init(InputField& input_field, Context context) {
    input_field.list = List<UTF8Char>::create(context);
}

void init(InputField& input_field) {
    init(input_field, default_context());
}

void destroy(InputField& input_field) {
    destroy(input_field.list);
}

void update(InputField& input_field, Event event) {
    if (event.type != Event::Type::KeyPress) return;

    if (event.key == Key::Del) {
        pop_safe(input_field.list);
        return;
    }

    if (event.key != Key::Ascii && event.key != Key::UTF8) return;

    add(input_field.list, event.utf8_char);
}

String get_string(InputField& input_field) {
    let builder = StringBuilder::create(Context{input_field.list.allocator});
    defer(destroy(builder));

    for_each(iter(input_field.list), [&](UTF8Char utf8_char) { add(builder, utf8_char); });
    return build(builder);
}

Canvas draw(InputField& input_field) {
    Canvas canvas;
    init(canvas, input_field.list.size, 1);

    for (usize i = 0; i < input_field.list.size; i++) {
        set(canvas, i, 0, input_field.list[i]);
    }

    return canvas;
}
