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

#define CONSTRUCT(comp, ...) \
    __COMPOSITION_IMPLEMENTATION(comp) { __VA_ARGS__ }

#endif