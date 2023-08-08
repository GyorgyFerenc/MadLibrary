#pragma once

#include <exception>

#include "Array.hpp"
#include "Context.hpp"
#include "Intrinsics.hpp"
#include "Memory.hpp"
#include "Pair.hpp"

/*
 * Basic dynamic list
 */
template <class T>
struct List {
    T*         ptr;
    usize      size;
    usize      capacity;
    IAllocator allocator;

    /*
     * This does not do boundary checking
     * use the "at" function for that
     */
    T& operator[](usize pos) {
        return this->ptr[pos];
    }
};

template <class T>
void init(List<T>& list, Context context) {
    list.allocator = context.allocator;
    list.size = 0;
    list.capacity = 1;
    let try_ptr = allocate_array<T>(list.allocator, 1);
    list.ptr = unwrap(try_ptr);
}

template <class T>
void destroy(List<T>& list) {
    free_array(list.allocator, list.ptr, list.capacity);
}

template <class T>
List<T> clone(List<T> list, Context context) {
    List<T> new_list;

    new_list.allocator = context.allocator;
    let try_ptr = allocate_array<T>(new_list.allocator, list.capacity);
    new_list.ptr = unwrap(try_ptr);
    typed_memcpy(new_list.ptr, list.ptr, list.size);

    new_list.size = list.size;
    new_list.capacity = list.capacity;
    return new_list;
}

/*
 * Ensures that the capacity of the
 * list is at least new_capacity
 */
template <class T>
void reserve(List<T>& list, usize new_capacity) {
    if (new_capacity <= list.capacity) return;

    let try_ptr = allocate_array<T>(list.allocator, new_capacity);
    let ptr = unwrap(try_ptr, "upsize");
    typed_memcpy(ptr, list.ptr, list.size);
    free_array(list.allocator, list.ptr, list.capacity);
    list.ptr = ptr;
    list.capacity = new_capacity;
}

template <class T>
void add(List<T>& list, T element) {
    // NOTE(Ferenc): maybe add a fall of to the multiplier as the capacity grows
    if (list.size == list.capacity) reserve(list, 2 * list.capacity);

    list.ptr[list.size] = element;

    list.size++;
}

template <class T>
Error insert(List<T>& list, usize pos, T element) {
    if (list.size <= pos) return CoreError::OutOfRange;
    list.size++;
    if (list.size == list.capacity) reserve(list, 2 * list.capacity);

    for (usize i = list.size - 1; i > pos; i--) {
        list[i] = list[i - 1];
    }

    list[pos] = element;

    return CoreError::Correct;
}

template <class T>
Errorable<T> remove(List<T>& list, usize pos) {
    if (list.size <= pos) return {CoreError::OutOfRange};

    let element = list.ptr[pos];
    for (usize i = pos; i < list.size - 1; i++) {
        list.ptr[i] = list.ptr[i + 1];
    }
    list.size--;

    return {CoreError::Correct, element};
}

/*
 * Does not do boundary checks
 */
template <class T>
T& top(List<T>& list) {
    return list.ptr[list.size - 1];
}

/*
 * Does not do boundary checks
 */
template <class T>
T& pop(List<T>& list) {
    // NOTE(Ferenc): If shrinking would be added
    // a refactor is needed
    list.size--;
    return list.ptr[list.size];
}

template <class T>
Errorable<T*> pop_safe(List<T>& list) {
    if (empty(list)) return {CoreError::OutOfRange};
    list.size--;
    return {CoreError::Correct, &list.ptr[list.size]};
}

/*
 * It errors if the pos is out of range
 */
template <class T>
Errorable<T*> at(List<T>& list, usize pos) {
    if (pos >= list.size) return {CoreError::OutOfRange};
    return {CoreError::Correct, &list.ptr[pos]};
}

template <class T>
void clear(List<T>& list) {
    list.size = 0;
}

template <class T>
inline bool empty(List<T>& list) {
    return list.size == 0;
}

template <class T>
Array<T> to_array(List<T>& list, Context context) {
    Array<T> array;
    init(array, list.size, context);
    typed_memcpy(array.ptr, list.ptr, list.size);
    array.size = list.size;
    return array;
}

/*
 * Reverses the list inplace
 */
template <class T>
void reverse(List<T>& list) {
    for (usize i = 0; i < list.size / 2; i++) {
        T temp = list[i];
        list[i] = list[list.size - i - 1];
        list[list.size - i - 1] = temp;
    }
}

template <class T>
struct ListIter {
    List<T>* list;
    usize    current = 0;
};

template <class T>
Pair<bool, T*> next(ListIter<T>& iter) {
    if (iter.current == iter.list->size) return {false};
    return {true, &(*iter.list)[iter.current++]};
}

template <class T>
inline ListIter<T> iter(List<T>& list) {
    return ListIter<T>{&list};
}
