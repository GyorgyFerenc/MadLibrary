#include <iostream>

#include "../../include/Defer.hpp"

int main(int argc, char const* argv[]) {
    // Initiailizes Defering in this scope
    DEFER_INIT;

    // Will be executed last
    DEFER(std::cout << "1. Defered" << std::endl);

    // Will be executed before last
    DEFER({
        std::cout << "2. Defered" << std::endl;
        std::cout << "3. Defered" << std::endl;
    });

    // Will be executed first
    std::cout << "Not Defered" << std::endl;

    /*
    Output:
    Not Defered
    2. Defered
    3. Defered
    1. Defered
    */
}
