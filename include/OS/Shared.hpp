#pragma  once

#include "../Core.hpp"

using IPv4 = u8[4];
struct Address{
    IPv4 ip;
    u16  port;
};

struct Socket{
    int socket_descriptor;
    Address address;
};
