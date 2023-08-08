#pragma once

#include "Core/Array.hpp"
#include "Core/Format.hpp"
#include "Core/Intrinsics.hpp"
#include "Core/List.hpp"
#include "Core/Memory.hpp"
#include "Core/Pair.hpp"
#include "Core/Queue.hpp"
#include "Core/Stack.hpp"
#include "Core/String.hpp"
// --- Iter ---

/*
 * Iter must be an object
 * which has a function next, like:
 *
 * Pair<bool, T*> next(Iter& iter)
 *
 * where T* is a pointer to the type we iterating over
 * and iter is the provided iterator
 */
template <class Iter, class Func>
inline void for_each(Iter iter, Func func) {
    let a = next(iter);  // Pair<bool, T*>
    while (a.first) {
        func(*a.second);
        a = next(iter);
    }
}

/*
 * puts el in [low, high]
 */
template <class T>
T clamp(T el, T low, T high) {
    if (el <= low) return low;
    if (el >= high) return high;
    return el;
}

inline String to_string_uint(uint64 number, Context context) {
    char table[] = {
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
    };

    StringBuilder builder;
    init(builder, context);
    defer(destroy(builder));
    reserve(builder, 20);

    do {
        let digit = number % 10;
        number /= 10;
        add(builder, table[digit]);
    } while (number > 0);

    reverse(builder);
    return build(builder, context);
}

int64 abs(int64 number) {
    return number >= 0 ? number : -number;
}

inline String to_string_int(int64 number, Context context) {
    char table[] = {
        '0',
        '1',
        '2',
        '3',
        '4',
        '5',
        '6',
        '7',
        '8',
        '9',
    };
    StringBuilder builder;
    init(builder, context);
    defer(destroy(builder));
    reserve(builder, 21);

    let temp = number;

    do {
        let digit = abs(number % 10);
        number /= 10;
        add(builder, table[digit]);
    } while (number != 0);

    if (temp < 0) add(builder, '-');

    reverse(builder);
    return build(builder, context);
}
