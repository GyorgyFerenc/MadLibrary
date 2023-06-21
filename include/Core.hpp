#pragma once

#include <alloca.h>
#include <sys/mman.h>

#include <cassert>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <memory>
#include <regex>
#include <sstream>
#include <stack>
#include <unordered_set>

#define UNREACHABLE assert(false && "uncreachable");

using uint = unsigned int;
using int8 = int8_t;
using int16 = int16_t;
using int32 = int32_t;
using int64 = int64_t;
using uint8 = uint8_t;
using uint16 = uint16_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using usize = size_t;
using byte = unsigned char;

#define var auto
#define let auto
#define lambda(...) [&](__VA_ARGS__)
#define fn lambda

#include <functional>

struct defer_implementation {
    std::function<void()> func;

    inline ~defer_implementation() {
        func();
    }
};

struct defer_dummy {
    inline defer_implementation operator+(std::function<void()> func) {
        return defer_implementation{func};
    }
};

#define CONCAT_IMPL(str1, str2) str1##str2
#define CONCAT(str1, str2) CONCAT_IMPL(str1, str2)

#ifdef __COUNTER__
#define ANONYMOUS_VARIABLE(str) CONCAT(str, __COUNTER__)
#else
#define ANONYMOUS_VARIABLE(str) CONCAT(str, __LINE__)
#endif

#define defer_block auto ANONYMOUS_VARIABLE(__defer_instance) = defer_dummy{} + [&]()
#define defer(body) \
    auto ANONYMOUS_VARIABLE(__defer_instance) = defer_implementation{[&]() { body; }};

void panic(const char* text) {
    std::cout << "panic:" << std::endl;
    std::cout << text << std::endl;
    exit(1);
}

void panic(std::string text) {
    std::cout << "panic:" << std::endl;
    std::cout << text << std::endl;
    exit(1);
}

void panic() {
    std::cout << "panic" << std::endl;
    exit(1);
}

template <class... Arg>
std::string format(const char* fmt, const Arg&... arg);

template <class... Arg>
void panic_format(const char* fmt, Arg... arg) {
    panic(format(fmt, arg...));
}

template <class v, class err>
struct Result {
    static Result<v, err> Ok(v value) {
        var result = Result<v, err>{};
        result.ok = true;
        result.value = value;
        return result;
    }

    static Result<v, err> Error(err error) {
        var result = Result<v, err>{};
        result.ok = false;
        result.error = error;
        return result;
    }

    v unwrap() const {
        if (is_error()) {
            panic("unwrap panic");
        }

        return this->value;
    }

    v unwrap(const char* text) const {
        if (is_error()) {
            panic(text);
        }

        return this->value;
    }

    err unwrap_error() const {
        if (is_ok()) {
            panic("unwrap_error panic");
        }

        return this->error;
    }

    err unwrap_error(const char* text) const {
        if (is_ok()) {
            panic(text);
        }

        return this->error;
    }

    bool is_ok() {
        return ok;
    }

    bool is_error() {
        return !ok;
    }

   private:
    bool ok;
    v    value;
    err  error;
};

#define on_ok(r) if (r.is_ok())
#define on_error(r) if (r.is_error())
#define return_on_ok(r, return_value) on_ok(r) return return_value;
#define return_on_error(r, return_value) on_error(r) return return_value;

template <class v>
struct Option {
    static Option<v> Some(v value) {
        var option = Option<v>{};
        option.some = true;
        option.value = value;
        return option;
    }

    static Option<v> None() {
        var s = Option<v>{};
        s.some = false;
        return s;
    }

    v unwrap() const {
        if (!this->some) {
            panic("unwrap panic");
        }

        return this->value;
    }

    v unwrap(const char* text) const {
        if (!this->some) {
            panic(text);
        }

        return this->value;
    }

    bool is_some() const {
        return some;
    }

    bool is_none() const {
        return !some;
    }

   private:
    bool some;
    v    value;
};

#define on_some(r) if (r.is_some())
#define on_none(r) if (!r.is_some())
#define return_on_some(r, return_value) on_some(r) return return_value;
#define return_on_none(r, return_value) on_none(r) return return_value;

template <class T>
void print(const T& obj) {
    std::cout << obj;
}

template <class... Arg>
void print(Arg... arg) {
    (print(arg), ...);
}

template <class... Arg>
void println(Arg... arg) {
    (print(arg), ...);
    std::cout << std::endl;
}

namespace Private {
struct Characters {
    const char* format;
    usize       position = 0;

    char next() {
        let current = position;
        position++;
        return format[current];
    }
};

template <class T>
void format_helper(Characters& characters, const T& obj, std::stringstream& ss) {
    while (true) {
        let ch = characters.next();
        if (ch == '%') {
            ss << obj;
            break;
        }
        ss << ch;
    }
}

}  // namespace Private

template <class... Arg>
std::string format(const char* fmt, const Arg&... arg) {
    var characters = Private::Characters{
        .format = fmt,
        .position = 0,
    };

    std::stringstream ss;
    (Private::format_helper(characters, arg, ss), ...);

    let len = strlen(fmt);

    while (characters.position < len) {
        let chr = characters.next();
        ss << chr;
    }

    return ss.str();
}

template <class... Arg>
void print_format(const char* fmt, Arg... arg) {
    print(format(fmt, arg...));
}

template <class... Arg>
void println_format(const char* fmt, Arg... arg) {
    println(format(fmt, arg...));
}

/*
    Allocators
*/

#define KiB 1024
#define MiB 1024 * KiB
#define GiB 1024 * MiB
#define TiB 1024 * GiB

#define KB 1000
#define MB 1000 * KB
#define GB 1000 * MB
#define TB 1000 * GB

struct Raw {
    void* ptr;
    usize len;

    Option<Raw> chunck(usize new_len, usize offset = 0) const {
        if (new_len + offset > len) {
            return Option<Raw>::None();
        }
        return Option<Raw>::Some(Raw{
            .ptr = (byte*)ptr + offset,
            .len = new_len,
        });
    }

    template <class T>
    Option<T*> cast() const {
        if (this->len < sizeof(T)) return Option<T*>::None();
        return Option<T*>::Some((T*)this->ptr);
    }

    template <class T>
    Option<T*> cast_array(usize size) const {
        if (this->len < sizeof(T) * size) return Option<T*>::None();
        return Option<T*>::Some((T*)this->ptr);
    }
};

Option<Raw> allocate(usize size) {
    var* ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (ptr == (void*)-1) {
        return Option<Raw>::None();
    }

    return Option<Raw>::Some(Raw{
        .ptr = ptr,
        .len = size,
    });
}

void free(const Raw& raw) {
    munmap(raw.ptr, raw.len);
}

struct Allocator {
    virtual Option<Raw> allocate_raw(usize) = 0;
    virtual void        free_raw(const Raw&) = 0;

    template <class T>
    Option<T*> allocate() {
        let raw_option = this->allocate_raw(sizeof(T));
        on_none(raw_option) return Option<T*>::None();

        let raw = raw_option.unwrap();
        return Option<T*>::Some((T*)raw.ptr);
    }

    template <class T>
    Option<T*> allocate_array(usize size) {
        let raw_option = this->allocate_raw(sizeof(T) * size);
        on_none(raw_option) return Option<T*>::None();

        let raw = raw_option.unwrap();
        return Option<T*>::Some((T*)raw.ptr);
    }

    template <class T>
    void free(const T* ptr) {
        let raw = Raw{
            .ptr = (void*)ptr,
            .len = sizeof(T),
        };
        this->free_raw(raw);
    }

    template <class T>
    void free_array(const T* ptr, usize size) {
        let raw = Raw{
            .ptr = (void*)ptr,
            .len = sizeof(T) * size,
        };
        this->free_raw(raw);
    }
};

struct BasicAllocator : public Allocator {
    Option<Raw> allocate_raw(usize size) override {
        return ::allocate(size);
    };

    void free_raw(const Raw& raw) override {
        ::free(raw);
    };
};

template <usize max_size = 2 * KiB>
struct TemporaryAllocator : public Allocator {
    Option<Raw> allocate_raw(usize size) override {
        if (m_current + size > max_size) m_current = 0;

        Raw raw{
            .ptr = (void*)(m_memory + m_current),
            .len = size,
        };

        m_current += size;

        return Option<Raw>::Some(raw);
    }

    void free_raw(const Raw& raw) override {
    }

   private:
    byte  m_memory[max_size];
    usize m_current = 0;
};

template <usize max_size = 2 * KiB>
struct StackAllocator : public Allocator {
    Option<Raw> allocate_raw(usize size) override {
        if (m_current + size > max_size) return Option<Raw>::None();

        let raw = Raw{
            .ptr = m_memory + m_current,
            .len = size,
        };

        m_current += size;

        return Option<Raw>::Some(raw);
    }

    void free_raw(const Raw& raw) override {
        if (raw.ptr == m_memory + m_current - raw.len) {
            m_current -= raw.len;
        }
    }

    void free_all() {
        m_current = 0;
    }

   private:
    byte  m_memory[max_size];
    usize m_current = 0;
};

/*
    Make it better
*/
struct DebugAllocator : public Allocator {
    usize m_allocations = 0;

    Option<Raw> allocate_raw(usize size) override {
        m_allocations += size;

        return ::allocate(size);
    };

    void free_raw(const Raw& raw) override {
        m_allocations -= raw.len;

        return ::free(raw);
    };

    void panic_on_leak() {
        if (m_allocations != 0)
            panic_format("Debug allocator definetly lost % bytes", m_allocations);
    }
};

template <usize page_size = 2 * KiB>
struct PageAllocator : public Allocator {
    Option<Raw> allocate_raw(usize size) override {
        if (size > page_size) return Option<Raw>::None();

        if (m_head == nullptr) {
            let raw_option = ::allocate(sizeof(Page));
            on_none(raw_option) return Option<Raw>::None();
            let raw = raw_option.unwrap();

            m_head = (Page*)raw.ptr;
            m_pos = size;

            let allocated_raw = m_head->memory;
            return Option<Raw>::Some({
                .ptr = allocated_raw,
                .len = size,
            });
        }

        if (m_pos + size <= page_size) {
            let ptr = m_head->memory + m_pos;
            m_pos += size;

            return Option<Raw>::Some({
                .ptr = ptr,
                .len = size,
            });
        }

        let new_page_option = ::allocate(sizeof(Page));
        on_none(new_page_option) return Option<Raw>::None();
        let new_page = (Page*)new_page_option.unwrap().ptr;

        new_page->next = m_head;
        m_head = new_page;

        let ptr = m_head->memory;
        m_pos = size;

        return Option<Raw>::Some({
            .ptr = ptr,
            .len = size,
        });
    };

    void free_raw(const Raw& raw) override{

    };

    void free_all() {
        destroy_page(m_head);
    }

    struct Page {
        byte  memory[page_size];
        Page* next = nullptr;
    };

    void destroy_page(Page* page) {
        if (page == nullptr) return;
        destroy_page(page->next);
        ::free({.ptr = page, .len = sizeof(Page)});
    }

    Page* m_head = nullptr;
    usize m_pos = 0;
};

struct Context {
    Allocator* allocator;
    Allocator* temp_allocator;
};

inline Context default_context() {
    /*
       Ring allocator with 2 MiB of data
    */
    static TemporaryAllocator<2 * MiB> temporary_allocator;
    /*
       Basic allocator, allocates on heap
    */
    static BasicAllocator allocator;

    static let context = Context{
        .allocator = &allocator,
        .temp_allocator = &temporary_allocator,
    };

    return context;
}

/*
    ADT
*/

/*
 * We need this because a template cannot be a reference
 *
 * Option<int&> is error;
 * Option<Ref<int>> is the same as above
 */
template <class T>
struct Ref {
    T*        ptr;
    inline T& deref() {
        return *ptr;
    }
};

// Maybe do it with macro ??
template <class T>
struct Iter {
    virtual Option<Ref<T>> next() = 0;

    template <class Function>
    void inline for_each(Function func) {
        let item_option = this->next();
        while (item_option.is_some()) {
            let& item = item_option.unwrap().deref();
            func(item);
            item_option = this->next();
        }
    }
};

// Dynamic list
template <class T>
struct List {
    static List<T> create() {
        return List<T>::create(default_context());
    }

    static List<T> create(Context context) {
        List<T> list;
        list.m_capacity = 1;
        list.m_size = 0;
        list.m_allocator = context.allocator;

        list.m_ptr = list.m_allocator->template allocate_array<T>(1).unwrap("List: constructor");
        return list;
    }

    static List<T> create(Context context, usize capacity) {
        List<T> list;
        list.m_capacity = capacity;
        list.m_size = 0;
        list.m_allocator = context.allocator;

        list.m_ptr =
            list.m_allocator->template allocate_array<T>(capacity).unwrap("List: constructor");

        return list;
    }

    void destroy() const {
        m_allocator->free_array(m_ptr, m_capacity);
    }

    template <class Func>
    void destroy(Func destroy_function) const {
        for (usize i = 0; i < m_size; i++) {
            destroy_function(this->m_ptr[i]);
        }

        m_allocator->free_array(m_ptr, m_capacity);
    }

    void clear() {
        m_size = 0;
    }

    void add(T element) {
        if (m_size + 1 > m_capacity) reserve(m_capacity * 2);

        this->m_ptr[this->m_size] = element;
        this->m_size++;
    }

    void remove(usize position) {
        for (size_t i = position; i < m_size - 1; i++) {
            m_ptr[i] = m_ptr[i + 1];
        }
        m_size -= 1;
    }

    Option<Ref<T>> at(usize position) const {
        if (position >= m_size) {
            return Option<Ref<T>>::None();
        }
        return Option<Ref<T>>::Some(Ref<T>{&this->m_ptr[position]});
    }

    T* ptr() const {
        return this->m_ptr;
    }
    usize size() const {
        return m_size;
    }
    bool empty() const {
        return m_size == 0;
    }

    void reserve(usize size) {
        if (size <= this->m_capacity) return;

        T* new_memory = m_allocator->allocate_array<T>(size).unwrap("List reserve");

        for (usize i = 0; i < this->m_size; i++) {
            new_memory[i] = this->m_ptr[i];
        }

        m_allocator->free_array(this->m_ptr, this->m_capacity);

        this->m_ptr = new_memory;
        this->m_capacity = size;
    }

    List<T> clone() const {
        return clone(Context{.allocator = m_allocator});
    }

    List<T> clone(Context context) const {
        let list = List<T>::create(context, this->m_capacity);

        for (usize i = 0; i < m_size; i++) {
            list.add(this->m_ptr[i]);
        }

        return list;
    }

    struct Iter : public ::Iter<T> {
        Iter(List<T>& list) : list(list){};

        Option<Ref<T>> next() override {
            let el = list.at(current);
            current++;
            return el;
        }

        List<T>& list;
        usize    current = 0;
    };

    Iter iter() {
        return Iter{*this};
    }

    T*         m_ptr;
    usize      m_capacity;
    usize      m_size;
    Allocator* m_allocator;
};

/*
 * This implementation uses a linked list
 */
template <class T>
struct Stack {
    static Stack<T> create() {
        return Stack<T>::create(default_context());
    }

    static Stack<T> create(Context context) {
        Stack<T> s;
        s.m_allocator = context.allocator;
        s.m_head = nullptr;
        s.m_size = 0;

        return s;
    }

    void destroy() {
        let next = m_head;
        while (next != nullptr) {
            let temp = next;
            next = next->next;
            m_allocator->free(temp);
        }
    }

    template <class Func>
    void destroy(Func func) {
        let next = m_head;
        while (next != nullptr) {
            let temp = next;
            next = next->next;
            func(temp->elem);
            m_allocator->free(temp);
        }
    }

    inline Stack<T> clone() {
        return clone(Context{.allocator = m_allocator});
    }

    inline Stack<T> clone(Context context) {
        let stack = Stack<T>::create(context);
        let next = m_head;

        while (next != nullptr) {
            stack.push(next->elem);
            next = next->next;
        }

        return stack;
    }

    inline void push(T element) {
        m_size++;

        if (m_head == nullptr) {
            m_head = m_allocator->allocate<Node>().unwrap();
            m_head->elem = element;
            return;
        }

        let node = m_allocator->allocate<Node>().unwrap();
        node->elem = element;
        node->next = m_head;
        m_head = node;
    }

    Option<T> pop() {
        m_size--;

        if (m_head == nullptr) return Option<T>::None();

        let elem = m_head->elem;
        let node = m_head;
        m_head = m_head->next;
        m_allocator->free(node);
        return Option<T>::Some(elem);
    }

    Option<Ref<T>> peek() {
        if (m_head == nullptr) return Option<Ref<T>>::None();
        return Option<Ref<T>>::Some(Ref<T>{&m_head->next});
    }

    usize size() {
        return size;
    }

    struct Node {
        T     elem;
        Node* next = nullptr;
    };

    struct Iter : public ::Iter<T> {
        Node* m_current;

        Option<Ref<T>> next() override {
            if (m_current == nullptr) return Option<Ref<T>>::None();

            let elem = m_current->elem;
            m_current = m_current->next;
            return Option<Ref<T>>::Some(Ref<T>{&elem});
        }
    };

    Iter iter() {
        Iter iter;
        iter.m_current = m_head;
        return iter;
    };

    Allocator* m_allocator;
    Node*      m_head = nullptr;
    usize      m_size = 0;
};

/*
 * It is an immutable string
 */
struct String {
    char* m_str;
    usize m_size;  // includes '\0'

    Allocator* m_allocator;

    void destroy() {
        m_allocator->free_array(m_str, m_size);
    }

    usize size() const {
        return m_size - 1;  // because of '\0'
    }

    const char* c_str() const {
        return m_str;
    }

    Option<Ref<char>> at(usize pos) const {
        if (pos >= m_size - 1) {
            return Option<Ref<char>>::None();
        }
        return Option<Ref<char>>::Some(Ref<char>{&m_str[pos]});
    }

    Option<String> substring(usize start_pos, usize len) {
        return substring(Context{.allocator = m_allocator}, start_pos, len);
    }

    Option<String> substring(Context context, usize start_pos, usize len) {
        if (len == 0) return Option<String>::None();
        if (start_pos + len > size()) return Option<String>::None();

        let new_str = context.allocator->allocate_array<char>(len + 1).unwrap();
        memcpy(new_str, m_str + start_pos, len);
        new_str[len] = '\0';

        let str = String{
            .m_str = new_str,
            .m_size = len + 1,
            .m_allocator = context.allocator,
        };
        return Option<String>::Some(str);
    }

    struct Iter : public ::Iter<char> {
        const String* m_str;
        usize         m_current = 0;

        Option<Ref<char>> next() override {
            return m_str->at(m_current++);
        }
    };

    Iter iter() const {
        Iter iter;
        iter.m_str = this;
        return iter;
    };
};

std::ostream& operator<<(std::ostream& os, const String& string) {
    return os << string.m_str;
}

struct StringBuilder {
    // TODO: add capacity

    Allocator* m_allocator;

    char* m_str;
    usize m_size;  // includes '\0'

    static StringBuilder create() {
        return StringBuilder::create(default_context());
    }

    static StringBuilder create(Context context) {
        return StringBuilder{
            .m_allocator = context.allocator,
            .m_str = context.allocator->allocate_array<char>(1).unwrap(),
            .m_size = 1,
        };
    }

    void destroy() {
        m_allocator->free_array(m_str, m_size);
    }

    void resize(usize new_size) {
        if (new_size <= m_size) return;

        let new_str = m_allocator->allocate_array<char>(new_size).unwrap();
        memcpy(new_str, m_str, m_size);

        m_allocator->free_array(m_str, m_size);
        m_size = new_size;
        m_str = new_str;
    }

    void add(String str) {
        add(str.m_str, str.size());
    }

    void add(const char* c_str) {
        add(c_str, strlen(c_str));
    }

    void add(const char* c_str, usize c_size) {
        let old_size = m_size;
        resize(m_size + c_size);

        memcpy(m_str + old_size - 1, c_str, c_size);

        m_str[m_size - 1] = '\0';
    }

    void add(std::string cpp_str) {
        let old_size = m_size;
        resize(m_size + cpp_str.size());
        memcpy(m_str + old_size - 1, cpp_str.data(), cpp_str.size());
        m_str[m_size - 1] = '\0';
    }

    String build() {
        let str = m_allocator->allocate_array<char>(m_size).unwrap();
        memcpy(str, m_str, m_size);
        return String{
            .m_str = str,
            .m_size = m_size,
            .m_allocator = m_allocator,
        };
    }
};

struct StringView {
    String m_string;
    usize  m_start = 0;  // inclusive
    usize  m_end;        // inclusive

    using CharOption = Option<Ref<char>>;

    inline static StringView create(String str) {
        StringView view;
        view.m_string = str;
        view.m_start = 0;
        view.m_end = str.size() - 1;  //[m_start, m_end]
        return view;
    }

    inline StringView clone() {
        return *this;
    }

    usize size() {
        return m_end - m_start + 1;
    }

    CharOption at(usize position) {
        if (position > m_end) return CharOption::None();
        return m_string.at(m_start + position);
    }

    Option<String> substring(Context context, usize position, usize len) {
        if (m_start + position + len - 1 > m_end) return Option<String>::None();

        return m_string.substring(context, position + m_start, len);
    }

    Option<String> substring(usize position, usize len) {
        if (m_start + position + len - 1 > m_end) return Option<String>::None();

        return m_string.substring(position + m_start, len);
    }

    void remove_prefix(usize number) {
        m_start += number;
    }

    void remove_suffix(usize number) {
        m_end -= number;
        if (m_end < 0) m_end = 0;
    }

    bool starts_with(const char* prefix) {
        for (usize i = 0; prefix[i] != '\0'; i++) {
            let chr_option = at(i);
            on_none(chr_option) return false;
            let chr = chr_option.unwrap().deref();

            if (prefix[i] != chr) return false;
        }
        return true;
    }

    bool ends_with(const char* suffix) {
        let suffix_len = strlen(suffix);
        let self_len = size();

        if (suffix_len > self_len) return false;

        for (usize i = 0; i < suffix_len; i++) {
            let self_chr = at(self_len - i - 1).unwrap("ends_with").deref();
            let suffix_chr = suffix[suffix_len - i - 1];

            if (suffix_chr != self_chr) return false;
        }

        return true;
    }

    struct Iter : public ::Iter<char> {
        const StringView* m_view;
        usize             m_current = 0;

        inline CharOption next() override {
            if (m_current > m_view->m_end) return CharOption::None();
            let chr = m_view->m_string.at(m_current);

            m_current++;
            return chr;
        };
    };

    Iter iter() const {
        Iter it;
        it.m_view = this;
        it.m_current = m_start;
        return it;
    }
    friend std::ostream& operator<<(std::ostream&, const StringView&);
};

std::ostream& operator<<(std::ostream& os, const StringView& string) {
    string.iter().for_each([&](char chr) { os << chr; });
    return os;
}
