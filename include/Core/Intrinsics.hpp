#pragma once

#include <cassert>
#include <cstdint>

using uint = unsigned int;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using usize = std::size_t;
using byte = unsigned char;

#define UNREACHABLE assert(false && "uncreachable");

#define let auto

// --- DEFER ---
#include <functional>
struct defer_implementation {
    std::function<void()> func;

    inline ~defer_implementation() {
        func();
    }
};

struct defer_dummy {
    inline defer_implementation operator+(std::function<void()> func) {
        return defer_implementation{func};
    }
};

#define CONCAT_IMPL(str1, str2) str1##str2
#define CONCAT(str1, str2) CONCAT_IMPL(str1, str2)

#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) CONCAT(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str) CONCAT(str, __LINE__)
#endif

#define defer_block auto ANONYMOUS_VARIABLE(__defer_instance) = defer_dummy{} + [&]()
#define defer(body) \
    auto ANONYMOUS_VARIABLE(__defer_instance) = defer_implementation{[&]() { body; }};
// --- /DEFER ---

// --- PANIC ---
#include <iostream>
void panic(const char* text) {
    std::cout << "panic:" << std::endl;
    std::cout << text << std::endl;
    exit(1);
}

void panic(std::string text) {
    std::cout << "panic:" << std::endl;
    std::cout << text << std::endl;
    exit(1);
}

void panic() {
    std::cout << "panic" << std::endl;
    exit(1);
}
// --- /PANIC ---

// --- Error ---
using Error = uint32;

inline Error declare_error() {
    static Error error = 0;
    return error++;
}

namespace CoreError {
Error OutOfRange = declare_error();
Error EmptyAcces = declare_error();  // accesing element in an empty container
Error InvalidOperation = declare_error();
Error Correct = declare_error();
}  // namespace CoreError

template <class T>
struct Errorable {
    Error error;
    T     value;
};

template <class T>
inline T unwrap(Errorable<T>& errorable) {
    if (errorable.error == CoreError::Correct) return errorable.value;
    panic("unwrap");
    UNREACHABLE;
}

template <class T>
inline T unwrap(Errorable<T>& errorable, const char* msg) {
    if (errorable.error == CoreError::Correct) return errorable.value;
    panic(msg);
    UNREACHABLE;
}

#define return_error(errorable) \
    if (errorable.error != CoreError::Correct) return {errorable.error};
// --- /Error ---
