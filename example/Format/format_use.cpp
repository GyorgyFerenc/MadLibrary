#include <iostream>

#include "../../include/Format.hpp"

int main(int argc, char const* argv[]) {
    std::cout << MadLibrary::format("%", 1) << std::endl;
    std::cout << MadLibrary::format("    % %", 1, 12) << std::endl;
    std::cout << MadLibrary::format("This is number: %\nThis one as well: %", 1, 12) << std::endl;
    auto a = {1, 2, 3, 4};
    std::cout << MadLibrary::format("This is a list: %", a) << std::endl;
}
