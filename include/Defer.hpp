#pragma once

#include <functional>
#include <iostream>
#include <stack>

namespace ml {
class Defer {
   public:
    Defer() {
    }

    Defer(std::function<void()> fun) {
        add(fun);
    }

    ~Defer() {
        while (!s.empty()) {
            auto f = s.top();
            f();
            s.pop();
        }
    }

    void add(std::function<void()> fun) {
        s.push(fun);
    }

   private:
    std::stack<std::function<void()>> s;
};

}  // namespace ml

#define defer_init() let __def = ml::Defer()
#define defer(code_block) __def.add(lambda_ref((), {code_block}))