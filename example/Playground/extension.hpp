#pragma once

#define lambda(parameter_list, return_type, body) \
    [=] parameter_list -> return_type body

// Func should be pure
#define func [[nodiscard]] auto

// Proc has side effects
#define proc auto

#define let const auto
#define var auto
#define ref &
