#pragma once

#ifndef MADLIBRARY_COMPOSITION
#define MADLIBRARY_COMPOSITION

#define __COMPOSITION_IMPLEMENTATION(comp) _##comp

// Compose the type comp to the current class
// Use the as macro for casting to components
#define COMPOSE(comp) comp __COMPOSITION_IMPLEMENTATION(comp)

// Cast the variable to the component comp
#define AS(variable, comp) variable.__COMPOSITION_IMPLEMENTATION(comp)

// Cast the "this" keyword to an object of comp
// Use only inside class methods
#define THIS_AS(comp) this->__COMPOSITION_IMPLEMENTATION(comp)

// Constructs the underlying component.
// It must be used inside an initializer list
#define CONSTRUCT(comp, ...) \
    __COMPOSITION_IMPLEMENTATION(comp) { __VA_ARGS__ }

#define DISPATCH_INIT(func, ret, ...)                  \
    template <class self>                              \
    using func_ptr_##func = ret (*)(__VA_ARGS__);      \
    template <class T>                                 \
    func_ptr_##func<T> dispatch_##func(const T& obj) { \
        throw std::runtime_error{"Not implemented"};   \
    }

#define DISPATCH(func, type) \
    inline func_ptr_##func<type> dispatch_##func(const type& obj) { return func; }

// #define DISPATCH_CALL(func, obj) dispatch_##func(obj)(obj)
#define CALL(func, ...) dispatch_##func(obj)(__VA_ARGS__)


#endif