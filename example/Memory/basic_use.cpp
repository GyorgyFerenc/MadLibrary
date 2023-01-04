#include <iostream>
#include <memory>

#include "../../include/Format.hpp"
#include "../../include/Memory.hpp"

using namespace MadLibrary;

template <typename... Args>
auto create_function(Args... arg) {
    return [... arg = std::move(arg)]() { (println("%", arg), ...); };
}

int main(int argc, char const* argv[]) {
    auto a = std::make_unique<int>(12);
    auto asd = [a = std::move(a)]() { println("%", *a); };
    asd();
    create_function(1, 2, 3, 4, 5)();
}
