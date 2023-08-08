#include <array>
#include <forward_list>
#include <iostream>
#include <list>
#include <map>
#include <utility>
#include <vector>

#include "../../include/String.hpp"

struct Test {
    std::string to_string() const {
        return "Test{}";
    }
};

int main(int argc, char const* argv[]) {
    std::cout << "initializer_list: " << MadLibrary::to_string({1, 2, 3, 4}) << std::endl;
    std::cout << "vector: " << MadLibrary::to_string(std::vector<int>({1, 2, 3, 4})) << std::endl;
    std::cout << "array: " << MadLibrary::to_string(std::array<int, 4>({1, 2, 3, 4})) << std::endl;
    std::cout << "list: " << MadLibrary::to_string(std::list<int>({1, 2, 3, 4})) << std::endl;
    std::cout << "forward_list: " << MadLibrary::to_string(std::forward_list<int>({1, 2, 3, 4}))
              << std::endl;
    std::cout << "pair: " << MadLibrary::to_string(std::make_pair(1, 2)) << std::endl;
    std::cout << "map: "
              << MadLibrary::to_string(
                     std::map({std::make_pair(1, 2), std::make_pair(2, 2), std::make_pair(3, 2)}))
              << std::endl;

    int a = 12;
    std::cout << "pointer: " << MadLibrary::to_string(&a) << std::endl;
    std::cout << "Test: " << MadLibrary::to_string(Test{}) << std::endl;
}
