// #define DISABLE_LOGGING              // if defined than logging is disabled and have zero
// overhead
#include "../../include/Logger.hpp"  // Include the logger

// This is not necessary because if logging is disabled than you just don't
// use the struct and the function, the compiler optimalization will purge it
#ifdef LOGGING_ENABLED  // When logging is enabled, LOGGING_ENABLED macro is defined

struct MyFancyLogger : public MadLibrary::Logger {
    void log(const std::string text) override {
        std::cout << "[My fancy logger] " << text << std::endl;
    }
};

// Some complex initialization method
MadLibrary::Logger* create_logger() {
    return new MyFancyLogger{};
}

#endif

int main(int argc, char const* argv[]) {
    // It creates the body only if logging enabled
    IF_LOGGING_ENABLED({
        auto logger = create_logger();
        REGISTER_LOGGER("MyFancyLogger", logger);
    })

    // Logs on the level "MyFancyLogger"
    LOG("MyFancyLogger", "This is a log");
}
