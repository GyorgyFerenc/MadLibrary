#pragma once

#include "Core/Array.hpp"
#include "Core/Format.hpp"
#include "Core/Intrinsics.hpp"
#include "Core/List.hpp"
#include "Core/Memory.hpp"
#include "Core/Pair.hpp"
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
