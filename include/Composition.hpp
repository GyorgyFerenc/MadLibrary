#pragma once
#include "Function.hpp"

// Compose the type comp to the current class
// Use the as macro for casting to components
#define compose(comp) comp _##comp

// Cast the variable to the component comp
#define as(variable, comp) variable._##comp

// Cast the "this" keyword to an object of comp
// Use only inside class methods
#define this_as(comp) this->_##comp

#define interface struct

#define composed_of(comp) comp& _##comp

namespace ml {
template <class T, class... Args>
func combine_composition(Args&... args)->T {
    return T{args...};
}
}  // namespace ml
