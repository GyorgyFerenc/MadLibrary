#pragma once

#include "Core/Array.hpp"
#include "Core/Format.hpp"
#include "Core/Intrinsics.hpp"
#include "Core/List.hpp"
#include "Core/Memory.hpp"
#include "Core/Pair.hpp"
#include "Core/Stack.hpp"
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

// --- /Iter ---
///*
// * Fixed size array
// */
// template <class T>
// struct Array {
//    T*    m_ptr;
//    usize m_size;
//
//    Allocator* m_allocator;
//
//    inline static Array<T> create(usize size) {
//        return Array<T>::create(default_context(), size);
//    }
//
//    inline static Array<T> create(Context context, usize size) {
//        return Array<T>{
//            .m_ptr = context.allocator->allocate_array<T>(size).unwrap(),
//            .m_size = size,
//            .m_allocator = context.allocator,
//        };
//    }
//
//    void destroy() const {
//        m_allocator->free_array(m_ptr, m_size);
//    }
//
//    template <class Func>
//    void destroy(Func destroy_function) const {
//        for (usize i = 0; i < m_size; i++) {
//            destroy_function(this->m_ptr[i]);
//        }
//
//        m_allocator->free_array(m_ptr, m_size);
//    }
//
//    inline usize size() {
//        return m_size;
//    }
//
//    inline T& operator[](usize position) {
//        return m_ptr[position];
//    }
//
//    Option<Ref<T>> at(usize position) const {
//        if (position >= m_size) {
//            return Option<Ref<T>>::None();
//        }
//        return Option<Ref<T>>::Some(Ref<T>{&this->m_ptr[position]});
//    }
//
//    struct Iter : public ::Iter<T> {
//        Iter(Array<T>& array) : array(array){};
//
//        Option<Ref<T>> next() override {
//            return array.at(current++);
//        }
//
//        Array<T>& array;
//        usize     current = 0;
//    };
//
//    Iter iter() {
//        return Iter{*this};
//    }
//};
//
///*
// * This implementation uses a linked list
// */
// template <class T>
// struct Stack {
//    static Stack<T> create() {
//        return Stack<T>::create(default_context());
//    }
//
//    static Stack<T> create(Context context) {
//        Stack<T> s;
//        s.m_allocator = context.allocator;
//        s.m_head = nullptr;
//        s.m_size = 0;
//
//        return s;
//    }
//
//    void destroy() {
//        let next = m_head;
//        while (next != nullptr) {
//            let temp = next;
//            next = next->next;
//            m_allocator->free(temp);
//        }
//    }
//
//    template <class Func>
//    void destroy(Func func) {
//        let next = m_head;
//        while (next != nullptr) {
//            let temp = next;
//            next = next->next;
//            func(temp->elem);
//            m_allocator->free(temp);
//        }
//    }
//
//    inline Stack<T> clone() {
//        return clone(Context{.allocator = m_allocator});
//    }
//
//    inline Stack<T> clone(Context context) {
//        let stack = Stack<T>::create(context);
//        let next = m_head;
//
//        while (next != nullptr) {
//            stack.push(next->elem);
//            next = next->next;
//        }
//
//        return stack;
//    }
//
//    inline void push(T element) {
//        m_size++;
//
//        if (m_head == nullptr) {
//            m_head = m_allocator->allocate<Node>().unwrap();
//            m_head->elem = element;
//            return;
//        }
//
//        let node = m_allocator->allocate<Node>().unwrap();
//        node->elem = element;
//        node->next = m_head;
//        m_head = node;
//    }
//
//    Option<T> pop() {
//        m_size--;
//
//        if (m_head == nullptr) return Option<T>::None();
//
//        let elem = m_head->elem;
//        let node = m_head;
//        m_head = m_head->next;
//        m_allocator->free(node);
//        return Option<T>::Some(elem);
//    }
//
//    Option<Ref<T>> peek() {
//        if (m_head == nullptr) return Option<Ref<T>>::None();
//        return Option<Ref<T>>::Some(Ref<T>{&m_head->next});
//    }
//
//    usize size() {
//        return size;
//    }
//
//    struct Node {
//        T     elem;
//        Node* next = nullptr;
//    };
//
//    struct Iter : public ::Iter<T> {
//        Node* m_current;
//
//        Option<Ref<T>> next() override {
//            if (m_current == nullptr) return Option<Ref<T>>::None();
//
//            let elem = m_current->elem;
//            m_current = m_current->next;
//            return Option<Ref<T>>::Some(Ref<T>{&elem});
//        }
//    };
//
//    Iter iter() {
//        Iter iter;
//        iter.m_current = m_head;
//        return iter;
//    };
//
//    Allocator* m_allocator;
//    Node*      m_head = nullptr;
//    usize      m_size = 0;
//};
//
///*
// * It is an immutable string
// */
// struct String {
//    char* m_str;
//    usize m_size;  // includes '\0'
//
//    Allocator* m_allocator;
//
//    /*
//     * uses the str's allocator
//     */
//    inline static String from(String str) {
//        return String::from(Context{.allocator = str.m_allocator}, str);
//    }
//
//    static String from(Context context, String string) {
//        let str = String{};
//        str.m_allocator = context.allocator;
//        str.m_size = string.m_size;
//        str.m_str = str.m_allocator->allocate_array<char>(str.m_size).unwrap();
//        memcpy(str.m_str, string.m_str, str.m_size);
//        return str;
//    }
//
//    inline static String from(std::string cpp_str) {
//        return String::from(default_context(), cpp_str);
//    }
//
//    static String from(Context context, std::string cpp_str) {
//        let str = String{};
//        str.m_allocator = context.allocator;
//        str.m_size = cpp_str.size() + 1;
//        str.m_str = str.m_allocator->allocate_array<char>(str.m_size).unwrap();
//        memcpy(str.m_str, cpp_str.data(), str.m_size - 1);
//        str.m_str[str.m_size - 1] = '\0';
//        return str;
//    }
//
//    inline static String from(const char* c_str) {
//        return String::from(default_context(), c_str);
//    }
//
//    static String from(Context context, const char* c_str) {
//        let str = String{};
//        str.m_allocator = context.allocator;
//        str.m_size = strlen(c_str) + 1;
//        str.m_str = str.m_allocator->allocate_array<char>(str.m_size).unwrap();
//        memcpy(str.m_str, c_str, str.m_size - 1);
//        str.m_str[str.m_size - 1] = '\0';
//        return str;
//    }
//
//    void destroy() {
//        m_allocator->free_array(m_str, m_size);
//    }
//
//    String clone() {
//        return String::from(*this);
//    }
//
//    String clone(Context context) {
//        return String::from(context, *this);
//    }
//
//    usize size() const {
//        return m_size - 1;  // because of '\0'
//    }
//
//    const char* c_str() const {
//        return m_str;
//    }
//
//    Option<Ref<char>> at(usize pos) const {
//        if (pos >= m_size - 1) {
//            return Option<Ref<char>>::None();
//        }
//        return Option<Ref<char>>::Some(Ref<char>{&m_str[pos]});
//    }
//
//    Option<String> substring(usize start_pos, usize len) {
//        return substring(Context{.allocator = m_allocator}, start_pos, len);
//    }
//
//    Option<String> substring(Context context, usize start_pos, usize len) {
//        if (len == 0) return Option<String>::None();
//        if (start_pos + len > size()) return Option<String>::None();
//
//        let new_str = context.allocator->allocate_array<char>(len + 1).unwrap();
//        memcpy(new_str, m_str + start_pos, len);
//        new_str[len] = '\0';
//
//        let str = String{
//            .m_str = new_str,
//            .m_size = len + 1,
//            .m_allocator = context.allocator,
//        };
//        return Option<String>::Some(str);
//    }
//
//    struct Iter : public ::Iter<char> {
//        const String* m_str;
//        usize         m_current = 0;
//
//        Option<Ref<char>> next() override {
//            return m_str->at(m_current++);
//        }
//    };
//
//    Iter iter() const {
//        Iter iter;
//        iter.m_str = this;
//        return iter;
//    };
//};
//
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
