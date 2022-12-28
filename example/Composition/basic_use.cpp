#include <iostream>

#include "../../include/Composition.hpp"

struct A {
    void say_hi() {
        std::cout << "A says hi!" << std::endl;
    }
};

struct B {
    // Compose B out of A
    COMPOSE(A);

    void say_hi() {
        std::cout << "B says helloo!" << std::endl;
    }
};

int main(int argc, char const* argv[]) {
    B b{};

    b.say_hi();
    // Casts variable b into type A
    AS(b, A).say_hi();

    // Note that we expect a reference
    auto& a = AS(b, A);
    a.say_hi();

    // Note that we can acces the underling object that is
    // created, but this is not recomended because the implementation
    // might change
    b._A.say_hi();

    /*
    Output should be:
    B says helloo!
    A says hi!
    A says hi!
    A says hi!
    */
}
