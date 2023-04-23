#pragma once

#include <stdint.h>
#include <sys/mman.h>

#include <iostream>
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

#define let const auto
#define var auto

#include <functional>

struct defer_implementation {
    std::function<void()> func;

    ~defer_implementation() {
        func();
    }
};

struct defer_dummy {
    defer_implementation operator+(std::function<void()> func) {
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

#define KiB 1024
#define MiB 1024 * KiB
#define GiB 1024 * MiB
#define TiB 1024 * GiB

#define KB 1000
#define MB 1000 * KB
#define GB 1000 * MB
#define TB 1000 * GB

void panic(const char* text) {
    std::cerr << "panic:" << std::endl;
    std::cerr << text << std::endl;
    exit(1);
}

void panic() {
    std::cerr << "panic" << std::endl;
    exit(1);
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

/*
    It is a wrapper around allocate and free
    It is very dump
    He doesnt know about what it allocates
*/
struct BasicAllocator {
    Option<Raw> allocate(usize size) {
        return ::allocate(size);
    }

    // It is false becuase it cannot be decided
    bool own(const Raw& raw) const {
        return false;
    }

    void free(const Raw& raw) {
        ::free(raw);
    }
    // It does nothing
    void free_all() {
    }
};

/*
    It uses malloc
    It is dump
    He doesnt know about what it allocates
*/
struct MAllocator {
    Option<Raw> allocate(usize size) {
        let ptr = malloc(size);
        if (ptr == NULL) return Option<Raw>::None();

        return Option<Raw>::Some(Raw{
            .ptr = ptr,
            .len = size,
        });
    }

    // It is false becuase it cannot be decided
    bool own(const Raw& raw) const {
        return false;
    }

    void free(const Raw& raw) {
        ::free(raw.ptr);  // malloc free
    }

    // It does nothing
    void free_all() {
    }
};

/*
    TODO: Add better name
    It is like BasicAllocator but
    uses 1 more byte for each allocataion
    Maybe bad idea? IDK
*/
struct BasicAllocator2 {
    Option<Raw> allocate(usize size) {
        let raw_option = ::allocate(size + 1);
        on_none(raw_option) return Option<Raw>::None();

        let raw = raw_option.unwrap();
        let requested = raw.chunck(size, 1);
        on_none(requested) return Option<Raw>::None();

        *(byte*)raw.ptr = 0b01010101;  // Mark the fact that it is allocated by BasicAllocator2

        return requested;
    }

    bool own(const Raw& raw) const {
        let mark = (byte*)raw.ptr - 1;
        return *mark == 0b01010101;
    }

    void free(const Raw& raw) {
        var full = raw;
        full.ptr = (byte*)full.ptr - 1;
        full.len += 1;
        ::free(full);
    }

    // It does nothing
    void free_all() {
    }
};

// keep tracks of allocations in a hashset
struct SetAllocator {
    // TODO implement
};

template <class First, class Secondary = BasicAllocator>
struct FallBackAllocator {
    Option<Raw> allocate(usize size) {
        let raw = first.allocate(size);
        on_some(raw) return raw;

        return secondary.allocate(size);
    }

    bool own(const Raw& raw) const {
        return first.own(raw) || secondary.own(raw);
    }

    void free(const Raw& raw) {
        if (first.own(raw)) first.free(raw);
        secondary.free(raw);
    }

    void free_all() {
        first.free_all();
        secondary.free_all();
    }

   private:
    First     first;
    Secondary secondary;
};

template <usize max_size = 2 * KB>
struct StackAllocator {
    Option<Raw> allocate(usize size) {
        if (current + size > max_size) return Option<Raw>::None();

        Raw raw{
            .ptr = (void*)(memory + current),
            .len = size,
        };

        current += size;

        return Option<Raw>::Some(raw);
    }

    bool own(const Raw& raw) const {
        bool left_bound = (void*)memory <= raw.ptr;
        bool right_bound = raw.ptr <= (void*)(memory + current - raw.len);

        return left_bound && right_bound;
    }

    void free(const Raw& raw) {
        bool last = ((byte*)raw.ptr + raw.len) == (memory + current);
        if (last) current -= raw.len;
    }

    void free_all() {
        current = 0;
    }

   private:
    byte  memory[max_size];
    usize current = 0;
};

template <class Allocator = BasicAllocator>
struct MultiTypeAllocator {
    template <class T>
    Option<T*> allocate() {
        let raw_option = allocator.allocate(sizeof(T));
        on_none(raw_option) return Option<T*>::None();

        let raw = raw_option.unwrap();
        return Option<T*>::Some((T*)raw.ptr);
    }

    template <class T>
    bool own(const T* ptr) const {
        let raw = to_raw(ptr);
        return allocator.own(raw);
    }

    template <class T>
    void free(const T* ptr) {
        let raw = to_raw(ptr);
        allocator.free(raw);
    }

    void free_all() {
        allocator.free_all();
    }

   private:
    Allocator allocator;

    template <class T>
    Raw to_raw(const T* ptr) const {
        return Raw{
            .ptr = (void*)ptr,
            .len = sizeof(T),
        };
    }
};

template <class T, class Allocator = BasicAllocator>
struct TypeAllocator {
    Option<T*> allocate() {
        let raw_option = allocator.allocate(sizeof(T));
        on_none(raw_option) return Option<T*>::None();

        let raw = raw_option.unwrap();
        return Option<T*>::Some((T*)raw.ptr);
    }

    Option<T*> allocate_array(usize size) {
        let raw_option = allocator.allocate(sizeof(T) * size);
        on_none(raw_option) return Option<T*>::None();

        let raw = raw_option.unwrap();
        return Option<T*>::Some((T*)raw.ptr);
    }

    bool own(const T* ptr) const {
        let raw = to_raw(ptr);
        return allocator.own(raw);
    }

    void free(const T* ptr) {
        let raw = to_raw(ptr);
        allocator.free(raw);
    }

    void free_array(const T* ptr, usize size) {
        let raw = Raw{
            .ptr = (void*)ptr,
            .len = sizeof(T) * size,
        };
        allocator.free(raw);
    }

    void free_all() {
        allocator.free_all();
    }

   private:
    Allocator allocator;

    Raw to_raw(const T* ptr) const {
        return Raw{
            .ptr = (void*)ptr,
            .len = sizeof(T),
        };
    }
};

template <class T>
struct Ref {
    T* ptr;
    T& deref() {
        return *ptr;
    }
};

#define for_each(variable, iterator, body)                      \
    {                                                           \
        var iter = iterator;                                    \
        var variable##_anonym = iter.next();                    \
        while (variable##_anonym.is_some()) {                   \
            var& variable = variable##_anonym.unwrap().deref(); \
            { body; }                                           \
            variable##_anonym = iter.next();                    \
        }                                                       \
    }

// Dynamic list
template <class T, class Allocator = BasicAllocator>
struct List {
    List() {
        capacity = 1;
        length = 0;
        memory = allocator.allocate_array(1).unwrap("List: constructor");
    }

    List(usize size) {
        capacity = size;
        length = 0;
        memory = allocator.allocate_array(size).unwrap("List: constructor");
    }

    void destroy() {
        allocator.free_array(memory, capacity);
    }

    void destroy(std::function<void(const T&)> destroy_function) {
        for (usize i = 0; i < length; i++) {
            destroy_function(this->memory[i]);
        }

        allocator.free_array(memory, capacity);
    }

    void clear() {
        length = 0;
    }

    void add(T element) {
        if (length + 1 >= capacity) reserve(capacity * 2);

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

    T* ptr() {
        return this->memory;
    }

    usize size() const {
        return length;
    }

    bool empty() const {
        return length == 0;
    }

    void reserve(usize size) {
        if (size <= this->capacity) return;

        T* new_memory = allocator.allocate_array(size).unwrap("List reserve");

        for (usize i = 0; i < this->length; i++) {
            new_memory[i] = this->memory[i];
        }

        allocator.free_array(this->memory, this->capacity);

        this->memory = new_memory;
        this->capacity = size;
    }

    List<T, Allocator> clone() const {
        List<T, Allocator> list{capacity};
        list.length = length;

        for (usize i = 0; i < length; i++) {
            list.memory[i] = this->memory[i];
        }

        return list;
    }

    List<T, Allocator> clone(std::function<T(const T&)> clone_func) const {
        List<T, Allocator> list{capacity};
        list.length = length;

        for (usize i = 0; i < length; i++) {
            list.memory[i] = clone_func(this->memory[i]);
        }

        return list;
    }

    struct Iter {
        Iter(List<T, Allocator>& list) : list(list) {
        }

        Option<Ref<T>> next() {
            let el = list.at(current);
            current++;
            return el;
        }

       private:
        List<T, Allocator>& list;
        usize               current = 0;
    };

    Iter iter() {
        return Iter{*this};
    }

   private:
    T*                          memory;
    usize                       capacity;
    usize                       length;
    TypeAllocator<T, Allocator> allocator;
};

// TODO: Make it better
template <class Allocator = BasicAllocator>
struct DebugAllocator {
    Option<Raw> allocate(usize size) {
        let raw = allocator.allocate(size);
        on_some(raw) list.add(raw.unwrap());
        return raw;
    }

    bool own(const Raw& raw) const {
        return allocator.own(raw);
    }

    void free(const Raw& raw) {
        allocator.free(raw);

        for (usize i = 0; i < list.size(); i++) {
            let mem = list.at(i).unwrap().deref();
            if (mem.ptr == raw.ptr) list.remove(i);
        }
    }

    void free_all() {
        allocator.free_all();
        list.clear();
    }

    bool memory_leak() {
        return !list.empty();
    }

   private:
    Allocator allocator;
    List<Raw> list;
};

#include <type_traits>

/*
    Template meta programing c++ magic shit
    Do not question, just run ....
*/
template <typename T>
concept HasToString =
    requires(const T& t) { std::is_member_function_pointer_v<decltype(&T::to_string)>; };
template <HasToString T>
void print(const T& obj) {
    std::cout << obj.to_string();
}

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
        return *(format + current);
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

template <HasToString T>
void format_helper(Characters& characters, const T& obj, std::stringstream& ss) {
    while (true) {
        let ch = characters.next();
        if (ch == '%') {
            ss << obj.to_string();
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

template <class Allocator = BasicAllocator>
struct String {
    static String<Allocator> from(const char* cstr) {
        usize len = strlen(cstr);

        String<Allocator> string;
        string.char_block = string.allocator.allocate_array(len).unwrap("String from");

        memcpy(string.char_block, cstr, len);

        return string;
    }

    static String<Allocator> from(const std::string& stl_string) {
        return String<Allocator>::from(stl_string.c_str());
    }

    usize size() const {
        return len;
    }

    void destroy() const {
        allocator.free_array(char_block, len);
    }

    String<Allocator> clone() {
        return String<Allocator>::from(char_block);
    }

   private:
    char* char_block;
    usize len = 0;

    TypeAllocator<char, Allocator> allocator;

    template <class Allo>
    friend std::ostream& operator<<(std::ostream& os, const String<Allo>& string);
};

template <class Allocator>
std::ostream& operator<<(std::ostream& os, const String<Allocator>& string) {
    for (usize i = 0; i < string.size(); i++) {
        os << string.char_block[i];
    }
    return os;
}
