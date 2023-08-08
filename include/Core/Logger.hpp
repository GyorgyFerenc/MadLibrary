#pragma once

#include <fstream>

#include "Intrinsics.hpp"

struct ILogger {
    enum Severity : uint8 {
        Info,
        Warning,
        Error,
    };

    void* obj;

    void (*log_impl)(void* obj, Severity severity, const char* c_str);
    void (*destroy_impl)(void* obj);
};

void destroy(ILogger& logger) {
    logger.destroy_impl(logger.obj);
}

void log(ILogger& logger, ILogger::Severity severity, const char* c_str) {
    logger.log_impl(logger.obj, severity, c_str);
}

void log_info(ILogger& logger, const char* c_str) {
    log(logger, ILogger::Severity::Info, c_str);
}

void log_warning(ILogger& logger, const char* c_str) {
    log(logger, ILogger::Severity::Warning, c_str);
}

void log_error(ILogger& logger, const char* c_str) {
    log(logger, ILogger::Severity::Error, c_str);
}

struct FileLogger {
    std::ofstream out;
};

void init(FileLogger& file_logger, const char* c_str) {
    file_logger.out.open(c_str);
}

void destroy(FileLogger& file_logger) {
    file_logger.out.close();
}

void log(FileLogger& file_logger, ILogger::Severity severity, const char* c_str) {
    switch (severity) {
        case ILogger::Severity::Info:
            file_logger.out << "[INFO] ";
            break;
        case ILogger::Severity::Warning:
            file_logger.out << "[WARNING] ";
            break;
        case ILogger::Severity::Error:
            file_logger.out << "[ERROR] ";
            break;
    }
    file_logger.out << c_str << std::endl;
}

ILogger to_interface(FileLogger& file_logger) {
    return ILogger{
        .obj = &file_logger,
        .log_impl = [](void*             obj,
                       ILogger::Severity severity,
                       const char*       c_str) { log(*(FileLogger*)obj, severity, c_str); },
        .destroy_impl = [](void* obj) { destroy(*(FileLogger*)obj); },
    };
}
