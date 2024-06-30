#pragma once

#include "../core.hpp"

/*
interface In_Stream{
    usize read(In_Stream stream, Slice<u8> slice);
}
*/

using In_Stream_Proc = usize(*)(void*, Slice<u8>);

struct In_Stream{
    void* data = NULL;
    In_Stream_Proc proc = NULL;
};

usize read(In_Stream stream, Slice<u8> slice){
    return stream.proc(stream.data, slice);
}

template <interface In_Stream>
u64 scan_uint(In_Stream){
    panic("implement");
    return 0;
}


/*
interface Out_Stream{
    usize write(Out_Stream stream, Slice<u8> slice);
}
*/

using Out_Stream_Proc = usize(*)(void*, Slice<u8>);

struct Out_Stream{
    void* data = NULL;
    Out_Stream_Proc proc = NULL;
};

usize write(Out_Stream stream, Slice<u8> slice){
    return stream.proc(stream.data, slice);
}



template <interface Out_Stream, class T>
usize write_fmt(Out_Stream stream, T elem){
    panic("write: uninmplemented");
    UNREACHABLE;
}


template <interface Out_Stream>
usize write_fmt(Out_Stream stream, bool elem){
    if (elem)
        return write_fmt(stream, "true");
    else
        return write_fmt(stream, "false");
}

template <interface Out_Stream, class T>
usize write_fmt(Out_Stream stream, T* ptr){
    //todo(Ferenc): Do something lol
    switch (sizeof(void*)){
    case sizeof(u64): return write_fmt(stream, cast(u64)ptr);
    case sizeof(u32): return write_fmt(stream, cast(u32)cast(u64)ptr);
    case sizeof(u16): return write_fmt(stream, cast(u16)cast(u64)ptr);
    case sizeof(u8):  return write_fmt(stream, cast(u8) cast(u64)ptr);
    }
}

template <interface Out_Stream, class T>
usize write_fmt(Out_Stream stream, Slice<T> slice){
    let n = 0;
    n += write_fmt(stream, "s[");
    For_Each(iter(slice)){
        n += write_fmt(stream, "%, ", it.value);
    }
    n += write_fmt(stream, "]");
    return n;   
}


template <interface Out_Stream>
usize write_fmt(Out_Stream stream, char chr){
    return write(stream, bytes_of(&chr));
}


template <interface Out_Stream>
usize write_fmt(Out_Stream stream, u64 number){
    u8 buffer[30];
    usize pos = 30 - 1;

    while (true){
        let digit = number % 10;
        number /= 10;
        buffer[pos] = '0' + digit;
        pos -= 1;
        if (number == 0) break;
    }

    let slice = slice_from(buffer, pos + 1, 29 - pos);
    return write(stream, slice);
}

template <interface Out_Stream>
inline
usize write_fmt(Out_Stream stream, u32 number){ return write_fmt(stream, cast(u64) number); }

template <interface Out_Stream>
inline
usize write_fmt(Out_Stream stream, u16 number){ return write_fmt(stream, cast(u64) number); }

template <interface Out_Stream>
inline
usize write_fmt(Out_Stream stream, u8  number){ return write_fmt(stream, cast(u64) number); }

template <interface Out_Stream>
usize write_fmt(Out_Stream stream, i64 number){
    bool negative = number < 0;
    if (negative){
        number *= -1;
    }

    u8 buffer[30];
    usize pos = 30 - 1;

    while (true){
        let digit = number % 10;
        number /= 10;
        buffer[pos] = '0' + digit;
        pos -= 1;
        if (number == 0) break;
    }

    if (negative){
        buffer[pos] = '-';
        pos -= 1;
    }

    let slice = slice_from(buffer, pos + 1, 29 - pos);
    return write(stream, slice);
}

template <interface Out_Stream>
inline
usize write_fmt(Out_Stream stream, i32 number){ return write_fmt(stream, cast(i64) number); }

template <interface Out_Stream>
inline
usize write_fmt(Out_Stream stream, i16 number){ return write_fmt(stream, cast(i64) number); }

template <interface Out_Stream>
inline
usize write_fmt(Out_Stream stream, i8  number){ return write_fmt(stream, cast(i64) number); }

template <interface Out_Stream>
usize write_fmt(Out_Stream stream, String str){
    return write(stream, raw(str));
}

template <interface Out_Stream>
usize write_fmt(Out_Stream stream, const char* c_str){
    return write_fmt(stream, alias(c_str));
}

template <interface Out_Stream>
usize write_fmt(Out_Stream stream, char* c_str){
    return write_fmt(stream, cast(const char*) c_str);
}

struct Format{
    const char* fmt = "\0";
    usize nr = 0;
};


template <interface Out_Stream, class T>
void format_helper(Out_Stream stream, Format* fmt, T elem, bool has_elem){
    while (*fmt->fmt != '\0'){
        defer (fmt->fmt++);
        let chr = *fmt->fmt;
        if (chr == '%'){
            if (has_elem){
                fmt->nr += write_fmt(stream, elem);
            } else {
                panic("format_helper: Too much elem in format string");
            }
            break;
        } else if (chr == '~'){
            fmt->fmt++;
            let chr = *fmt->fmt;
            
            switch (chr){
            case  '%':  
            case '~': fmt->nr += write_fmt(stream, chr); break;
            }
        } else {
            fmt->nr += write_fmt(stream, chr);
        }
    }
}


template <interface Out_Stream, class ...Args>
usize write_fmt(Out_Stream stream, const char* fmt, Args... arg){
    let format = Format{fmt};
    (format_helper(stream, &format, arg, true), ...);
    format_helper(stream, &format, 0, false);
    return format.nr;
}


/*
   Implemented with OS specific procedure
*/

template <class ...Args>
usize print_fmt(const char* fmt, Args... arg);

template <class ...Args>
usize println_fmt(const char* fmt, Args... arg);
