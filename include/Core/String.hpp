#pragma once

#include "Array.hpp"
#include "Context.hpp"
#include "Format.hpp"
#include "Intrinsics.hpp"
#include "List.hpp"
#include "Memory.hpp"

// TODO(Ferenc): Add substring, add from functions

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

usize size(const String& str) {
    return size(str.array) - 1;  // because of '\0'
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
