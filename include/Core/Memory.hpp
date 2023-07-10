#pragma once
#include <cstddef>

#include "Format.hpp"
#include "Intrinsics.hpp"

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
};

namespace MemoryError {
Error NotEnoughSpace = declare_error();
Error Allocation = declare_error();
}  // namespace MemoryError

Errorable<Raw> chunck(Raw& raw, usize new_len, usize offset = -1) {
    if (new_len + offset > raw.len) {
        return {MemoryError::NotEnoughSpace};
    }
    return {Correct,
            Raw{
                .ptr = (byte*)raw.ptr + offset,
                .len = new_len,
            }};
}

template <class T>
Errorable<T*> cast(Raw& raw) {
    if (raw.len < sizeof(T)) return {MemoryError::NotEnoughSpace};
    return {Correct, (T*)raw.ptr};
}

template <class T>
Errorable<T*> cast_array(Raw& raw, usize size) {
    if (raw.len < sizeof(T) * size) return {MemoryError::NotEnoughSpace};
    return {Correct, (T*)raw.ptr};
}

Errorable<Raw> allocate(usize size) {
    let* ptr = malloc(size);
    if (ptr == NULL) return {MemoryError::Allocation};

    return {Correct,
            Raw{
                .ptr = ptr,
                .len = size,
            }};
}

void free(const Raw& raw) {
    free(raw.ptr);
}

struct Allocator {
    virtual Errorable<Raw> allocate_raw(usize) = 0;
    virtual void           free_raw(const Raw&) = 0;

    template <class T>
    Errorable<T*> allocate() {
        let raw_option = this->allocate_raw(sizeof(T));
        return_error(raw_option);

        let raw = unwrap(raw_option);
        return {Correct, (T*)raw.ptr};
    }

    template <class T>
    Errorable<T*> allocate_array(usize size) {
        let raw_option = this->allocate_raw(sizeof(T) * size);
        return_error(raw_option);

        let raw = unwrap(raw_option);
        return {Correct, (T*)raw.ptr};
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
    Errorable<Raw> allocate_raw(usize size) override {
        return ::allocate(size);
    };

    void free_raw(const Raw& raw) override {
        ::free(raw);
    };
};

template <usize max_size = 2 * KiB>
struct TemporaryAllocator : public Allocator {
    Errorable<Raw> allocate_raw(usize size) override {
        if (m_current + size > max_size) m_current = 0;

        Raw raw{
            .ptr = (void*)(m_memory + m_current),
            .len = size,
        };

        m_current += size;

        return {Correct, raw};
    }

    void free_raw(const Raw& raw) override {
    }

   private:
    byte  m_memory[max_size];
    usize m_current = 0;
};

template <usize max_size = 2 * KiB>
struct StackAllocator : public Allocator {
    Errorable<Raw> allocate_raw(usize size) override {
        if (m_current + size > max_size) return {MemoryError::NotEnoughSpace};

        let raw = Raw{
            .ptr = m_memory + m_current,
            .len = size,
        };

        m_current += size;

        return {Correct, raw};
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

    Errorable<Raw> allocate_raw(usize size) override {
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
    Errorable<Raw> allocate_raw(usize size) override {
        if (size > page_size) return {MemoryError::NotEnoughSpace};

        if (m_head == nullptr) {
            let raw_option = ::allocate(sizeof(Page));
            return_error(raw_option);
            let raw = unwrap(raw_option);

            m_head = (Page*)raw.ptr;
            m_pos = size;

            let allocated_raw = m_head->memory;
            return {Correct,
                    Raw{
                        .ptr = allocated_raw,
                        .len = size,
                    }};
        }

        if (m_pos + size <= page_size) {
            let ptr = m_head->memory + m_pos;
            m_pos += size;

            return {Correct,
                    {
                        .ptr = ptr,
                        .len = size,
                    }};
        }

        let new_page_option = ::allocate(sizeof(Page));
        return_error(new_page_option);
        let new_page = (Page*)unwrap(new_page_option).ptr;

        new_page->next = m_head;
        m_head = new_page;

        let ptr = m_head->memory;
        m_pos = size;

        return {Correct,
                Raw{
                    .ptr = ptr,
                    .len = size,
                }};
    };

    void free_raw(const Raw& raw) override{};

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
