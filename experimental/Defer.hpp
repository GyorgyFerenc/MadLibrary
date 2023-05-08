#pragma once

#ifndef MADLIBRARY_DEFER
#define MADLIBRARY_DEFER

#include <functional>

struct defer_implementation {
    std::function<void()> func;

    ~defer_implementation() {
        func();
    }
};

struct defer_dummy {
    defer_implementation operator+(std::function<void()> func) {
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

#define defer auto ANONYMOUS_VARIABLE(__defer_instance) = defer_dummy{} + [&]()

#endif