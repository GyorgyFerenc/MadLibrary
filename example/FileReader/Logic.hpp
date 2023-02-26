#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Extension.hpp"

func split(const std::string& text, char delimeter) {
    if (text == "") return std::vector<std::string>{};
    std::stringstream        ss{text};
    std::vector<std::string> ret;
    std::string              s;

    while (!ss.eof()) {
        std::getline(ss, s, delimeter);
        if (s.size() == 0) continue;
        
        ret.push_back(s);
    }

    return ret;
}

func uppercase_word(std::string word)->std::string {
    if (word.empty()) {
        return "";
    }

    word[0] = toupper(word[0]);
    return word;
}

func combine(const std::vector<std::string>& list,
             char                            delimeter)
    ->std::string {
    if (list.size() == 0) {
        return "";
    }

    if (list.size() == 1) {
        return list[0];
    }

    std::stringstream ss;

    ss << list[0];
    for (size_t i = 1; i < list.size(); i++) {
        ss << delimeter << list[i];
    }

    return ss.str();
}

template <typename T, typename func_type>
func map(const std::vector<T>& vec, func_type fun)
    ->std::vector<T> {
    std::vector<T> ret;

    for (auto&& elem : vec) {
        ret.push_back(fun(elem));
    }

    return ret;
}