#pragma once

#ifndef MADLIBRARY_DEFER
#define MADLIBRARY_DEFER

#include <functional>
#include <stack>

namespace MadLibrary {
class Defer {
   public:
    Defer() {
    }

    Defer(std::function<void()> fun) {
        this->add(fun);
    }

    ~Defer() {
        while (!stack.empty()) {
            auto f = stack.top();
            f();
            stack.pop();
        }
    }

    void add(std::function<void()> fun) {
        stack.push(fun);
    }

   private:
    std::stack<std::function<void()>> stack;
};

}  // namespace MadLibrary

#define DEFER_INIT auto __def = MadLibrary::Defer()
#define DEFER(code_block) __def.add([]() { code_block; })

#endif