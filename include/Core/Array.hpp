#pragma once

#include "Context.hpp"
#include "CoreErrors.hpp"
#include "Intrinsics.hpp"
#include "Memory.hpp"
#include "Pair.hpp"

/*
 * fixed size array
 */
template <class T>
struct Array {
    usize      size = 0;
    T*         ptr;
    Allocator* allocator;

    inline static Array<T> create(usize size, Context context) {
        Array<T> array;
        array.size = size;
        array.allocator = context.allocator;
        let try_ptr = context.allocator->allocate_array<T>(size);
        array.ptr = unwrap(try_ptr);

        return array;
    }

    inline static Array<T> create(usize size) {
        return create(size, default_context());
    }

    /*
     * This does not do boundary checking
     * use the "at" function for that
     */
    T& operator[](usize pos) {
        return this->ptr[pos];
    }
};

template <class T>
void destroy(Array<T> array) {
    array.allocator->template free_array(array.ptr, array.size);
}

template <class T>
Array<T> clone(Array<T> array, Context context) {
    Array<T> new_array;

    new_array.allocator = context.allocator;
    let try_ptr = new_array.allocator->template allocate_array<T>(array.size);
    new_array.ptr = unwrap(try_ptr);
    typed_memcpy(new_array.ptr, array.ptr, array.size);

    new_array.size = array.size;
    return new_array;
}

template <class T>
Array<T> clone(Array<T> array) {
    return clone(array, Context{.allocator = array.allocator});
}

/*
 * It errors if the pos is out of range
 */
template <class T>
Errorable<T*> at(Array<T>& array, usize pos) {
    if (pos >= array.size) return {CoreError::OutOfRange};
    return {Correct, &array.ptr[pos]};
}

template <class T>
inline bool empty(Array<T>& array) {
    return array.size == 0;
}

template <class T>
struct ArrayIter {
    Array<T>* array;
    usize     current = 0;
};

template <class T>
Pair<bool, T*> next(ArrayIter<T>& iter) {
    if (iter.current == iter.array->size) return {false};
    return {true, &(*iter.array)[iter.current++]};
}

template <class T>
inline ArrayIter<T> iter(Array<T>& array) {
    return ArrayIter<T>{&array};
}
// template <class T>
