#pragma once
#include <memory>
// Marks the declarations and definition of a variable
// deduces the type
#define let auto
#define let_ref auto&
#define let_const const auto&
#define let_weak std::weak_ptr<auto>

// Marks the declarations of a variable
#define var(variable, type) std::unique_ptr<type> variable
#define var_p(variable, type) type variable
#define var_s(variable, type) std::shared_ptr<type> variable
#define var_w(variable, type) std::weak_ptr<type> variable

#define null(type) std::unique_ptr<type>{};
#define new(type, ...) std::make_unique<type>(__VA_ARGS__)
#define new_shared(type, ...) std::make_shared<type>(__VA_ARGS__)

#define t_u(type) std::unique_ptr<type>
#define t_s(type) std::shared_ptr<type>
#define t_w(type) std::weak_ptr<type>

// Unpacks the tuple, array, to the variables provided
#define unpack(variable, ...) auto [variable, __VA_ARGS__]

#ifdef _GLIBCXX_TUPLE

#define tuple(type, ...) std::tuple<type, __VA_ARGS__>

#endif

// #define unique(type) std::unique_ptr<type>
// #define shared(type) std::shared_ptr<type>
