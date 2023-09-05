#pragma once

#include "Logger.hpp"
#include "Memory.hpp"

// TODO(Ferenc): Add logger to context

// --- Context ---
struct Context {
    IAllocator allocator;
    ILogger    logger;
};

inline Context default_context() {
    // Basic allocator, allocates on heap
    static BasicAllocator allocator;

    static let context = Context{
        .allocator = BasicAllocator_::to_interface(&allocator),
    };

    return context;
}

// --- /Context ---
