#pragma once

#include <cstring>

#include "Array.hpp"
#include "Context.hpp"
// #include "Format.hpp"
#include "Intrinsics.hpp"
#include "List.hpp"
#include "Memory.hpp"
#include "Queue.hpp"

/*
 * Extracts the number of octets from the utf8 encoding
 */
uint8 len_of_utf8_from_first_byte(const char chr) {
    // utf-8 specification: https://datatracker.ietf.org/doc/html/rfc3629

    let is_ascii = ((uint8)chr >> 7) == 0;
    if (is_ascii) return 1;

    let is_two_octet = ((uint8)chr >> 5) == 0b110;
    if (is_two_octet) return 2;

    let is_three_octet = ((uint8)chr >> 4) == 0b1110;
    if (is_three_octet) return 3;

    let is_four_octet = ((uint8)chr >> 3) == 0b11110;
    if (is_four_octet) return 4;

    panic("Not utf8 encoding");
    UNREACHABLE;
}

struct UTF8Char {
    char utf8_chr[4];
};

/*
 * Returns the number of octets in the utf8_chr
 */
uint8 set(UTF8Char& utf8_char, const char* utf8_chr) {
    // NOTE(Ferenc): utf-8 specification: https://datatracker.ietf.org/doc/html/rfc3629
    // TODO(Ferenc): Add checking for 10xxxxxx in octets
    // TODO(Ferenc): Maybe factor out a check if const char* is utf8 character

    // usize size = strlen(utf8_chr);
    // if (size > 4) panic("set(UTF8Char,const char*) len is too big");

    utf8_char.utf8_chr[0] = '\0';
    utf8_char.utf8_chr[1] = '\0';
    utf8_char.utf8_chr[2] = '\0';
    utf8_char.utf8_chr[3] = '\0';

    let nr_of_octets = len_of_utf8_from_first_byte(utf8_chr[0]);

    switch (nr_of_octets) {
        case 1: {
            utf8_char.utf8_chr[0] = utf8_chr[0];
        } break;
        case 2: {
            utf8_char.utf8_chr[0] = utf8_chr[0];
            utf8_char.utf8_chr[1] = utf8_chr[1];
        } break;
        case 3: {
            utf8_char.utf8_chr[0] = utf8_chr[0];
            utf8_char.utf8_chr[1] = utf8_chr[1];
            utf8_char.utf8_chr[2] = utf8_chr[2];
        } break;
        case 4: {
            utf8_char.utf8_chr[0] = utf8_chr[0];
            utf8_char.utf8_chr[1] = utf8_chr[1];
            utf8_char.utf8_chr[2] = utf8_chr[2];
            utf8_char.utf8_chr[3] = utf8_chr[3];
        } break;
    }

    return nr_of_octets;
}

uint8 set(UTF8Char& utf8_char, const char chr) {
    utf8_char.utf8_chr[0] = chr;
    utf8_char.utf8_chr[1] = '\0';
    utf8_char.utf8_chr[2] = '\0';
    utf8_char.utf8_chr[3] = '\0';
    return 1;
}

bool equal(UTF8Char utf8_char, const char chr) {
    return utf8_char.utf8_chr[0] == chr;
}

bool equal(UTF8Char utf8_char, const char* other) {
    let nr_of_octets = len_of_utf8_from_first_byte(other[0]);

    switch (nr_of_octets) {
        case 1: {
            return utf8_char.utf8_chr[0] == other[0];
        } break;
        case 2: {
            return utf8_char.utf8_chr[0] == other[0] && utf8_char.utf8_chr[1] == other[1];
        } break;
        case 3: {
            return utf8_char.utf8_chr[0] == other[0] && utf8_char.utf8_chr[1] == other[1] &&
                   utf8_char.utf8_chr[2] == other[2];
        } break;
        case 4: {
            return utf8_char.utf8_chr[0] == other[0] && utf8_char.utf8_chr[1] == other[1] &&
                   utf8_char.utf8_chr[2] == other[2] && utf8_char.utf8_chr[3] == other[3];
        } break;
    }

    panic("other is not utf8");
    UNREACHABLE;
}

bool equal(UTF8Char utf8_char, UTF8Char other) {
    return utf8_char.utf8_chr[0] == other.utf8_chr[0] &&
           utf8_char.utf8_chr[1] == other.utf8_chr[1] &&
           utf8_char.utf8_chr[2] == other.utf8_chr[2] && utf8_char.utf8_chr[3] == other.utf8_chr[3];
}

std::ostream& operator<<(std::ostream& os, const UTF8Char& utf8_char) {
    return os << utf8_char.utf8_chr;
}

// TODO(Ferenc): Add substring, add "from" functions

/*
 * Fixed size string
 * Use StringBuilder to
 * create one
 */
struct String {
    Array<char> array;

    char& operator[](usize pos) {
        return array[pos];
    }
    const char& operator[](usize pos) const {
        return array[pos];
    }
};

namespace String_ {
void init(String* str, usize size, Context context) {
    Array_::init(&str->array, size + 1, context);
    str->array[size] = '\0';
}

String create(usize size, Context context) {
    String str;
    init(&str, size, context);
    return str;
}

void init_from(String* str, const char* c_str, Context context) {
    let size = strlen(c_str);
    init(str, size, context);
    typed_memcpy(str->array.ptr, c_str, size);
    str->array[size] = '\0';
}

String from(const char* c_str, Context context) {
    String str;
    init_from(&str, c_str, context);
    return str;
}

void destroy(String* str) {
    Array_::destroy(&str->array);
}

String clone(String str, Context context) {
    String new_str;
    new_str.array = Array_::clone(str.array, context);
    return new_str;
}

/*
 * This gives back the byte size without the '\0'
 * Not the actual lenght if the string consists of utf8 characters
 */
usize size(String str) {
    return str.array.size - 1;  // because of '\0'
}

usize size_utf8(String str) {
    usize nr_of_octets = 0;
    for (usize i = 0; i < size(str);) {
        let len = len_of_utf8_from_first_byte(str[i]);
        nr_of_octets++;
        i += len;
    }
    return nr_of_octets;
}

char* c_str(String str) {
    return str.array.ptr;
}

Array<UTF8Char> to_utf8_array(String str, Context context) {
    usize nr_of_octets = size_utf8(str);

    let array = Array_::create<UTF8Char>(nr_of_octets, context);

    usize array_i = 0;
    for (usize i = 0; i < size(str);) {
        let len = len_of_utf8_from_first_byte(str[i]);

        UTF8Char chr;
        set(chr, &str[i]);

        array[array_i] = chr;

        i += len;
        array_i++;
    }
    return array;
}

Array<UTF8Char> to_utf8_array(String str) {
    return to_utf8_array(str,
                         Context{
                             .allocator = str.array.allocator,
                         });
}
}  // namespace String_

struct StringBuilder {
    List<char> list;
};

namespace StringBuilder_ {
void init(StringBuilder* builder, Context context) {
    List_::init(&builder->list, context);
}

StringBuilder create(Context context) {
    StringBuilder builder;
    init(&builder, context);
    return builder;
}

void destroy(StringBuilder* builder) {
    List_::destroy(&builder->list);
}

void reserve(StringBuilder* builder, usize new_capacity) {
    List_::reserve(&builder->list, new_capacity);
}

void add(StringBuilder* builder, char chr) {
    List_::add(&builder->list, chr);
}

void add(StringBuilder* builder, String str) {
    let size = String_::size(str);
    List_::reserve(&builder->list, builder->list.size + size);

    for (usize i = 0; i < size; i++) {
        List_::add(&builder->list, str[i]);
    }
}

void add(StringBuilder* builder, UTF8Char utf8_char) {
    let size = len_of_utf8_from_first_byte(utf8_char.utf8_chr[0]);
    List_::reserve(&builder->list, builder->list.size + size);
    for (usize i = 0; i < size; i++) {
        List_::add(&builder->list, utf8_char.utf8_chr[i]);
    }
}

void add(StringBuilder* builder, const char* chr) {
    let size = strlen(chr);
    List_::reserve(&builder->list, builder->list.size + size);
    for (usize i = 0; i < size; i++) {
        List_::add(&builder->list, chr[i]);
    }
}

void clear(StringBuilder* builder) {
    List_::clear(&builder->list);
}

String build(StringBuilder builder, Context context) {
    List_::add(&builder.list, '\0');
    return String{
        .array = List_::to_array(builder.list, context),
    };
}

void reverse(StringBuilder* builder) {
    List_::reverse(&builder->list);
}
};  // namespace StringBuilder_
