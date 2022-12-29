#pragma once

#ifndef MADLIBRARY_FORMAT
#define MADLIBRARY_FORMAT

#include <queue>
#include <sstream>
#include <string>

#include "String.hpp"

namespace Private {
struct CharacterStream {
    std::queue<char> data;

    CharacterStream() {
    }

    CharacterStream(std::string str) {
        this->put(str);
    }

    void put(char c) {
        this->data.push(c);
    }

    void put(std::string str) {
        for (auto&& c : str) {
            this->data.push(c);
        }
    }

    char get() {
        auto c = this->data.front();
        this->data.pop();
        return c;
    }

    bool empty() {
        return this->data.empty();
    }

    std::string to_string() {
        std::string str{};

        while (!this->data.empty()) {
            char c = this->data.front();
            this->data.pop();
            str += c;
        }

        return str;
    }
};

template <class T>
void format_helper(Private::CharacterStream& in, T arg, Private::CharacterStream& out) {
    while (!in.empty()) {
        char c = in.get();

        if (c == '%') {
            out.put(MadLibrary::to_string(arg));
            return;
        }

        out.put(c);
    }
    throw std::runtime_error{"Not enough format characters!"};
}
}  // namespace Private

namespace MadLibrary {

/*
    Take a format string and replaces every '%' character with the next argument.
    It uses MadLibrary::to_string to convert data into strings.
*/
template <class... Args>
std::string format(std::string format, Args... args) {
    Private::CharacterStream in{format};
    Private::CharacterStream out;

    (Private::format_helper(in, args, out), ...);
    return out.to_string();
}

}  // namespace MadLibrary

#endif