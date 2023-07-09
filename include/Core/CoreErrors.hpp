#pragma once

#include "Intrinsics.hpp"

namespace CoreError {
let OutOfRange = declare_error();
let EmptyAcces = declare_error();  // accesing element in an empty container
let InvalidOperation = declare_error();
}  // namespace CoreError
