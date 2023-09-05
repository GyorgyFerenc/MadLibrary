#pragma once

#include <cstdio>
#include <cstring>

#include "Context.hpp"
#include "Intrinsics.hpp"
#include "Memory.hpp"
#include "String.hpp"

namespace fmt {

inline String double_to_str(double number, Context context) {
    char c_str[100];
    sprintf(c_str, "%2f", number);
    String str;
    String_::init_from(&str, c_str, context);
    return str;
}

inline String uint_to_str(uint64 number, Context context) {
    StringBuilder builder;
    StringBuilder_::init(&builder, context);
    defer(StringBuilder_::destroy(&builder));
    StringBuilder_::reserve(&builder, 21);

    do {
        let digit = number % 10;
        number /= 10;
        StringBuilder_::add(&builder, '0' + digit);
    } while (number != 0);

    StringBuilder_::reverse(&builder);
    return StringBuilder_::build(builder, context);
}

inline String int_to_str(int64 number, Context context) {
    StringBuilder builder;
    StringBuilder_::init(&builder, context);
    defer(StringBuilder_::destroy(&builder));
    StringBuilder_::reserve(&builder, 21);

    let temp = number;

    let abs64 = [](int64 i) {
        if (i < 0) return -i;
        return i;
    };

    do {
        let digit = abs64(number % 10);
        number /= 10;
        StringBuilder_::add(&builder, '0' + digit);
    } while (number != 0);

    if (temp < 0) StringBuilder_::add(&builder, '-');

    StringBuilder_::reverse(&builder);
    return StringBuilder_::build(builder, context);
}

inline void print(const char* msg) {
    fputs(msg, stdout);
}

inline void print(char chr) {
    fputc(chr, stdout);
}

inline void print(String str) {
    print(String_::c_str(str));
}

inline void print(int64 uint) {
    StackAllocator<100> stack;
    Context             context;
    context.allocator = StackAllocator_::to_interface(&stack);

    let str = int_to_str(uint, context);
    print(str);
}

inline void print(int32 uint) {
    print((int64)uint);
}

inline void print(int16 uint) {
    print((int64)uint);
}
inline void print(int8 uint) {
    print((int64)uint);
}

inline void print(uint64 uint) {
    StackAllocator<100> stack;
    Context             context;
    context.allocator = StackAllocator_::to_interface(&stack);

    let str = uint_to_str(uint, context);
    print(str);
}

inline void print(uint32 uint) {
    print((uint64)uint);
}

inline void print(uint16 uint) {
    print((uint64)uint);
}

inline void print(uint8 uint) {
    print((uint64)uint);
}

template <class T>
inline void println(T msg) {
    print(msg);
    fputc('\n', stdout);
}

namespace private_ {

struct CharacterStream {
    const char* str = "\0";
};

bool empty(CharacterStream stream) {
    return *stream.str == '\0';
}

char next(CharacterStream* stream) {
    let tmp = *stream->str;
    stream->str++;
    return tmp;
}

template <class T>
inline void printf_aux(CharacterStream* stream, T arg) {
    while (!empty(*stream)) {
        let chr = next(stream);
        if (chr == '%') {
            print(arg);
            return;
        }
        // Ensure that the next character is printed no matter what
        if (chr == '\\') chr = next(stream);
        print(chr);
    }
}

};  // namespace private_

template <class... Args>
inline void printf(const char* fmt_str, Args... arg) {
    let stream = private_::CharacterStream{
        .str = fmt_str,
    };

    (private_::printf_aux(&stream, arg), ...);
    if (!private_::empty(stream)) print(stream.str);
}

namespace private_ {
template <class T>
inline void format_aux(CharacterStream* stream, StringBuilder* builder, T arg) {
    while (!empty(*stream)) {
        let chr = next(stream);
        if (chr == '%') {
            StringBuilder_::add(builder, arg);
            return;
        }
        // Ensure that the next character is printed no matter what
        if (chr == '\\') chr = next(stream);
        StringBuilder_::add(builder, chr);
    }
}
}  // namespace private_

template <class... Args>
String format(Context context, const char* fmt_str, Args... arg) {
    let stream = private_::CharacterStream{
        .str = fmt_str,
    };

    let builder = StringBuilder_::create(context);

    (private_::format_aux(&stream, &builder, arg), ...);
    if (!private_::empty(stream)) StringBuilder_::add(&builder, stream.str);
    return StringBuilder_::build(builder, context);
}

};  // namespace fmt
