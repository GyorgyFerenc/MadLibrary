#ifndef MADLIBRARY_LOGGER
#define MADLIBRARY_LOGGER

#include <iostream>
#include <sstream>
#include <unordered_map>

namespace MadLibrary {
const std::string RESET = "\e[0m";
const std::string RED = "\e[1;31m";
const std::string YELLOW = "\e[1;33m";

struct Logger {
    virtual void log(const std::string text) {
    }
    virtual ~Logger() {
    }
};

#ifndef DISABLE_LOGGING

#define LOGGING_ENABLED

#define LOG(level, ...) MadLibrary::logger.log(level, __VA_ARGS__)
#define LOG_INFO(...) MadLibrary::logger.log(MadLibrary::INFO, __VA_ARGS__)
#define LOG_WARNING(...) MadLibrary::logger.log(MadLibrary::WARNING, __VA_ARGS__)
#define LOG_ERROR(...) MadLibrary::logger.log(MadLibrary::ERROR, __VA_ARGS__)
#define REGISTER_LOGGER(level, logger_ptr) MadLibrary::logger.register_logger(level, logger_ptr);
#define IF_LOGGING_ENABLED(body) body

const std::string INFO = "INFO";
const std::string WARNING = "WARNING";
const std::string ERROR = "ERROR";

struct InfoLogger : public Logger {
    virtual void log(const std::string text) {
        std::cout << "[" << INFO << "] " << text << std::endl;
    }
};

struct WarningLogger : public Logger {
    virtual void log(const std::string text) {
        std::cerr << YELLOW << "[" << WARNING << "] " << RESET << text << std::endl;
    }
};

struct ErrorLogger : public Logger {
    virtual void log(const std::string text) {
        std::cerr << RED << "[" << ERROR << "] " << RESET << text << std::endl;
    }
};

struct MainLogger {
    std::unordered_map<std::string, Logger*> loggers;

    MainLogger() {
        this->register_logger(INFO, new InfoLogger);
        this->register_logger(WARNING, new WarningLogger);
        this->register_logger(ERROR, new ErrorLogger);
    }

    /*
    Use the macro LOG, don't call this function.
    Log the text using the logger registered on this level.
    */
    template <typename... T>
    void log(const std::string level, T... text) {
        std::stringstream ss;
        ((ss << text), ...);

        try {
            loggers.at(level)->log(ss.str());
        } catch (const std::out_of_range& e) {
            throw std::runtime_error{"No logger was specified for the level"};
        }
    }

    /*
    Use the macro REGISTER_LOG, don't call this function.
    Register the logger on the given level.
    */
    void register_logger(const std::string level, Logger* logger) {
        auto it = this->loggers.find(level);

        if (it == this->loggers.end()) {
            this->loggers.emplace(level, logger);
            return;
        }

        delete it->second;
        it->second = logger;
    }

    ~MainLogger() {
        for (auto&& logger : this->loggers) {
            delete logger.second;
        }
    }
};

MainLogger logger{};

#else

#define LOG(level, ...)
#define LOG_INFO(...)
#define LOG_WARNING(...)
#define LOG_ERROR(...)
#define REGISTER_LOGGER(level, _logger)
#define IF_LOGGING_ENABLED(body)

#endif  // DISABLE_LOGGING
#define LOGGING_ENABLED

}  // namespace MadLibrary

#endif  // MADLIBRARY_LOGGER