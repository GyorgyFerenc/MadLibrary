#pragma once

#define MADLIBRARY_MEMORY

namespace MadLibrary {
template <typename T>
struct Reference {};

template <typename T>
struct Ptr {
    Ptr() {
        this->ptr = nullptr;
    }

    Ptr(T* ptr) {
        this->ptr = ptr;
    }

    Ptr(Ptr<T>&& ptr) {
        ptr.reset();
    }

    ~Ptr() {
        this->reset();
    }

    void reset() {
        if (this->ptr == nullptr) return;
        delete this->ptr;
    }

    T& operator*() {
        return *this->ptr;
    }

    T* operator->() {
        return this->ptr;
    }

   private:
    T* ptr;
};

}  // namespace MadLibrary
