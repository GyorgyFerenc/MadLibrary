#include <iostream>

#include "../../include/Format.hpp"

int main(int argc, char const* argv[]) {
    auto init_list = {1, 2, 3, 4};

    std::cout << MadLibrary::format("%", 1) << std::endl;
    std::cout << MadLibrary::format(" % % ", 1, 12) << std::endl;
    std::cout << MadLibrary::format("This is number: %\nThis one as well: %", 1, 12) << std::endl;
    std::cout << MadLibrary::format("This is a list: %", init_list) << std::endl;

    // If iostream is included then you can use the MadLibrary::println or print
    MadLibrary::println("%", 1);
    MadLibrary::println(" % % ", 1, 12);
    MadLibrary::println("This is number: %\nThis one as well: %", 1, 12);
    MadLibrary::println("This is a list: %", init_list);
}
