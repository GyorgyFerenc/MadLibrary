#pragma once

#include <functional>
#include <iostream>
#include <stack>

namespace ml {
class Defer {
    using function_type = std::function<void()>;

   public:
    Defer() {
    }

    Defer(function_type fun) {
        add(fun);
    }

    ~Defer() {
        while (!s.empty()) {
            s.top()();
            s.pop();
        }
    }

    void add(function_type fun) {
        s.push(fun);
    }

   private:
    std::stack<function_type> s;
};

}  // namespace ml

#define defer_init() let __def = ml::Defer()
#define defer(code_block) __def.add(lambda_ref((), {code_block}))