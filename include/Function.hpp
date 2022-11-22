#include <memory>

#pragma once

// Creates a lambda function
#define lambda_ref(parameters, body) [&] parameters body
#define lambda_val(parameters, body) [=] parameters body

// Marks the declaration and the definition of a function.
#define func [[nodiscard]] auto

#define ret(Type) ->std::unique_ptr<Type>
#define ret_p(Type) ->Type
#define ret_s(Type) ->std::shared_ptr<Type>
#define ret_w(Type) ->std::weak_ptr<Type>

// Marks the fact that the parameter of a function is intended
// to be read, and not modified for the caller
#define in(variable, type) const std::unique_ptr<type>& variable
#define in_p(variable, type) const type& variable
#define in_s(variable, type) const std::shared_ptr<type>& variable
#define in_w(variable, type) const std::weak_ptr<type>& variable

// Marks the fact that the parameter of a function is intended
// to be an otput variable does modified by the function.
// The output variables value is not considered by the function
#define out(variable, type) std::unique_ptr<type>& variable
#define out_p(variable, type) type& variable
#define out_s(variable, type) std::sharshared_ptr<type>& variable
#define out_w(variable, type) std::weak_ptr<type>& variable

// Marks the fact that the parameter of a function is intended
// to be an otput variable does modified by the function.
// The output variables value is considered by the function
#define inout(variable, type) std::unique_ptr<type>& variable
#define inout_p(variable, type) type& variable
#define inout_s(variable, type) std::shshared_ptr<type>& variable
#define inout_w(variable, type) std::weak_ptr<type>& variable
