#pragma once

// Creates a lambda function
#define lambda_ref(parameters, body) [&] parameters body
#define lambda_val(parameters, body) [=] parameters body

// Marks the declaration and the definition of a function.
#define func [[nodiscard]] auto

// Marks the fact that the parameter of a function is intended
// to be read, and not modified for the caller
#define in(variable, type) const type& variable

// Marks the fact that the parameter of a function is intended
// to be an otput variable does modified by the function.
// The output variables value is not considered by the function
#define out(variable, type) type& variable

// Marks the fact that the parameter of a function is intended
// to be an otput variable does modified by the function.
// The output variables value is considered by the function
#define inout(variable, type) type& variable
