#pragma once

#ifdef _GLIBCXX_IOSTREAM
// MadLanguage extension to c++
namespace ml {

template <class T>
void __print_with_space_after(T a) {
    std::cout << a << " ";
}

template <class... Args>
void print(Args... args) {
    (std::cout << ... << args);
    std::cout << std::flush;
}
template <class... Args>
void println(Args... args) {
    print(args...);
    std::cout << std::endl;
}

template <class... Args>
void print_space_between(Args... args) {
    (__print_with_space_after(args), ...);
    std::cout << std::flush;
}
template <class... Args>
void println_space_between(Args... args) {
    (__print_with_space_after(args), ...);
    std::cout << std::endl;
}

}  // namespace ml

#endif