// #define DISABLE_LOGGING // if defined than logging is disabled and have zero overhead
#include "../../include/Logger.hpp"  // Include the logger

struct MyFancyLogger : public MadLibrary::Logger {
    void log(const std::string text) override {
        std::cout << "[My fancy logger] " << text << std::endl;
    }
};

int main(int argc, char const* argv[]) {
    // Register the logger for the level "MyFancyLogger"
    // Note that you can register the same logger on multiple level
    REGISTER_LOGGER("MyFancyLogger", new MyFancyLogger);

    // Overrides the error logger
    REGISTER_LOGGER(MadLibrary::ERROR, new MyFancyLogger);

    // Logs on the "MyFancyLogger" level
    LOG("MyFancyLogger", "This is fancy log message");

    // Logs it using MyFancyLogger
    LOG_ERROR("This is an error");
}
