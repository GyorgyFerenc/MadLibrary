#include <functional>
#include <iostream>
#include <string>
#include <vector>

template <class T>
std::function<T(T)> add_lhs(T a) {
    return [a](T b) { return a + b; };
}

template <class T>
std::function<T(T)> add_rhs(T a) {
    return [a](T b) { return b + a; };
}

template <class T>
T square(T a) {
    return a * a;
}

template <class T>
T increment(T a) {
    return a + 1;
}

template <class T>
T decrement(T a) {
    return a - 1;
}

template <class T, class... Args>
T pipe(T start, Args... args) {
    std::vector<std::function<T(T)>> v;
    (..., v.push_back(args));

    for (auto&& func : v) {
        start = func(start);
    }

    return start;
}

// int main() {
//     int a = pipe(0, increment<int>, increment<int>, increment<int>);
//     std::cout << a << std::endl;

//     int b = pipe(0, add_lhs(5), square<int>);
//     std::cout << b << std::endl;
// }   