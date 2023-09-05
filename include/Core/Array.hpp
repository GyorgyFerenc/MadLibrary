#pragma once

#include "Context.hpp"
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
    IAllocator allocator;

    /*
     * This does not do boundary checking
     * use the "at" function for that
     */
    T& operator[](usize pos) {
        return this->ptr[pos];
    }

    const T& operator[](usize pos) const {
        return this->ptr[pos];
    }
};

namespace Array_ {
template <class T>
void init(Array<T>* array, usize size, Context context) {
    array->size = size;
    array->allocator = context.allocator;
    let try_ptr = IAllocator_::allocate_array<T>(context.allocator, size);
    array->ptr = Errorable_::unwrap(try_ptr);
}

template <class T>
Array<T> create(usize size, Context context) {
    Array<T> array;
    init(&array, size, context);
    return array;
}

template <class T>
void destroy(Array<T>* array) {
    IAllocator_::free_array(array->allocator, array->ptr, array->size);
}

template <class T>
Array<T> clone(Array<T> array, Context context) {
    Array<T> new_array;

    new_array.allocator = context.allocator;
    let try_ptr = IAllocator_::allocate_array<T>(new_array.allocator, array.size);
    new_array.ptr = Errorable_::unwrap(try_ptr);
    typed_memcpy(new_array.ptr, array.ptr, array.size);

    new_array.size = array.size;
    return new_array;
}

template <class T>
usize size(Array<T> array) {
    return array.size;
}

/*
 * It errors if the pos is out of range
 */
template <class T>
Errorable<T*> at(Array<T> array, usize pos) {
    if (pos >= array.size) return {CoreError::OutOfRange};
    return {CoreError::Correct, &array.ptr[pos]};
}

template <class T>
inline bool empty(Array<T> array) {
    return array.size == 0;
}

}  // namespace Array_
template <class T>
struct ArrayIter {
    Array<T>* array;
    usize     current = 0;
};

namespace ArrayIter_ {
template <class T>
Pair<bool, T*> next(ArrayIter<T>* iter) {
    if (iter->current == iter->array->size) return {false};
    return {true, &(*iter->array)[iter->current++]};
}
}  // namespace ArrayIter_

namespace Array_ {
/*
 * Iter needs the address as a reference to the array
 * it iterates over
 */
template <class T>
inline ArrayIter<T> iter(Array<T>* array) {
    return ArrayIter<T>{array};
}
}  // namespace Array_
