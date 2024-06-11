#pragma  once

#include "../core.hpp"

using IPv4 = u8[4];
struct Address{
    IPv4 ip;
    u16  port;
};
struct Socket;

struct File;

struct Thread;
struct Mutex;
struct Conditional_Variable;
