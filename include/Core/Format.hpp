#pragma once

#include <cstring>
#include <iostream>
#include <sstream>

#include "Intrinsics.hpp"

template <class... Arg>
std::string format(const char* fmt, const Arg&... arg);

template <class... Arg>
void panic_format(const char* fmt, Arg... arg) {
    panic(format(fmt, arg...));
}

template <class T>
void print(const T& obj) {
    std::cout << obj;
}

template <class... Arg>
void print(Arg... arg) {
    (print(arg), ...);
}

template <class... Arg>
void println(Arg... arg) {
    (print(arg), ...);
    std::cout << std::endl;
}

namespace Private {
struct Characters {
    const char* format;
    usize       position = 0;

    char next() {
        let current = position;
        position++;
        return format[current];
    }
};

template <class T>
void format_helper(Characters& characters, const T& obj, std::stringstream& ss) {
    while (true) {
        let ch = characters.next();
        if (ch == '%') {
            ss << obj;
            break;
        }
        ss << ch;
    }
}

}  // namespace Private

template <class... Arg>
std::string format(const char* fmt, const Arg&... arg) {
    let characters = Private::Characters{
        .format = fmt,
        .position = 0,
    };

    std::stringstream ss;
    (Private::format_helper(characters, arg, ss), ...);

    let len = strlen(fmt);

    while (characters.position < len) {
        let chr = characters.next();
        ss << chr;
    }

    return ss.str();
}

template <class... Arg>
void print_format(const char* fmt, Arg... arg) {
    print(format(fmt, arg...));
}

template <class... Arg>
void println_format(const char* fmt, Arg... arg) {
    println(format(fmt, arg...));
}


