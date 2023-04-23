#pragma once

#ifndef MADLIBRARY_STRING
#define MADLIBRARY_STRING

#include <sstream>
#include <string>

namespace Private {
template <class Iter>
std::string to_string_forwoard_iter(Iter begin, Iter end);
};

namespace MadLibrary {
std::string to_string(std::string value) {
    return value;
}

std::string to_string(const int value) {
    return std::to_string(value);
}
std::string to_string(const long value) {
    return std::to_string(value);
}
std::string to_string(const long long value) {
    return std::to_string(value);
}
std::string to_string(const unsigned value) {
    return std::to_string(value);
}
std::string to_string(const unsigned long value) {
    return std::to_string(value);
}
std::string to_string(const unsigned long long value) {
    return std::to_string(value);
}
std::string to_string(const float value) {
    return std::to_string(value);
}
std::string to_string(const double value) {
    return std::to_string(value);
}
std::string to_string(const long double value) {
    return std::to_string(value);
}

template <class T>
std::string to_string(T* value) {
    // TODO: Do it lighter
    std::stringstream ss;
    ss << value;
    return ss.str();
}

/*
    Creates string such: [elem1, elem2, ...]
*/
template <class T>
std::string to_string(const std::initializer_list<T>& list) {
    return Private::to_string_forwoard_iter(list.begin(), list.end());
}

#if defined(_STL_VECTOR_H) || defined(_GLIBCXX_VECTOR) || defined(_VECTOR_)
/*
    Creates string such: [elem1, elem2, ...]
*/
template <class T>
std::string to_string(const std::vector<T>& list) {
    return Private::to_string_forwoard_iter(list.begin(), list.end());
}

#endif

#if defined(_STL_ARRAY_H) || defined(_GLIBCXX_ARRAY) || defined(_ARRAY_)
/*
    Creates string such: [elem1, elem2, ...]
*/
template <class T, std::size_t size>
std::string to_string(const std::array<T, size>& list) {
    return Private::to_string_forwoard_iter(list.begin(), list.end());
}

#endif

#if defined(_STL_LIST_H) || defined(_GLIBCXX_LIST) || defined(_LIST_)
/*
    Creates string such: [elem1, elem2, ...]
*/
template <class T>
std::string to_string(const std::list<T>& list) {
    return Private::to_string_forwoard_iter(list.begin(), list.end());
}

#endif

#if defined(_STL_FORWARD_LIST_H) || defined(_GLIBCXX_FORWARD_LIST) || defined(_FORWARD_LIST_)
/*
    Creates string such: [elem1, elem2, ...]
*/
template <class T>
std::string to_string(const std::forward_list<T>& list) {
    return Private::to_string_forwoard_iter(list.begin(), list.end());
}

#endif

#if defined(_STL_UTILITY_H) || defined(_GLIBCXX_UTILITY) || defined(_UTILITY_)
/*
    Creates string such: first -> second
*/
template <class T1, class T2>
std::string to_string(const std::pair<T1, T2>& pair) {
    auto first = MadLibrary::to_string(pair.first);
    auto second = MadLibrary::to_string(pair.second);
    return first + " -> " + second;
}

#endif

#if defined(_STL_MAP_H) || defined(_GLIBCXX_MAP) || defined(_MAP_)

template <class T1, class T2>
std::string to_string(const std::map<T1, T2>& map) {
    return Private::to_string_forwoard_iter(map.begin(), map.end());
}

#endif

/*
    It expectes a member function: std::string to_string()
   const;
*/
template <class T>
std::string to_string(const T data) {
    return data.to_string();
}

}  // namespace MadLibrary

namespace Private {
template <class Iter>
std::string to_string_forwoard_iter(Iter begin, Iter end) {
    std::stringstream ss;

    ss << "[";

    while (begin != end) {
        ss << MadLibrary::to_string(*begin);
        begin++;

        if (begin != end) {
            ss << ", ";
        }
    }

    ss << "]";

    return ss.str();
}
}  // namespace Private

#endif