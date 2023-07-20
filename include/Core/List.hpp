#pragma once

#include <exception>

#include "Array.hpp"
#include "Context.hpp"
#include "Intrinsics.hpp"
#include "Memory.hpp"
#include "Pair.hpp"
#include "Queue.hpp"

/*
 * Basic dynamic list
 */
template <class T>
struct List {
    T*         ptr;
    usize      size;
    usize      capacity;
    Allocator* allocator;

    static List<T> create(Context context) {
        List<T> list;
        list.allocator = context.allocator;
        list.size = 0;
        list.capacity = 1;
        let try_ptr = list.allocator->template allocate_array<T>(1);
        list.ptr = unwrap(try_ptr);

        return list;
    }

    static List<T> create() {
        return List<T>::create(default_context());
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
void destroy(List<T>& list) {
    list.allocator->template free_array(list.ptr, list.capacity);
}

template <class T>
List<T> clone(List<T> list, Context context) {
    List<T> new_list;

    new_list.allocator = context.allocator;
    let try_ptr = new_list.allocator->template allocate_array<T>(list.capacity);
    new_list.ptr = unwrap(try_ptr);
    typed_memcpy(new_list.ptr, list.ptr, list.size);

    new_list.size = list.size;
    new_list.capacity = list.capacity;
    return new_list;
}

template <class T>
List<T> clone(List<T> list) {
    return clone(list, Context{.allocator = list.allocator});
}

/*
 * Ensures that the capacity of the
 * list is at least new_capacity
 */
template <class T>
void reserve(List<T>& list, usize new_capacity) {
    if (new_capacity <= list.capacity) return;

    let try_ptr = list.allocator->template allocate_array<T>(new_capacity);
    let ptr = unwrap(try_ptr, "upsize");
    typed_memcpy(ptr, list.ptr, list.size);
    list.allocator->template free_array(list.ptr, list.capacity);
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
inline bool empty(List<T>& list) {
    return list.size == 0;
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

template <class T>
Array<T> to_array(List<T>& list, Context context) {
    let array = Array<T>::create(list.size, context);
    typed_memcpy(array.ptr, list.ptr, list.size);
    array.size = list.size;
    return array;
}

template <class T>
Array<T> to_array(List<T>& list) {
    return to_array(list, Context{.allocator = list.allocator});
}
