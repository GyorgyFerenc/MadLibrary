// #define DISABLE_LOGGING // if defined than logging is disabled and have zero overhead
#include "../../include/Logger.hpp"  // Include the logger

int main(int argc, char const* argv[]) {
    LOG_INFO("This is an info");                       // Writes to stdout
    LOG_WARNING("This is a warning");                  // Writes to stderr
    LOG_ERROR("This is an error");                     // Writes to stderr
    LOG(MadLibrary::ERROR, "This is again an error");  // Writes to stderr
    LOG("ERROR", "This is again an error");            // Writes to stderr
}
