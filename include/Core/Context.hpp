#pragma once

#include "Memory.hpp"

struct Logger {
    virtual void log(const char* c_str) = 0;
};

// --- Context ---
struct Context {
    Allocator* allocator;
    Allocator* temp_allocator;
    Logger*    logger;
};

inline Context default_context() {
    // Ring allocator with 2 MiB of data
    static TemporaryAllocator<2 * MiB> temporary_allocator;

    // Basic allocator, allocates on heap
    static BasicAllocator allocator;

    static let context = Context{
        .allocator = &allocator,
        .temp_allocator = &temporary_allocator,
    };

    return context;
}

// --- /Context ---
