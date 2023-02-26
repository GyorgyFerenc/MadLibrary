#include <assert.h>

#include "Logic.hpp"

proc test_split() {
    static let test = [](std::string text,
                         std::size_t len) {
        let vec = split(text, ' ');
        assert(vec.size() == len);
    };
    test("alma a fa alatt", 4);
    test("", 0);
    test("k k k k ", 4);
    test("  k k k k     ", 4);
}

proc test_uppercase_word() {
    static let test = [](std::string text,
                         std::string expected) {
        let word = uppercase_word(text);
        assert(word == expected);
    };

    test("asd", "Asd");
    test("", "");
    test("a", "A");
}

proc test_combine() {
    static let test =
        [](std::vector<std::string> to_combine,
           std::string              expected) {
            let comb = combine(to_combine, ' ');

            assert(comb == expected);
        };

    static let cheat = [](std::string text) {
        test(split(text, ' '), text);
    };

    cheat("alma a fa alatt");
    cheat("");
    cheat("a l m a a fa alatt");
}
int main(int argc, char const* argv[]) {
    test_split();
    test_uppercase_word();
    test_combine();
}
