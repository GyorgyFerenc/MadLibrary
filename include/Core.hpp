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

// --- DEFER ---
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
// --- /DEFER ---

// --- PANIC ---
void panic(const char* text) {
    printf("%s\n",text);
    assert(false && "panic");
}

void panic() {
    assert(false && "panic");
}
// --- /PANIC ---

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
inline Option<T> some(T value) {
    return {true, value};
}

template <class T>
inline Option<T> none() {
    return {false};
}

template <class T>
inline T unwrap(Option<T> option) {
    if (option.error == Core_Error::Correct) return option.value;
    panic("Option_::unwrap()");
    UNREACHABLE;
}

template <class T>
inline T unwrap(Option<T> option, const char* msg) {
    if (option.error == Core_Error::Correct) return option.value;
    panic(msg);
    UNREACHABLE;
}

}  // namespace Option_

template<class T>
T* typed_memcpy(T* dest, const T* src, usize n){
    return (T*)memcpy(dest, src, sizeof(T) * n);
}

/*
 *   TODO(Ferenc): Add alignment to allocators
 *   TODO(Ferenc): Add nicer interface to allocator implementations
 */
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
Errorable<T*> allocate(Allocator allocator){
    let try_ptr = allocate_raw(allocator, sizeof(T));
    return {try_ptr.error, (T*)try_ptr.value};
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



#define For_Each(iter) for (let it = iter; it.next();) 

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

};


template <class T>
struct Dynamic_Array{
    T* ptr;
    usize size;
    usize capacity;
    Allocator allocator;
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
        let err = reserve(array, 2 * array->capacity);
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

/*
 * Strings are UTF-8 encoded
 * .size is size in bytes
 */
using String = Array<u8>;
// Rune is on 4 bytes representing a unicode code point
using Rune   = u32;

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
String clone_from(Allocator allocator, const char* c_str){
    let size = strlen(c_str);
    let str = String_::create(allocator, size);
    memcpy((void*)str.ptr, (void*)c_str, size);
    return str;
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
usize rune_length(String str){
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

usize encode_rune_to_utf8(Rune rune, u8* ptr){
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
Pair<Rune, usize> decode_rune_from_utf8(u8* encoded){
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


Array<Rune> to_rune_array(String str, Allocator allocator){
    let len = rune_length(str);
    let array = Array_::create<Rune>(allocator, len);
    usize i = 0;
    usize i_array = 0;

    while (i < str.size) {
        let [rune, size] = decode_rune_from_utf8(str.ptr + i);
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
    Rune  rune;

    inline
    bool next(){
        if (idx >= size) return false;
        let [rune, nr] = String_::decode_rune_from_utf8(ptr + idx);
        this->idx += nr;
        this->rune = rune;
        this->count++;
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
