#pragma once

#include <cstdlib>
#include <sys/mman.h>

#include <cstdint>
#include <cstring>
#include <iostream>
#include <sstream>
#include <stack>
#include <unordered_set>

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

    inline ~defer_implementation() { func(); }
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

#define defer_block                                                            \
    auto ANONYMOUS_VARIABLE(__defer_instance) = defer_dummy{} + [&]()
#define defer(body)                                                            \
    auto ANONYMOUS_VARIABLE(__defer_instance) =                                \
        defer_implementation{[&]() { body; }};

void panic(const char *text) {
    std::cerr << "panic:" << std::endl;
    std::cerr << text << std::endl;
    exit(1);
}

void panic(std::string text) {
    std::cerr << "panic:" << std::endl;
    std::cerr << text << std::endl;
    exit(1);
}

void panic() {
    std::cerr << "panic" << std::endl;
    exit(1);
}

template <class v, class err> struct Result {
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

    v unwrap(const char *text) const {
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

    err unwrap_error(const char *text) const {
        if (is_ok()) {
            panic(text);
        }

        return this->error;
    }

    bool is_ok() { return ok; }

    bool is_error() { return !ok; }

  private:
    bool ok;
    v value;
    err error;
};

#define on_ok(r) if (r.is_ok())
#define on_error(r) if (r.is_error())
#define return_on_ok(r, return_value) on_ok(r) return return_value;
#define return_on_error(r, return_value) on_error(r) return return_value;

template <class v> struct Option {
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

    v unwrap(const char *text) const {
        if (!this->some) {
            panic(text);
        }

        return this->value;
    }

    bool is_some() const { return some; }

    bool is_none() const { return !some; }

  private:
    bool some;
    v value;
};

#define on_some(r) if (r.is_some())
#define on_none(r) if (!r.is_some())
#define return_on_some(r, return_value) on_some(r) return return_value;
#define return_on_none(r, return_value) on_none(r) return return_value;

#include <type_traits>

/*
    Template meta programing c++ magic shit
    Do not question, just run ....
*/
template <typename T>
concept HasToString = requires(const T &t) {
    std::is_member_function_pointer_v<decltype(&T::to_string)>;
};
template <HasToString T> void print(const T &obj) {
    std::cout << obj.to_string();
}

template <class T> void print(const T &obj) { std::cout << obj; }

template <class... Arg> void print(Arg... arg) { (print(arg), ...); }

template <class... Arg> void println(Arg... arg) {
    (print(arg), ...);
    std::cout << std::endl;
}

namespace Private {
struct Characters {
    const char *format;
    usize position = 0;

    char next() {
        let current = position;
        position++;
        return *(format + current);
    }
};

template <class T>
void format_helper(Characters &characters, const T &obj,
                   std::stringstream &ss) {
    while (true) {
        let ch = characters.next();
        if (ch == '%') {
            ss << obj;
            break;
        }
        ss << ch;
    }
}

template <HasToString T>
void format_helper(Characters &characters, const T &obj,
                   std::stringstream &ss) {
    while (true) {
        let ch = characters.next();
        if (ch == '%') {
            ss << obj.to_string();
            break;
        }
        ss << ch;
    }
}
} // namespace Private

template <class... Arg> std::string format(const char *fmt, const Arg &...arg) {
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

template <class... Arg> void print_format(const char *fmt, Arg... arg) {
    print(format(fmt, arg...));
}

template <class... Arg> void println_format(const char *fmt, Arg... arg) {
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
    void *ptr;
    usize len;

    Option<Raw> chunck(usize new_len, usize offset = 0) const {
        if (new_len + offset > len) {
            return Option<Raw>::None();
        }
        return Option<Raw>::Some(Raw{
            .ptr = (byte *)ptr + offset,
            .len = new_len,
        });
    }

    template <class T> Option<T *> cast() const {
        if (this->len < sizeof(T))
            return Option<T *>::None();
        return Option<T *>::Some((T *)this->ptr);
    }

    template <class T> Option<T *> cast_array(usize size) const {
        if (this->len < sizeof(T) * size)
            return Option<T *>::None();
        return Option<T *>::Some((T *)this->ptr);
    }
};

Option<Raw> allocate(usize size) {
    var *ptr = mmap(NULL, size, PROT_READ | PROT_WRITE,
                    MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
    if (ptr == (void *)-1) {
        return Option<Raw>::None();
    }

    return Option<Raw>::Some(Raw{
        .ptr = ptr,
        .len = size,
    });
}

void free(const Raw &raw) { munmap(raw.ptr, raw.len); }

struct Allocator {
    virtual Option<Raw> allocate_raw(usize) = 0;
    virtual void free_raw(const Raw &) = 0;

    template <class T> Option<T *> allocate() {
        let raw_option = this->allocate_raw(sizeof(T));
        on_none(raw_option) return Option<T *>::None();

        let raw = raw_option.unwrap();
        return Option<T *>::Some((T *)raw.ptr);
    }

    template <class T> Option<T *> allocate_array(usize size) {
        let raw_option = this->allocate_raw(sizeof(T) * size);
        on_none(raw_option) return Option<T *>::None();

        let raw = raw_option.unwrap();
        return Option<T *>::Some((T *)raw.ptr);
    }

    template <class T> void free(const T *ptr) {
        let raw = Raw{
            .ptr = (void *)ptr,
            .len = sizeof(T),
        };
        this->free_raw(raw);
    }

    template <class T> void free_array(const T *ptr, usize size) {
        let raw = Raw{
            .ptr = (void *)ptr,
            .len = sizeof(T) * size,
        };
        this->free_raw(raw);
    }
};

struct BasicAllocator : public Allocator {
    Option<Raw> allocate_raw(usize size) override { return ::allocate(size); };

    void free_raw(const Raw &raw) override { ::free(raw); };
};

template <usize max_size = 2 * KiB>
struct TemporaryAllocator : public Allocator {
    Option<Raw> allocate_raw(usize size) override {
        if (m_current + size > max_size)
            m_current = 0;

        Raw raw{
            .ptr = (void *)(m_memory + m_current),
            .len = size,
        };

        m_current += size;

        return Option<Raw>::Some(raw);
    }

    void free_raw(const Raw &raw) override {}

  private:
    byte m_memory[max_size];
    usize m_current = 0;
};

template <usize max_size = 2 * KiB> struct StackAllocator : public Allocator {
    Option<Raw> allocate_raw(usize size) override {
        if (m_current + size > max_size)
            return Option<Raw>::None();

        let raw = Raw{
            .ptr = m_memory + m_current,
            .len = size,
        };

        return Option<Raw>::Some(raw);
    }

    void free_raw(const Raw &raw) {
        if (raw.ptr == m_memory + m_current - raw.len) {
            m_current -= raw.len;
        }
    }

    void free_all() { m_current = 0; }

  private:
    byte m_memory[max_size];
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

    void free_raw(const Raw &raw) override {
        m_allocations -= raw.len;

        return ::free(raw);
    };

    void panic_on_leak() {
        if (m_allocations != 0)
            panic(format("Debug allocator definetly lost % bytes",
                         m_allocations));
    }
};

template <usize page_size = 2 * KiB> struct PageAllocator : public Allocator {

    Option<Raw> allocate_raw(usize size) override {
        if (size > page_size)
            return Option<Raw>::None();

        if (m_head == nullptr) {
            let raw_option = ::allocate(sizeof(Page));
            on_none(raw_option) return Option<Raw>::None();
            let raw = raw_option.unwrap();

            m_head = (Page *)raw.ptr;
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
        let new_page = (Page *)new_page_option.unwrap().ptr;

        new_page->next = m_head;
        m_head = new_page;

        let ptr = m_head->memory;
        m_pos = size;

        return Option<Raw>::Some({
            .ptr = ptr,
            .len = size,
        });
    };

    void free_raw(const Raw &raw) override{

    };

    void free_all() { destroy_page(m_head); }

    struct Page {
        byte memory[page_size];
        Page *next = nullptr;
    };

    void destroy_page(Page *page) {
        if (page == nullptr)
            return;
        destroy_page(page->next);
        ::free({.ptr = page, .len = sizeof(Page)});
    }

    Page *m_head = nullptr;
    usize m_pos = 0;
};

/*
    Ring allocator with 2 KiB of data
*/
TemporaryAllocator<2 * KiB> temporary_allocator;

/*
    Basic allocator, allocates on heap
*/
BasicAllocator allocator;

/*
    ADT
*/

/* iter:
 *  iter.next() -> Option<T>
 */
template <class Iter, class Function>
void inline for_each(Iter iter, Function func) {
    var item_option = iter.next();
    while (item_option.is_some()) {
        var &item = item_option.unwrap().deref();
        func(item);
        item_option = iter.next();
    }
}

template <class T> struct Ref {
    T *ptr;
    T &deref() { return *ptr; }
};

// Dynamic list
template <class T> struct List {
    static List<T> create(Allocator &allocator) {
        List<T> list;
        list.capacity = 1;
        list.length = 0;
        list.allocator = &allocator;

        list.memory = list.allocator->template allocate_array<T>(1).unwrap(
            "List: constructor");
        return list;
    }

    static List<T> create(usize size, Allocator &allocator) {
        List<T> list;
        list.capacity = size;
        list.length = 0;
        list.allocator = &allocator;

        list.memory = list.allocator->template allocate_array<T>(size).unwrap(
            "List: constructor");
    }

    void destroy() const {
        println("Destroy");
        allocator->free_array(memory, capacity);
    }

    void destroy(std::function<void(const T &)> destroy_function) const {
        for (usize i = 0; i < length; i++) {
            destroy_function(this->memory[i]);
        }

        allocator->free_array(memory, capacity);
    }

    void clear() { length = 0; }

    void add(T element) {
        if (length + 1 > capacity)
            reserve(capacity * 2);

        this->memory[this->length] = element;
        this->length++;
    }

    void remove(usize position) {
        for (size_t i = position; i < length - 1; i++) {
            memory[i] = memory[i + 1];
        }
        length -= 1;
    }

    Option<Ref<T>> at(usize position) const {
        if (position >= length) {
            return Option<Ref<T>>::None();
        }
        return Option<Ref<T>>::Some(Ref<T>{&this->memory[position]});
    }
    T *ptr() const { return this->memory; }
    usize size() const { return length; }
    bool empty() const { return length == 0; }
    void reserve(usize size) {
        if (size <= this->capacity)
            return;

        T *new_memory =
            allocator->allocate_array<T>(size).unwrap("List reserve");

        for (usize i = 0; i < this->length; i++) {
            new_memory[i] = this->memory[i];
        }

        allocator->free_array(this->memory, this->capacity);

        this->memory = new_memory;
        this->capacity = size;
    }

    List<T> clone() const {
        List<T> list{capacity};
        list.length = length;

        for (usize i = 0; i < length; i++) {
            list.memory[i] = this->memory[i];
        }

        return list;
    }

    List<T> clone(std::function<T(T &)> clone_func) const {
        List<T> list{capacity};
        list.length = length;

        for (usize i = 0; i < length; i++) {
            list.memory[i] = clone_func(this->memory[i]);
        }

        return list;
    }

    struct Iter {
        Iter(List<T> &list) : list(list) {}

        Option<Ref<T>> next() {
            let el = list.at(current);
            current++;
            return el;
        }

      private:
        List<T> &list;
        usize current = 0;
    };

    Iter iter() { return Iter{*this}; }

  private:
    T *memory;
    usize capacity;
    usize length;
    Allocator *allocator;
};
