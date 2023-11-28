#pragma once

#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <cstring>


#define KB 1024
#define MB 1024 * KB
#define GB 1024 * MB

using uint = unsigned int;
using i8 = int8_t;
using i16 = int16_t;
using i32 = int32_t;
using i64 = int64_t;
using u8 = uint8_t;
using u16 = uint16_t;
using u32 = uint32_t;
using u64 = uint64_t;
using usize = std::size_t;

#define UNREACHABLE assert(false && "uncreachable");

#define let auto
#define cast(type) (type)

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
    printf("%s\n",text);
    assert(false && "panic");
}

void panic() {
    assert(false && "panic");
}

// --- Error ---
using Error = u32;

inline Error declare_error() {
    static Error error = 0;
    return error++;
}

namespace Core_Error {
Error Correct = declare_error();
}  // namespace Core_Error

template <class T>
struct Errorable {
    Error error;
    T     value;
};

namespace Errorable_ {
template <class T>
inline T unwrap(Errorable<T> errorable) {
    if (errorable.error == Core_Error::Correct) return errorable.value;
    panic("Errorable_::unwrap()");
    UNREACHABLE;
}

template <class T>
inline T unwrap(Errorable<T> errorable, const char* msg) {
    if (errorable.error == Core_Error::Correct) return errorable.value;
    panic(msg);
    UNREACHABLE;
}
}  // namespace Errorable_

/*
 * Returns the error if it is an error
 */
#define return_error(errorable) \
    if (errorable.error != Core_Error::Correct) return {errorable.error};


// --- /Error ---

template <class T>
struct Option {
    bool some;
    T    value;
};

namespace Option_ {

template <class T>
inline 
Option<T> some(T value) {
    return {true, value};
}

template <class T>
inline 
Option<T> none() {
    return {false};
}

template <class T>
inline 
T unwrap(Option<T> option) {
    if (option.some) return option.value;
    panic("Option_::unwrap()");
    UNREACHABLE;
}

template <class T>
inline 
T unwrap(Option<T> option, const char* msg) {
    if (option.some) return option.value;
    panic(msg);
    UNREACHABLE;
}

}  // namespace Option_

template<class T>
T* typed_memcpy(T* dest, const T* src, usize n){
    return (T*)memcpy(dest, src, sizeof(T) * n);
}


template<class T>
void typed_memset(T* ptr, T data, usize n){
    for (usize i = 0; i < n; i++){
        ptr[i] = data;
    }
}

namespace Allocation_Error  {
    let Unsupported_Allocation_Mode = declare_error();
    let Out_Of_Memory = declare_error(); //e.g malloc ENOMEN
    let Invalid_Free = declare_error();  //e.g free
};

enum class Allocation_Mode{
    Allocate,
    Reallocate,
    Free,
    Free_All,
};

using Allocator_Function = Errorable<void*> (*)(void* obj, Allocation_Mode mode, usize size, void* old_ptr, usize old_size);

struct Allocator{
    void* obj;
    Allocator_Function function;
};

namespace Allocator_{

inline
Errorable<void*> allocate_raw(Allocator allocator, usize size){
    return allocator.function(allocator.obj, Allocation_Mode::Allocate, size, nullptr, 0);
}


inline
Error free_raw(Allocator allocator, void* old_ptr, usize old_size){
    return allocator.function(allocator.obj, Allocation_Mode::Free, 0, old_ptr, old_size).error;
}


inline
Errorable<void*> reallocate_raw(Allocator allocator, usize size, void* old_ptr, usize old_size){
    return allocator.function(allocator.obj, Allocation_Mode::Reallocate, size, old_ptr, old_size);
}

inline
Error free_all(Allocator allocator){
    return allocator.function(allocator.obj, Allocation_Mode::Free_All, 0, nullptr, 0).error;
}

template<class T>
inline
Errorable<T*> allocate(Allocator allocator, T value = {}){
    let try_ptr = allocate_raw(allocator, sizeof(T));
    return_error(try_ptr);
    let t_ptr = (T*)try_ptr.value;
    *t_ptr = value;
    return {try_ptr.error, t_ptr};
}

template<class T>
inline
T* allocate_unwrap(Allocator allocator, T value = {}){
    let try_ptr = allocate_raw(allocator, sizeof(T));
    let t_ptr = (T*)Errorable_::unwrap(try_ptr);
    *t_ptr = value;
    return t_ptr;
}

template<class T>
inline
Error free(Allocator allocator, T* ptr){
    return free_raw(allocator, (void*)ptr, sizeof(T));
}

template<class T>
inline
Errorable<T*> allocate_array(Allocator allocator, usize size){
    let try_ptr = allocate_raw(allocator, sizeof(T) * size);
    return {try_ptr.error, (T*)try_ptr.value};
}

template<class T>
inline
Error free_array(Allocator allocator, T* ptr, usize old_size){
    return free_raw(allocator, (void*)ptr, sizeof(T) * old_size);
}


template<class T>
inline
Errorable<T*> reallocate_array(Allocator allocator, usize size, T* ptr, usize old_size){
    let try_ptr = reallocate_raw(allocator, sizeof(T) * size, ptr, sizeof(T) * old_size);
    return {try_ptr.error, (T*)try_ptr.value};
}

};



namespace Malloc_Allocator_{

Errorable<void*> malloc_allocate(Allocation_Mode mode, usize size, void* old_ptr, usize old_size){
    switch (mode) {
        case Allocation_Mode::Allocate:{ 
            let ptr = malloc(size);
            if (ptr == NULL) return {Allocation_Error::Out_Of_Memory};
            return {Core_Error::Correct, ptr};
        } break;
        case Allocation_Mode::Reallocate:{ 
            let ptr = realloc(old_ptr, size);
            if (ptr == NULL) return {Allocation_Error::Out_Of_Memory};
            return {Core_Error::Correct, ptr};
        } break;
        case Allocation_Mode::Free:{ 
            free(old_ptr);
            return {Core_Error::Correct};
        } break;
        case Allocation_Mode::Free_All:{ 
            return {Allocation_Error::Unsupported_Allocation_Mode};
        } break;
    }
    return {Allocation_Error::Unsupported_Allocation_Mode};
}

Allocator to_interface(){
    return Allocator{
        .obj = nullptr,
        .function = [](void* obj, Allocation_Mode mode, usize size, void* old_ptr, usize old_size) -> Errorable<void*>{
            return Malloc_Allocator_::malloc_allocate(mode, size, old_ptr, old_size);
        },
    };
}

};

struct Arena_Allocator{
    u8* memory;
    usize size;
    usize position = 0;
};

namespace Arena_Allocator_ {

inline
Arena_Allocator create(u8* memory, usize size){
    return Arena_Allocator{
        .memory = memory,
        .size = size,
        .position = 0,
    };
}

Errorable<void*> allocate_impl(Arena_Allocator* alloc, Allocation_Mode mode, usize size, void* old_ptr, usize old_size){
    switch (mode) {
        case Allocation_Mode::Reallocate:{ 
            let old_position = alloc->position - old_size;
            let old_ptr2 = (void*)&alloc->memory[old_position];
            
            // Resize was called on the last allocated memory
            if (old_ptr == old_ptr2) {
                if (old_position + size > alloc->size) return {Allocation_Error::Out_Of_Memory};
                alloc->position = old_position + size;
                return {Core_Error::Correct, old_ptr}; 
            }
            // need to allocate new memory
            let try_new_memory = allocate_impl(alloc, Allocation_Mode::Allocate, size, old_ptr, old_size);
            return_error(try_new_memory);
            let new_memory = try_new_memory.value;
            memcpy(new_memory, old_ptr, old_size);
            return {Core_Error::Correct, new_memory};
        } break;
        case Allocation_Mode::Allocate:{ 
            if (alloc->position + size > alloc->size) return {Allocation_Error::Out_Of_Memory};
            let ptr = (void*)&alloc->memory[alloc->position];
            alloc->position += size;
            return {Core_Error::Correct, ptr};
        } break;
        case Allocation_Mode::Free:{ 
            let old_position = alloc->position - old_size;
            let old_ptr2 = (void*)&alloc->memory[old_position];
            
            // Free was called on the last allocated memory
            // so we can free it up otherwise no
            if (old_ptr == old_ptr2) {
                alloc->position = old_position;
            }
            return {Core_Error::Correct};
        } break;
        case Allocation_Mode::Free_All:{ 
            alloc->position = 0;
            return {Core_Error::Correct};
        } break;
    }
    return {Allocation_Error::Unsupported_Allocation_Mode};
}


Allocator to_interface(Arena_Allocator* alloc){
    return Allocator{
        .obj = alloc,
        .function = [](void* obj, Allocation_Mode mode, usize size, void* old_ptr, usize old_size) -> Errorable<void*>{
            return allocate_impl((Arena_Allocator*)obj, mode, size, old_ptr, old_size);
        },
    };
}

bool has_space(Arena_Allocator alloc, usize size){
    return alloc.position + size <= alloc.size;
}


inline
Errorable<void*> allocate_raw(Arena_Allocator* allocator, usize size){
    return allocate_impl(allocator, Allocation_Mode::Allocate, size, nullptr, 0);
}


inline
Error free_raw(Arena_Allocator* allocator, void* old_ptr, usize old_size){
    return allocate_impl(allocator, Allocation_Mode::Free, 0, old_ptr, old_size).error;
}


inline
Errorable<void*> reallocate_raw(Arena_Allocator* allocator, usize size, void* old_ptr, usize old_size){
    return allocate_impl(allocator, Allocation_Mode::Reallocate, size, old_ptr, old_size);
}

inline
Error free_all(Arena_Allocator* allocator){
    return allocate_impl(allocator, Allocation_Mode::Free_All, 0, nullptr, 0).error;
}

template<class T>
inline
Errorable<T*> allocate(Arena_Allocator* allocator){
    let try_ptr = allocate_raw(allocator, sizeof(T));
    return {try_ptr.error, (T*)try_ptr.value};
}

template<class T>
inline
Error free(Arena_Allocator* allocator, T* ptr){
    return free_raw(allocator, (void*)ptr, sizeof(T));
}

template<class T>
inline
Errorable<T*> allocate_array(Arena_Allocator* allocator, usize size){
    let try_ptr = allocate_raw(allocator, sizeof(T) * size);
    return {try_ptr.error, (T*)try_ptr.value};
}

template<class T>
inline
Error free_array(Arena_Allocator* allocator, T* ptr, usize old_size){
    return free_raw(allocator, (void*)ptr, sizeof(T) * old_size);
}


template<class T>
inline
Errorable<T*> reallocate_array(Arena_Allocator* allocator, usize size, T* ptr, usize old_size){
    let try_ptr = reallocate_raw(allocator, sizeof(T) * size, ptr, sizeof(T) * old_size);
    return {try_ptr.error, (T*)try_ptr.value};
}

};


struct Page_Allocator{
    Allocator alloc;
    Arena_Allocator arena;


    u8* page;
    usize page_size;

    struct Page_Header{
        u8* next;
        usize size;
    };
};

namespace Page_Allocator_{


inline
Page_Allocator create(Allocator allocator, usize page_size){
    return Page_Allocator{
        .alloc = allocator,
        .page = nullptr,
        .page_size = page_size,
    };
}


Errorable<void*> allocate(Page_Allocator* alloc, Allocation_Mode mode, usize size, void* old_ptr, usize old_size){

    switch (mode) {
        case Allocation_Mode::Reallocate:{
            let try_mem = Arena_Allocator_::allocate_impl(&alloc->arena, mode, size, old_ptr, old_size);
            if (try_mem.error == Core_Error::Correct) return try_mem;
        }   // fallthrough to allocate the new page if needed
            // Notice that it will call Areana_Allocator_::allocate with
            // the mode that is passed in, which means that it will resize
            // thus the copying is handled by the Arena_Allocator
        case Allocation_Mode::Allocate: {
            if (alloc->page == nullptr || !Arena_Allocator_::has_space(alloc->arena, size)){
                let needed_size = alloc->page_size > size ? alloc->page_size : size;

                let try_mem = Allocator_::allocate_raw(alloc->alloc, sizeof(Page_Allocator::Page_Header) + needed_size);
                return_error(try_mem);
                let mem_block = (u8*)try_mem.value;

                let page_header = (Page_Allocator::Page_Header*)mem_block;
                page_header->next = alloc->page;
                page_header->size = needed_size;

                alloc->arena = Arena_Allocator_::create(mem_block + sizeof(Page_Allocator::Page_Header), needed_size);
                alloc->page = mem_block;
            }
            return Arena_Allocator_::allocate_impl(&alloc->arena, mode, size, old_ptr, old_size);
        }break;
        case Allocation_Mode::Free: return {Allocation_Error::Unsupported_Allocation_Mode};
        case Allocation_Mode::Free_All:{
            let page = alloc->page;
            while (page != nullptr) {
                let page_header = (Page_Allocator::Page_Header*)page;
                let next = page_header->next;
                Allocator_::free_raw(alloc->alloc, page, page_header->size);
                page = next;
            }
            return {Core_Error::Correct};
        }break;
    
    }
    return {Allocation_Error::Unsupported_Allocation_Mode};
}


Allocator to_interface(Page_Allocator* alloc){
    return Allocator{
        .obj = alloc,
        .function = [](void* obj, Allocation_Mode mode, usize size, void* old_ptr, usize old_size) -> Errorable<void*>{
            return allocate((Page_Allocator*)obj, mode, size, old_ptr, old_size);
        },
    };
}

};

struct Pool_Allocator{
    u8* memory;   
    usize memory_size;
    usize chunk_size;
    
    struct Free_Node{
        Free_Node* next = nullptr;
    };

    Free_Node* head;
};

namespace Pool_Allocator_ {
Errorable<void*> allocate(Pool_Allocator* alloc, Allocation_Mode mode, usize size, void* old_ptr, usize old_size);

inline
Pool_Allocator create(u8* memory, usize memory_size, usize chunk_size){
    let alloc = Pool_Allocator{
        .memory = memory,
        .memory_size = memory_size,
        .chunk_size = chunk_size,
        .head = nullptr,
    };
    allocate(&alloc, Allocation_Mode::Free_All, 0, nullptr, 0);
    return alloc;
}

Errorable<void*> allocate(Pool_Allocator* alloc, Allocation_Mode mode, usize size, void* old_ptr, usize old_size){
    switch (mode) {
        case Allocation_Mode::Reallocate:{
            return {Allocation_Error::Unsupported_Allocation_Mode};
        }break;
        case Allocation_Mode::Allocate:{
            if (alloc->head == nullptr) return {Allocation_Error::Out_Of_Memory};
            let mem = alloc->head;
            alloc->head = alloc->head->next;
            return {Core_Error::Correct, mem};
        }break;
        case Allocation_Mode::Free:{
            let node = (Pool_Allocator::Free_Node*)old_ptr;
            node->next = alloc->head;
            alloc->head = node;
            return {Core_Error::Correct};
        }break;
        case Allocation_Mode::Free_All:{
            alloc->head = nullptr;
            let nr = alloc->memory_size / alloc->chunk_size;

            for (usize i = 0; i < nr; i++){
                let ptr = &alloc->memory[i*alloc->chunk_size];
                let node = (Pool_Allocator::Free_Node*)ptr;
                node->next = alloc->head;
                alloc->head= node;
            }

            return {Core_Error::Correct};
        }break;
    
    }
    return {Allocation_Error::Unsupported_Allocation_Mode};
}

};

template<class T1, class T2>
struct Pair{
    T1 first;
    T2 second;
};

namespace Pair_{

template<class T1, class T2>
inline
Pair<T1, T2> create(T1 first, T2 second){
    return Pair<T1, T2>{
        .first = first,
        .second = second,
    };
}

};



#define For_Each(iter)     for (let  it = iter; it.next();) 
#define For_Each_Ref(iter) for (let& it = iter; it.next();) 

struct Range{
    int closed_low;
    int open_high;
    int value;

    inline
    bool next(){
        this->value = closed_low;
        let a  = closed_low < open_high;
        closed_low++;
        return a;
    }
};

namespace Range_{

Range create(int closed_low, int open_high){
    return Range{closed_low, open_high};
}

};

template <class T>
struct Array{
    T* ptr;
    usize size;
    Allocator allocator;

    T& operator[](usize idx){
        return ptr[idx];
    }
};

template<class T>
struct Array_Iter{
    T* ptr;
    usize size;

    T value;
    T* value_ptr;
    usize count = 0;
    usize idx; 


    inline
    bool next(){
        if (count >= size) return false;
        idx        = count++;
        value      = ptr[idx];
        value_ptr  = &ptr[idx];
        return true;
    }
};


namespace Array_{


template <class T>
inline
Array<T> create(Allocator allocator, usize size){
    let try_ptr = Allocator_::allocate_array<T>(allocator, size);
    return Array<T>{
        .ptr = Errorable_::unwrap(try_ptr, "Array_::create"),
        .size = size,
        .allocator = allocator,
    };
}

template <class T>
void destroy(Array<T>* array){
    Allocator_::free_array(array->allocator, array->ptr, array->size);
}


template<class T>
Array_Iter<T> iter(Array<T> array){
    return Array_Iter<T>{
        .ptr = array.ptr,
        .size = array.size,
        .count = 0,
    };
}

/*
 * This used so any data can be easily 
 * aliasad as an array of bytes
 * e. g.
 * ```cpp
 *  u32 a = 12;
 *  let bytes = Array_::alias_bytes_of(&a); // it is the bytes of a;
 * ```
 */
template<class T>
Array<u8> alias_bytes_of(T* data){
    return {
        .ptr  = (u8*) data,
        .size = sizeof(T),
    };
}

};


template <class T>
struct Dynamic_Array{
    T* ptr;
    usize size;
    usize capacity;
    Allocator allocator;

    T& operator[](usize idx){
        return ptr[idx];
    }
};

namespace Dynamic_Array_{
template <class T>
Dynamic_Array<T> create(Allocator allocator, usize capacity = 1, usize size = 0){
    assert(capacity > size);
    let try_ptr = Allocator_::allocate_array<T>(allocator, capacity);
    return Dynamic_Array<T>{
        .ptr = Errorable_::unwrap(try_ptr, "Dynamic_Array_::create"),
        .size = size,
        .capacity = capacity,
        .allocator = allocator,
    };
}

template <class T>
void destroy(Dynamic_Array<T>* array){
    Allocator_::free_array(array->allocator, array->ptr, array->capacity);
}

template <class T>
Error reserve(Dynamic_Array<T>* array, usize new_capacity){
    if (array->capacity >= new_capacity) return Core_Error::Correct;

    let try_ptr = Allocator_::reallocate_array(array->allocator, new_capacity, array->ptr, array->capacity);
    return_error(try_ptr);
    array->ptr = try_ptr.value;
    array->capacity = new_capacity;
    return Core_Error::Correct;
}

template <class T>
usize append(Dynamic_Array<T>* array, T value){
    if (array->size + 1 > array->capacity) {
        reserve(array, 2 * array->capacity);
    }

    usize pos = array->size;
    array->size++;
    array->ptr[pos] = value;
    return pos;
}

template <class T>
void clear(Dynamic_Array<T>* array){
    array->size = 0;
}


template<class T>
Array_Iter<T> iter(Dynamic_Array<T> array){
    return Array_Iter<T>{
        .ptr = array.ptr,
        .size = array.size,
        .count = 0,
    };
}

};

// Rune is on 4 bytes representing a unicode code point
using Rune   = u32;

namespace Rune_{

usize encode_to_utf8(Rune rune, u8* ptr){
    u8 rune_least1_byte = 0;
    u8 rune_least2_byte = 0;
    u8 rune_least3_byte = 0;

    if (rune <= 0x7f) {
        ptr[0] = 0;
        ptr[0] |= (u8)rune;
        return 1;
    }

#if BYTE_ORDER == LITTLE_ENDIAN
    rune_least1_byte = ((u8*)&rune)[0];
    rune_least2_byte = ((u8*)&rune)[1];
#endif
#if BYTE_ORDER == BIG_ENDIAN
    rune_least1_byte = ((u8*)&rune)[3];
    rune_least2_byte = ((u8*)&rune)[2];
#endif

    if (rune <= 0x07FF) {
        let bit_6 =  rune_least1_byte & 0b111111;
        let bit_2 = (rune_least1_byte >> 6) & 0b11;
        let bit_3 =  rune_least2_byte & 0b111;
        ptr[0]    = (0b110 << 5) | (bit_3 << 2) | bit_2;
        ptr[1]    = (0b10  << 6) |  bit_6;

        return 2;
    }

    if (rune <= 0xFFFF){
        let bit_6_from_least1       =  rune_least1_byte & 0b111111;
        let bit_2_from_least1       = (rune_least1_byte >> 6) & 0b11;
        let bit_least_4_from_least2 =  rune_least2_byte & 0b1111;
        let bit_4_from_least2       = (rune_least2_byte >> 4) & 0b1111;
        ptr[0] = (0b1110 << 4) |  bit_4_from_least2;
        ptr[1] = (0b10   << 6) | (bit_least_4_from_least2 << 2) | bit_2_from_least1;
        ptr[2] = (0b10   << 6) |  bit_6_from_least1;
        return 3;
    }

#if BYTE_ORDER == LITTLE_ENDIAN
    rune_least3_byte = ((u8*)&rune)[2];
#endif
#if BYTE_ORDER == BIG_ENDIAN
    rune_least3_byte = ((u8*)&rune)[1];
#endif
    
    let bit_6_from_least1       =  rune_least1_byte       & 0b111111;
    let bit_2_from_least1       = (rune_least1_byte >> 6) & 0b11;
    let bit_least_4_from_least2 =  rune_least2_byte       & 0b1111;
    let bit_4_from_least2       = (rune_least2_byte >> 4) & 0b1111;
    let bit_2_from_least3       =  rune_least3_byte       & 0b11;
    let bit_3_from_least3       = (rune_least3_byte >> 2) & 0b111;
    ptr[0] = (0b11110 << 3) |  bit_3_from_least3;
    ptr[1] = (0b10    << 6) | (bit_2_from_least3 << 4)       | bit_4_from_least2;
    ptr[2] = (0b10    << 6) | (bit_least_4_from_least2 << 2) | bit_2_from_least1;
    ptr[3] = (0b10    << 6) | bit_6_from_least1;
    return 4;
}

inline
Pair<Rune, usize> decode_from_utf8(u8* encoded){
    Rune rune = {0};
    u32 b = encoded[0];
    usize size = 0;

    if      ((b >> 7) == 0)       {
        rune = rune | b;
        size = 1;
    }
    else if ((b >> 5) == 0b110)   {
        let bit_6 =  encoded[1]       & 0b111111;
        let bit_5 =  encoded[0]       & 0b11111;
        rune = (bit_5 << 6) | bit_6;
        size = 2;
    }
    else if ((b >> 4) == 0b1110)  {
        let bit_6_2 =  encoded[2]       & 0b111111;
        let bit_6_1 =  encoded[1]       & 0b111111;
        let bit_4   =  encoded[0]       & 0b1111;
        rune = (bit_4 << 12) | (bit_6_1 << 6) | bit_6_2;
        size = 3;
    }
    else if ((b >> 3) == 0b11110) {
        let bit_6_3 =  encoded[3]       & 0b111111;
        let bit_6_2 =  encoded[2]       & 0b111111;
        let bit_6_1 =  encoded[1]       & 0b111111;
        let bit_3   =  encoded[0]       & 0b1111;
        rune = (bit_3 << 18) | (bit_6_1 << 12) | (bit_6_2 << 6) | bit_6_3;
        size = 4;
    }

    return {rune, size};
}

/*
 * It reads the first rune from the c_str
 */
Rune from_c_str(const char* c_str){
    return decode_from_utf8((u8*)c_str).first;
}


Rune from_char(char chr){
    return decode_from_utf8((u8*)&chr).first;
}

inline
bool whitespace(Rune rune){
    switch (rune) {
        case 0x0A:   return true;
        case 0x20:   return true;
        case 0xA0:   return true;
        case 0x1680: return true;
        case 0x202F: return true;
        case 0x205F: return true;
        case 0x3000: return true;
        case 0x2028: return true;
        case 0x2029: return true;
    }
    if (0x2000 <= rune && rune <= 0x200A) return true;
    return false;
}

inline
bool digit(Rune rune){
    return '0' <= rune && rune <= '9';
}

};


/*
 * Strings are UTF-8 encoded
 * .size is size in bytes
 */
using String = Array<u8>;

namespace String_{


inline 
String create(Allocator allocator, usize size){
    return Array_::create<u8>(allocator, size);
}

inline
void destroy(String* str){
    Array_::destroy(str);
}

inline
String alias(const char* c_str){
    return String{
        .ptr = (u8*)c_str,
        .size = strlen(c_str),
    };
}

inline
String clone_from_cstr(const char* c_str, Allocator allocator){
    let size = strlen(c_str);
    let str = String_::create(allocator, size);
    memcpy((void*)str.ptr, (void*)c_str, size);
    return str;
}

inline
Option<String> substr_alias(String str, usize begin, usize len){
    if (str.size < begin + len) return {false};
    return {true, 
            String{
                .ptr = str.ptr + begin,
                .size = len,
                .allocator = str.allocator,
            }
    };
}

inline
String substr_alias_unsafe(String str, usize begin, usize len){
    return String{
        .ptr = str.ptr + begin,
        .size = len,
        .allocator = str.allocator,
    };
}

inline
char* c_str_unsafe(String str, Allocator allocator){
    let ptr_try = Allocator_::allocate_array<char>(allocator, str.size + 1);
    let ptr     = Errorable_::unwrap(ptr_try);
    ptr[str.size] = '\0';
    memcpy(ptr, str.ptr, str.size);
    return ptr;
}

/*
 * The length of the string
 * mesaured by the nr of runes in it
 *
 * O(str.size)
 *
 * https://www.rfc-editor.org/rfc/rfc3629
 */
inline
usize length_by_rune(String str){
    if (str.size == 0) return 0;
    usize len = 0;   
    usize i = 0;

    while (i < str.size) {
        let b = str.ptr[i];
        if      ((b >> 7) == 0)       i++;
        else if ((b >> 5) == 0b110)   i += 2;
        else if ((b >> 4) == 0b1110)  i += 3;
        else if ((b >> 3) == 0b11110) i += 4;
        len++;
    }
    return len;
}

/*
 *  O(1)
 */
Rune rune_at_unsafe(String str, usize byte_pos){
     let [rune, _] = Rune_::decode_from_utf8(str.ptr + byte_pos);
     return rune;
}

bool equal(String lhs, String rhs){
    if (lhs.size != rhs.size) return false;

    For_Each(Array_::iter(lhs)){
        if (it.value != rhs[it.idx]) return false;
    }

    return true;
}


bool equal_c_str(String lhs, const char* rhs){
    
    for (usize i = 0; i < lhs.size; i++){
        if (rhs[0] == '\0') return false;
        if (lhs[i] != (u8)rhs[i]) return false; // (u8) because char is fucking signed
    }

    if (rhs[lhs.size] != '\0') return false;

    return true;
}


bool starts_with_c_str(String str, const char* c_str){
    for (usize i = 0; *c_str != '\0'; i++) {
        if (str.size <= i || str.ptr[i] != *c_str) return false;
        c_str++;
    }
    return true;
};


bool starts_with(String str1, String str2){
    if (str1.size < str2.size) return false;
    for (usize i = 0; i< str2.size; i++){
        if (str1.ptr[i] != str2.ptr[i]) return false;
    }
    return true;
};

/*
 * Doesn't free the removed parts
 */
String remove_prefix(String str, usize len){
    if (str.size <= len) return String{
        .size = 0,
    };
    return String{
        .ptr = str.ptr + len,
        .size = str.size - len,
        .allocator = str.allocator,
    };
}



Array<Rune> to_rune_array(String str, Allocator allocator){
    let len = length_by_rune(str);
    let array = Array_::create<Rune>(allocator, len);
    usize i = 0;
    usize i_array = 0;

    while (i < str.size) {
        let [rune, size] = Rune_::decode_from_utf8(str.ptr + i);
        array.ptr[i_array] = rune;

        i += size;
        i_array++;
    }

    return array;
}

inline
Array_Iter<u8> iter(String str){
    return Array_::iter(str);
}

};

struct String_Rune_Iter{
    u8* ptr;
    usize size;

    usize count = 0;
    usize idx   = 0; 
    usize last_idx = 0;
    Rune  rune = 0;
    usize rune_byte_len = 0; // Last decoded runes len

    inline
    bool next(){
        if (this->last_idx >= size) return false;
        this->idx = this->last_idx;
        let [rune, rune_byte_len] = Rune_::decode_from_utf8(ptr + idx);
        this->last_idx += rune_byte_len;
        this->rune = rune;
        this->count++;
        this->rune_byte_len = rune_byte_len;
        return true;
    }
};

namespace String_{

inline
String_Rune_Iter iter_rune(String str){
    return {
        .ptr = str.ptr,
        .size = str.size,
        .count = 0,
    };
}
}


struct String_Builder{
    Dynamic_Array<u8> bytes;
};

namespace String_Builder_ {

String_Builder create(Allocator allocator){
    return {
        .bytes = Dynamic_Array_::create<u8>(allocator),
    };
}

void destroy(String_Builder* builder){
    Dynamic_Array_::destroy(&builder->bytes);
}


String build(String_Builder builder, Allocator allocator){
    let array = Array_::create<u8>(allocator, builder.bytes.size);
    typed_memcpy(array.ptr, builder.bytes.ptr, array.size);
    return array;
}

void add_c_str(String_Builder* builder, const char* c_str){
    while (*c_str != '\0') {
        Dynamic_Array_::append(&builder->bytes, (u8)*c_str);
        c_str++;
    }
}


void add_char(String_Builder* builder, char chr){
    Dynamic_Array_::append(&builder->bytes, (u8)chr);
}


void add_rune(String_Builder* builder, Rune rune){
    u8 bytes[4] = {0, 0, 0, 0};;
    let len = Rune_::encode_to_utf8(rune, bytes);
    for (usize i = 0; i < len; i++){
        Dynamic_Array_::append(&builder->bytes, bytes[i]);
    }
}

void clear(String_Builder* builder){
    Dynamic_Array_::clear(&builder->bytes);
}


}



/*
 *  This uses a linked list
 */
template<class T>
struct Queue{
    struct Node{
        Node* next = nullptr;
        T     data;
    };

    Allocator allocator;
    Node*     head = nullptr;
    Node*     tail = nullptr;
    usize     size = 0;
};

namespace Queue_{


template<class T>
inline 
Queue<T> create(Allocator allocator){
    return {
        .allocator = allocator,
        .head      = nullptr,
        .tail      = nullptr,
        .size      = 0,
    };
};

template<class T>
void destroy(Queue<T>* q){
    let node = q->head;
    while (node != nullptr) {
        let temp = node->next;
        Allocator_::free(q->allocator, node);
        node = temp;
    }
    q->head = nullptr;
    q->tail = nullptr;
    q->size = 0;
}

template<class T>
void enque(Queue<T>* q, T elem){
    q->size++;
    let try_node = Allocator_::allocate<typename Queue<T>::Node>(q->allocator);
    let node     = Errorable_::unwrap(try_node);
    node->data   = elem;
    node->next   = nullptr;

    if (q->tail == nullptr){
        q->tail = node;
        q->head = node;
    }else{
        q->tail->next = node;
        q->tail = node;
    }
}


template<class T>
Option<T> deque(Queue<T>* q){
    if (q->head == nullptr) return {false};
    
    let data = q->head->data;
    let node = q->head;
    q->head  = node->next;
    Allocator_::free(q->allocator, node);
    if (q->head == nullptr) q->tail = nullptr;
    q->size--;

    return {true, data};
}


template<class T>
T deque_unsafe(Queue<T>* q){
    let data = q->head->data;
    let node = q->head;
    q->head  = node->next;
    Allocator_::free(q->allocator, node);
    if (q->head == nullptr) q->tail = nullptr;
    q->size--;

    return data;
}

template<class T>
bool empty(Queue<T> q){
    return q.size == 0;
}

};



template <class T> using Hash_Proc  = usize (*)(T lhs);
template <class T> using Probe_Proc = usize (*)(T lhs, usize i); 
template <class T> using Equal_Proc =  bool (*)(T lhs, T rhs);

template <class T>
struct Hash_Set{
    struct Elem{
        enum struct Kind: u8{
            Free,
            Used,
            Deleted,
        } kind = Kind::Free;
        T data;
    };

    Allocator allocator;
    Elem*     data = nullptr;
    usize     size = 0;
    usize     capacity = 0;

    Hash_Proc<T>  hash;
    Probe_Proc<T> probe;
    Equal_Proc<T> equal;
};

namespace Hash_Set_{

template <class T>
Hash_Set<T> create(Allocator     allocator, 
                   usize         capacity   = 1,
                   Hash_Proc<T>  hash_proc  = [](T lhs) -> usize { return (usize) lhs;}, 
                   Equal_Proc<T> equal_proc = [](T lhs, T rhs) -> bool { return lhs == rhs;},
                   Probe_Proc<T> probe      = [](T lhs, usize i) -> usize { return i * i; }
){
    let try_data = Allocator_::allocate_array<typename Hash_Set<T>::Elem>(allocator, capacity);
    let data     = Errorable_::unwrap(try_data);
    typed_memset(data, typename Hash_Set<T>::Elem{ .kind = Hash_Set<T>::Elem::Kind::Free, }, capacity);

    return Hash_Set<T>{
        .allocator = allocator,
        .data      = data,
        .size      = 0, 
        .capacity  = capacity,
        .hash      = hash_proc,
        .probe     = probe,
        .equal     = equal_proc,
    };
}

template <class T>
void destroy(Hash_Set<T>* set){
    Allocator_::free_array(set->allocator, set->data, set->capacity);
}

template <class T>
void reheash(Hash_Set<T>* set, usize capacity){
    let try_data = Allocator_::allocate_array<typename Hash_Set<T>::Elem>(set->allocator, capacity);
    let data     = Errorable_::unwrap(try_data);
    typed_memset(data, typename Hash_Set<T>::Elem{ .kind = Hash_Set<T>::Elem::Kind::Free, }, capacity);

    for (usize i = 0; i < set->capacity; i++){
        let elem = set->data[i];
        if (elem.kind != Hash_Set<T>::Elem::Kind::Used) continue;

        let hashed = set->hash(elem.data);
        for (usize i = 0; ; i++) {
            let pos  = (hashed + set->probe(elem.data, i)) % set->capacity;
            let elem_data = &data[pos];
            if (elem_data->kind != Hash_Set<T>::Elem::Kind::Used){
                elem_data->kind = Hash_Set<T>::Elem::Kind::Used;
                elem_data->data = elem.data;
                break;
            }
        }
    }

    Allocator_::free_array(set->allocator, set->data, set->capacity);
    set->data = data;
    set->capacity = capacity;
}

template <class T>
void add(Hash_Set<T>* set, T elem){
    if (set->size == set->capacity - 1 ||
        ((double)set->size / (double)set->capacity) >= 0.7){
        reheash(set, set->capacity * 2);
    }

    let hashed =  set->hash(elem);
    for (usize i = 0; ; i++) {
        let  pos  = (hashed + set->probe(elem, i)) % set->capacity;
        let elem_data = &set->data[pos];
        if (elem_data->kind == Hash_Set<T>::Elem::Kind::Used){
            if (set->equal(elem, elem_data->data)) break;
        } else {
            elem_data->kind = Hash_Set<T>::Elem::Kind::Used;
            elem_data->data = elem;
            set->size++;
            break;
        }
    }
}

template<class T>
bool remove(Hash_Set<T>* set, T elem){
    let hashed =  set->hash(elem);
    for (usize i = 0; i < set->size; i++) {
        let  pos  = (hashed + set->probe(elem, i)) % set->capacity;
        let elem_data = &set->data[pos];
        if (elem_data->kind == Hash_Set<T>::Elem::Kind::Free) return false;
        if (elem_data->kind == Hash_Set<T>::Elem::Kind::Used){
            if (set->equal(elem, elem_data->data)){
                elem_data->kind = Hash_Set<T>::Elem::Kind::Deleted;
                set->size--;
                return true;
            }
        } 
    }

    return false;
}

template <class T>
bool contains(Hash_Set<T> set, T elem){
    let hashed =  set.hash(elem);
    for (usize i = 0; i < set.size ; i++) {
        let  pos  = (hashed + set.probe(elem, i)) % set.capacity;
        let elem_data = set.data[pos];
        if (elem_data.kind == Hash_Set<T>::Elem::Kind::Free) return false;
        if (elem_data.kind == Hash_Set<T>::Elem::Kind::Used){
            if (set.equal(elem, elem_data.data)){
                return true;
            }
        } 
    }

    return false;
}

template<class T>
bool empty(Hash_Set<T> set){
    return set.size == 0;
}

};




template <class K, class V>
struct Hash_Map{
    struct Key_Elem{
        enum struct Kind: u8{
            Free,
            Used,
            Deleted,
        } kind = Kind::Free;
        K data;
    };

    Allocator allocator;
    Key_Elem* keys   = nullptr;
    V*        values = nullptr;
    usize capacity = 1;
    usize size = 0;

    Hash_Proc<K>  hash;
    Equal_Proc<K> equal;
    Probe_Proc<K> probe;
};

namespace Hash_Map_{

template <class K, class V>
Hash_Map<K, V> create(Allocator     allocator, 
                      usize         capacity   = 1,
                      Hash_Proc<K>  hash_proc  = [](K lhs) -> usize { return (usize) lhs;}, 
                      Equal_Proc<K> equal_proc = [](K lhs, K rhs) -> bool { return lhs == rhs;},
                      Probe_Proc<K> probe_proc = [](K lhs, usize i) -> usize { return i * i; }
){


    let try_keys = Allocator_::allocate_array<typename Hash_Map<K, V>::Key_Elem>(allocator, capacity);
    let keys     = Errorable_::unwrap(try_keys);
    typed_memset(keys, { .kind = Hash_Map<K, V>::Key_Elem::Kind::Free, }, capacity);

    let try_values = Allocator_::allocate_array<V>(allocator, capacity);
    let values     = Errorable_::unwrap(try_values);

    return Hash_Map<K, V>{
        .allocator = allocator,
        .keys      = keys,
        .values    = values,
        .capacity  = capacity,
        .size      = 0, 
        .hash      = hash_proc,
        .equal     = equal_proc,
        .probe     = probe_proc,
    };
}

template <class K, class V>
void destroy(Hash_Map<K, V>* map){
    Allocator_::free_array(map->allocator, map->keys,   map->capacity);
    Allocator_::free_array(map->allocator, map->values, map->capacity);
}

template <class K, class V>
void reheash(Hash_Map<K, V>* map, usize capacity){
    let try_keys = Allocator_::allocate_array<typename Hash_Map<K, V>::Key_Elem>(map->allocator, capacity);
    let keys     = Errorable_::unwrap(try_keys);
    typed_memset(keys, { .kind = Hash_Map<K, V>::Key_Elem::Kind::Free, }, capacity);

    let try_values = Allocator_::allocate_array<V>(map->allocator, capacity);
    let values     = Errorable_::unwrap(try_values);

    for (usize i = 0; i < map->capacity; i++){
        let key = map->keys[i];
        if (key.kind != Hash_Map<K, V>::Key_Elem::Kind::Used) continue;

        let hashed = map->hash(key.data);
        for (usize i = 0; ; i++) {
            let pos  = (hashed + map->probe(key.data, i)) % map->capacity;
            let new_key = &keys[pos];
            if (new_key->kind != Hash_Map<K, V>::Key_Elem::Kind::Used){
                new_key->kind = Hash_Map<K, V>::Key_Elem::Kind::Used;
                new_key->data = key.data;
                values[pos] = map->values[pos];
                break;
            }
        }
    }

    Allocator_::free_array(map->allocator, map->keys, map->capacity);
    Allocator_::free_array(map->allocator, map->values, map->capacity);
    map->keys = keys;
    map->values = values;
    map->capacity = capacity;
}

template <class K, class V>
void set(Hash_Map<K, V>* map, K key, V value){
    if (map->size == map->capacity - 1 ||
        ((double)map->size / (double)map->capacity) >= 0.7){
        reheash(map, map->capacity * 2);
    }

    let hashed =  map->hash(key);
    for (usize i = 0; ; i++) {
        let  pos  = (hashed + map->probe(key, i)) % map->capacity;
        let key_data = &map->keys[pos];
        if (key_data->kind == Hash_Map<K, V>::Key_Elem::Kind::Used){
            if (map->equal(key, key_data->data)) {
                map->values[pos] = value;
                break;
            }
        } else {
            key_data->kind = Hash_Map<K, V>::Key_Elem::Kind::Used;
            key_data->data = key;
            map->values[pos] = value;
            map->size++;
            break;
        }
    }
}

template <class K, class V>
bool remove(Hash_Map<K, V>* map, K key){
    let hashed =  map->hash(key);
    for (usize i = 0; i < map->size; i++) {
        let  pos  = (hashed + map->probe(key, i)) % map->capacity;
        let key_data = &map->keys[pos];
        if (key_data->kind == Hash_Map<K, V>::Key_Elem::Kind::Free) return false;
        if (key_data->kind == Hash_Map<K, V>::Key_Elem::Kind::Used){
            if (map->equal(key, key_data->data)){
                key_data->kind = Hash_Map<K, V>::Key_Elem::Kind::Deleted;
                map->size--;
                return true;
            }
        } 
    }

    return false;
}

template <class K, class V>
Option<V> get(Hash_Map<K, V> map, K key){
    let hashed =  map.hash(key);
    for (usize i = 0; i < map.size; i++) {
        let  pos  = (hashed + map.probe(key, i)) % map.capacity;
        let key_data = &map.keys[pos];
        if (key_data->kind == Hash_Map<K, V>::Key_Elem::Kind::Free) return {false};
        if (key_data->kind == Hash_Map<K, V>::Key_Elem::Kind::Used){
            if (map.equal(key, key_data->data)){
                return {true, map.values[pos]};
            }
        } 
    }

    return {false};
}


template <class K, class V>
V get_unwrap(Hash_Map<K, V> map, K key){
    return Option_::unwrap(get(map, key));
}


template <class K, class V>
V contains(Hash_Map<K, V> map, K key){
    return get(map, key).some;
}

template <class K, class V>
bool empty(Hash_Map<K, V> map){
    return map.size == 0;
}





};
