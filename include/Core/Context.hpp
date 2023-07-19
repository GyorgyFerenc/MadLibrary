#pragma once

#include "Memory.hpp"

struct Logger {
    virtual void log(const char* str) = 0;
};

struct StdoutLogger : public Logger {
    virtual void log(const char* str) override {
        std::cout << str << std::flush;
    }
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

    // Logs to stdout
    static StdoutLogger stdout_logger;

    static let context = Context{
        .allocator = &allocator,
        .temp_allocator = &temporary_allocator,
        .logger = &stdout_logger,
    };

    return context;
}

// --- /Context ---
