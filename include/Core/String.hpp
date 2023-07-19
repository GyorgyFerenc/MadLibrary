#pragma once

#include "Array.hpp"
#include "Context.hpp"
#include "Format.hpp"
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

    inline static String create(usize size, Context context) {
        String str;
        str.array = Array<char>::create(size + 1, context);  // because of '\0'
        str.array[size] = '\0';
        return str;
    }

    inline static String create(usize size) {
        return create(size, default_context());
    }

    static String from(const char* c_str, Context context) {
        let size = strlen(c_str);
        let str = String::create(size, context);
        typed_memcpy(str.array.ptr, c_str, size);
        str[size] = '\0';
        return str;
    }

    static String from(const char* c_str) {
        return from(c_str, default_context());
    }

    char& operator[](usize pos) {
        return array[pos];
    }
    const char& operator[](usize pos) const {
        return array[pos];
    }
};

void destroy(String& str) {
    destroy(str.array);
}

String clone(String& str, Context context) {
    String new_str;
    new_str.array = clone(str.array, context);
    return new_str;
}

String clone(String& str) {
    String new_str;
    new_str.array = clone(str.array);
    return new_str;
}

/*
 * This gives back the byte size without the '\0'
 * Not the actual lenght if the string consists of utf8 characters
 */
usize size(const String& str) {
    return size(str.array) - 1;  // because of '\0'
}

usize size_utf8(const String& str) {
    usize nr_of_octets = 0;
    for (usize i = 0; i < size(str);) {
        let len = len_of_utf8_from_first_byte(str[i]);
        nr_of_octets++;
        i += len;
    }
    return nr_of_octets;
}

char* c_str(const String& str) {
    return str.array.ptr;
}

std::ostream& operator<<(std::ostream& os, const String& string) {
    for (usize i = 0; i < size(string); i++) {
        os << string[i];
    }
    return os;
}

Array<UTF8Char> to_utf8_array(String& str, Context context) {
    usize nr_of_octets = size_utf8(str);

    let   array = Array<UTF8Char>::create(nr_of_octets, context);
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

Array<UTF8Char> to_utf8_array(String& str) {
    return to_utf8_array(str,
                         Context{
                             .allocator = str.array.allocator,
                         });
}

using StringIter = ArrayIter<char>;
StringIter iter(String& str) {
    return iter(str.array);
}

struct StringBuilder {
    List<char> list;

    static inline StringBuilder create(Context context) {
        return {
            .list = List<char>::create(context),
        };
    }
    static inline StringBuilder create() {
        return {
            .list = List<char>::create(),
        };
    }
};

void destroy(StringBuilder& builder) {
    destroy(builder.list);
}

void add(StringBuilder& builder, char chr) {
    add(builder.list, chr);
}

void add(StringBuilder& builder, const char* chr) {
    let size = strlen(chr);
    reserve(builder.list, builder.list.size + size);
    for (usize i = 0; i < size; i++) {
        add(builder.list, chr[i]);
    }
}

void add(StringBuilder& builder, std::string str) {
    let size = str.size();
    reserve(builder.list, builder.list.size + size);
    for (usize i = 0; i < size; i++) add(builder.list, str[i]);
}

String build(StringBuilder& builder) {
    add(builder.list, '\0');
    return String{
        .array = to_array(builder.list),
    };
}
