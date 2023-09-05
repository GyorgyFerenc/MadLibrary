#pragma once
#include <alloca.h>
#include <bits/fs_fwd.h>
#include <unistd.h>

#include <cstddef>
#include <cstring>

#include "Context.hpp"
#include "Intrinsics.hpp"

#define KiB 1024
#define MiB 1024 * KiB
#define GiB 1024 * MiB
#define TiB 1024 * GiB

#define KB 1000
#define MB 1000 * KB
#define GB 1000 * MB
#define TB 1000 * GB

namespace MemoryError {
Error NotEnoughSpace = declare_error();
Error Allocation = declare_error();
}  // namespace MemoryError

struct Raw {
    void* ptr;
    usize len;
};

namespace Raw_ {
Errorable<Raw> chunck(Raw* raw, usize new_len, usize offset = -1) {
    if (new_len + offset > raw->len) {
        return {MemoryError::NotEnoughSpace};
    }
    return {CoreError::Correct,
            Raw{
                .ptr = (byte*)raw->ptr + offset,
                .len = new_len,
            }};
}

template <class T>
Errorable<T*> cast(Raw raw) {
    if (raw.len < sizeof(T)) return {MemoryError::NotEnoughSpace};
    return {CoreError::Correct, (T*)raw.ptr};
}

template <class T>
Errorable<T*> cast_array(Raw raw, usize size) {
    if (raw.len < sizeof(T) * size) return {MemoryError::NotEnoughSpace};
    return {CoreError::Correct, (T*)raw.ptr};
}

Errorable<Raw> allocate(usize size) {
    let* ptr = malloc(size);
    if (ptr == NULL) return {MemoryError::Allocation};

    return {CoreError::Correct,
            Raw{
                .ptr = ptr,
                .len = size,
            }};
}

void free(Raw raw) {
    ::free(raw.ptr);
}
}  // namespace Raw_

struct IAllocator {
    void* obj;
    Errorable<Raw> (*allocate_raw)(void* obj, usize);
    void (*free_raw)(void* obj, Raw);
};

namespace IAllocator_ {
Errorable<Raw> allocate_raw(IAllocator allocator, usize size) {
    return allocator.allocate_raw(allocator.obj, size);
}

void free_raw(IAllocator allocator, Raw raw) {
    allocator.free_raw(allocator.obj, raw);
}

template <class T>
Errorable<T*> allocate(IAllocator allocator) {
    let raw_option = allocate_raw(allocator, sizeof(T));
    return_error(raw_option);

    let raw = Errorable_::unwrap(raw_option);
    return {CoreError::Correct, (T*)raw.ptr};
}

template <class T>
Errorable<T*> allocate_array(IAllocator allocator, usize size) {
    let raw_option = allocate_raw(allocator, sizeof(T) * size);
    return_error(raw_option);

    let raw = Errorable_::unwrap(raw_option);
    return {CoreError::Correct, (T*)raw.ptr};
}

template <class T>
void free(IAllocator allocator, T* ptr) {
    let raw = Raw{
        .ptr = (void*)ptr,
        .len = sizeof(T),
    };
    free_raw(allocator, raw);
}

template <class T>
void free_array(IAllocator allocator, T* ptr, usize size) {
    let raw = Raw{
        .ptr = (void*)ptr,
        .len = sizeof(T) * size,
    };
    free_raw(allocator, raw);
}
}  // namespace IAllocator_

struct BasicAllocator {};

namespace BasicAllocator_ {
Errorable<Raw> allocate_raw(BasicAllocator* basic_allocator, usize size) {
    return Raw_::allocate(size);
};

void free_raw(BasicAllocator* basic_allocator, Raw raw) {
    Raw_::free(raw);
};

IAllocator to_interface(BasicAllocator* basic_allocator) {
    return IAllocator{
        .obj = basic_allocator,
        .allocate_raw = [](void* obj,
                           usize size) { return allocate_raw((BasicAllocator*)obj, size); },
        .free_raw = [](void* obj, Raw raw) { free_raw((BasicAllocator*)obj, raw); },
    };
}
}  // namespace BasicAllocator_

template <usize max_size = 2 * KiB>
struct TemporaryAllocator {
    byte  memory[max_size];
    usize current = 0;
};

namespace TemporaryAllocator_ {
template <usize max_size>
Errorable<Raw> allocate_raw(TemporaryAllocator<>* temp_allocator, usize size) {
    if (temp_allocator->current + size > max_size) temp_allocator->current = 0;

    Raw raw{
        .ptr = (void*)(temp_allocator->memory + temp_allocator->current),
        .len = size,
    };

    temp_allocator->current += size;

    return {CoreError::Correct, raw};
}

template <usize max_size>
void free_raw(TemporaryAllocator<>* temp_allocator, const Raw& raw) {
}

template <usize max_size>
IAllocator to_interface(TemporaryAllocator<>* temp_allocator) {
    return IAllocator{
        .obj = temp_allocator,
        .allocate_raw =
            [](void* obj, usize size) {
                return allocate_raw((TemporaryAllocator<max_size>*)obj, size);
            },
        .free_raw = [](void* obj, Raw raw) { free_raw((TemporaryAllocator<max_size>*)obj, raw); },
    };
}
}  // namespace TemporaryAllocator_

template <usize max_size = 2 * KiB>
struct StackAllocator {
    byte  memory[max_size];
    usize current = 0;
};

namespace StackAllocator_ {
template <usize max_size>
Errorable<Raw> allocate_raw(StackAllocator<max_size>* stack_allocator, usize size) {
    if (stack_allocator->current + size > max_size) return {MemoryError::NotEnoughSpace};

    let raw = Raw{
        .ptr = stack_allocator->memory + stack_allocator->current,
        .len = size,
    };

    stack_allocator->current += size;

    return {CoreError::Correct, raw};
}

template <usize max_size>
void free_raw(StackAllocator<max_size>* stack_allocator, const Raw& raw) {
    if (raw.ptr == stack_allocator->memory + stack_allocator->current - raw.len) {
        stack_allocator->current -= raw.len;
    }
}

template <usize max_size>
void free_all(StackAllocator<max_size>* stack_allocator) {
    stack_allocator->current = 0;
}

template <usize max_size>
IAllocator to_interface(StackAllocator<max_size>* stack_allocator) {
    return IAllocator{
        .obj = stack_allocator,
        .allocate_raw =
            [](void* obj, usize size) {
                return allocate_raw((StackAllocator<max_size>*)obj, size);
            },
        .free_raw = [](void* obj, Raw raw) { free_raw((StackAllocator<max_size>*)obj, raw); },
    };
}
}  // namespace StackAllocator_

// TODO(Ferenc): add DebugAllocator

template <usize page_size = 2 * KiB>
struct PageAllocator {
    struct Page {
        byte  memory[page_size];
        Page* next = nullptr;
    };

    Page* head = nullptr;
    usize pos = 0;
};

namespace PageAllocator_ {
template <usize page_size>
Errorable<Raw> allocate_raw(PageAllocator<page_size>* page_allocator, usize size) {
    if (size > page_size) return {MemoryError::NotEnoughSpace};

    if (page_allocator->head == nullptr) {
        let raw_option = Raw_::allocate(sizeof(PageAllocator<page_size>::Page));
        return_error(raw_option);
        let raw = Errorable_::unwrap(raw_option);

        page_allocator->head = (typename PageAllocator<page_size>::Page*)raw.ptr;
        page_allocator->pos = size;

        let allocated_raw = page_allocator->head->memory;
        return {CoreError::Correct,
                Raw{
                    .ptr = allocated_raw,
                    .len = size,
                }};
    }

    if (page_allocator->pos + size <= page_size) {
        let ptr = page_allocator->head->memory + page_allocator->pos;
        page_allocator->pos += size;

        return {CoreError::Correct,
                {
                    .ptr = ptr,
                    .len = size,
                }};
    }

    let new_page_option = Raw_::allocate(sizeof(PageAllocator<page_size>::Page));
    return_error(new_page_option);
    let new_page =
        (typename PageAllocator<page_size>::Page*)Errorable_::unwrap(new_page_option).ptr;

    new_page->next = page_allocator->head;
    page_allocator->head = new_page;

    let ptr = page_allocator->head->memory;
    page_allocator->pos = size;

    return {CoreError::Correct,
            Raw{
                .ptr = ptr,
                .len = size,
            }};
};

template <usize page_size>
void free_raw(PageAllocator<page_size>* page_allocator, const Raw& raw){};

template <usize page_size>
void destroy_page(typename PageAllocator<page_size>::Page* page) {
    if (page == nullptr) return;
    destroy_page(page->next);
    ::free({.ptr = page, .len = sizeof(PageAllocator<page_size>::Page)});
}

template <usize page_size>
void free_all(PageAllocator<page_size>* page_allocator) {
    destroy_page(page_allocator->head);
    page_allocator->head = nullptr;
}

template <usize page_size>
IAllocator to_interface(PageAllocator<page_size>* page_allocator) {
    return IAllocator{
        .obj = page_allocator,
        .allocate_raw =
            [](void* obj, usize size) {
                return allocate_raw(*(PageAllocator<page_size>*)obj, size);
            },
        .free_raw = [](void* obj, Raw raw) { free_raw(*(PageAllocator<page_size>*)obj, raw); },
    };
}
}  // namespace PageAllocator_


// --- /Allocators ---

/*
 * Wrapper around memcpy
 * so it takes the type in account
 */
template <class T>
void typed_memcpy(T* __restrict dest, const T* __restrict src, usize n) {
    memcpy(dest, src, sizeof(T) * n);
}

/*
 * It is like memset but takes the type in account
 */
template <class T>
void typed_memset(T* dest, T elem, usize n) {
    for (usize i = 0; i < n; i++) {
        dest[i] = elem;
    }
}
