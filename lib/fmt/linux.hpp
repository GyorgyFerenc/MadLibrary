#pragma once

#include "shared.hpp"
#include <unistd.h>

struct Fd_Stream{
    int fd;
};

usize write(Fd_Stream stream, Slice<u8> slice){
    return write(stream.fd, slice.buffer, slice.len);
}

usize read(Fd_Stream stream, Slice<u8> slice){
    return read(stream.fd, slice.buffer, slice.len);
}

constexpr let STD_IN  = Fd_Stream{0};
constexpr let STD_OUT = Fd_Stream{1};
constexpr let STD_ERR = Fd_Stream{2};

Out_Stream out_stream(Fd_Stream* stream){
    return {
        .data = stream, 
        .proc = [](void* stream, Slice<u8> slice) -> usize{
            return write(*(cast(Fd_Stream*) stream), slice);
        }
    };
}

template <class ...Args>
usize print_fmt(const char* fmt, Args... arg){
    return write_fmt(STD_OUT, fmt, arg...);
}


template <class ...Args>
usize println_fmt(const char* fmt, Args... arg){
    let n = write_fmt(STD_OUT, fmt, arg...);
    n += write_fmt(STD_OUT, '\n');
    return n;
}
