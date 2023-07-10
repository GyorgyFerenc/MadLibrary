#pragma once

#include "Core/Array.hpp"
#include "Core/Format.hpp"
#include "Core/Intrinsics.hpp"
#include "Core/List.hpp"
#include "Core/Memory.hpp"
#include "Core/Pair.hpp"
#include "Core/Stack.hpp"
#include "Core/String.hpp"
// --- Iter ---

/*
 * Iter must be an object
 * which has a function next, like:
 *
 * Pair<bool, T*> next(Iter& iter)
 *
 * where T* is a pointer to the type we iterating over
 * and iter is the provided iterator
 */
template <class Iter, class Func>
inline void for_each(Iter iter, Func func) {
    let a = next(iter);  // Pair<bool, T*>
    while (a.first) {
        func(*a.second);
        a = next(iter);
    }
}
// std::ostream& operator<<(std::ostream& os, const String& string) {
//    return os << string.m_str;
//}
//
// struct StringBuilder {
//    // TODO: add capacity
//
//    Allocator* m_allocator;
//
//    char* m_str;
//    usize m_size;  // includes '\0'
//
//    static StringBuilder create() {
//        return StringBuilder::create(default_context());
//    }
//
//    static StringBuilder create(Context context) {
//        return StringBuilder{
//            .m_allocator = context.allocator,
//            .m_str = context.allocator->allocate_array<char>(1).unwrap(),
//            .m_size = 1,
//        };
//    }
//
//    void destroy() {
//        m_allocator->free_array(m_str, m_size);
//    }
//
//    void resize(usize new_size) {
//        if (new_size <= m_size) return;
//
//        let new_str = m_allocator->allocate_array<char>(new_size).unwrap();
//        memcpy(new_str, m_str, m_size);
//
//        m_allocator->free_array(m_str, m_size);
//        m_size = new_size;
//        m_str = new_str;
//    }
//
//    void add(String str) {
//        add(str.m_str, str.size());
//    }
//
//    void add(const char* c_str) {
//        add(c_str, strlen(c_str));
//    }
//
//    void add(const char* c_str, usize c_size) {
//        let old_size = m_size;
//        resize(m_size + c_size);
//
//        memcpy(m_str + old_size - 1, c_str, c_size);
//
//        m_str[m_size - 1] = '\0';
//    }
//
//    void add(std::string cpp_str) {
//        let old_size = m_size;
//        resize(m_size + cpp_str.size());
//        memcpy(m_str + old_size - 1, cpp_str.data(), cpp_str.size());
//        m_str[m_size - 1] = '\0';
//    }
//
//    String build() {
//        let str = m_allocator->allocate_array<char>(m_size).unwrap();
//        memcpy(str, m_str, m_size);
//        return String{
//            .m_str = str,
//            .m_size = m_size,
//            .m_allocator = m_allocator,
//        };
//    }
//};
//
// struct StringView {
//    String m_string;
//    usize  m_start = 0;  // inclusive
//    usize  m_end;        // inclusive
//
//    using CharOption = Option<Ref<char>>;
//
//    inline static StringView from(String str) {
//        StringView view;
//        view.m_string = str;
//        view.m_start = 0;
//        view.m_end = str.size() - 1;  //[m_start, m_end]
//        return view;
//    }
//
//    inline StringView clone() {
//        return *this;
//    }
//
//    usize size() {
//        return m_end - m_start + 1;
//    }
//
//    CharOption at(usize position) {
//        if (position > m_end) return CharOption::None();
//        return m_string.at(m_start + position);
//    }
//
//    Option<String> substring(Context context, usize position, usize len) {
//        if (m_start + position + len - 1 > m_end) return Option<String>::None();
//
//        return m_string.substring(context, position + m_start, len);
//    }
//
//    Option<String> substring(usize position, usize len) {
//        if (m_start + position + len - 1 > m_end) return Option<String>::None();
//
//        return m_string.substring(position + m_start, len);
//    }
//
//    void remove_prefix(usize number) {
//        m_start += number;
//    }
//
//    void remove_suffix(usize number) {
//        m_end -= number;
//        if (m_end < 0) m_end = 0;
//    }
//
//    bool starts_with(const char* prefix) {
//        for (usize i = 0; prefix[i] != '\0'; i++) {
//            let chr_option = at(i);
//            on_none(chr_option) return false;
//            let chr = chr_option.unwrap().deref();
//
//            if (prefix[i] != chr) return false;
//        }
//        return true;
//    }
//
//    bool ends_with(const char* suffix) {
//        let suffix_len = strlen(suffix);
//        let self_len = size();
//
//        if (suffix_len > self_len) return false;
//
//        for (usize i = 0; i < suffix_len; i++) {
//            let self_chr = at(self_len - i - 1).unwrap("ends_with").deref();
//            let suffix_chr = suffix[suffix_len - i - 1];
//
//            if (suffix_chr != self_chr) return false;
//        }
//
//        return true;
//    }
//
//    struct Iter : public ::Iter<char> {
//        const StringView* m_view;
//        usize             m_current = 0;
//
//        inline CharOption next() override {
//            if (m_current > m_view->m_end) return CharOption::None();
//            let chr = m_view->m_string.at(m_current);
//
//            m_current++;
//            return chr;
//        };
//    };
//
//    Iter iter() const {
//        Iter it;
//        it.m_view = this;
//        it.m_current = m_start;
//        return it;
//    }
//    friend std::ostream& operator<<(std::ostream&, const StringView&);
//};
//
// std::ostream& operator<<(std::ostream& os, const StringView& string) {
//    string.iter().for_each([&](char chr) { os << chr; });
//    return os;
//}
