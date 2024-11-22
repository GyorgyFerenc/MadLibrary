#pragma once

#include "core.hpp"
#include "fmt.hpp"

struct String_Builder{
    Array<u8> bytes;
};

inline
String_Builder create_string_builder(Allocator allocator){
    return {
        .bytes = create_array<u8>(allocator),
    };
}

inline
void destroy(String_Builder builder){
    destroy(builder.bytes);
}

inline
String build(String_Builder builder, Allocator allocator){
    let slice = slice_from(builder.bytes);
    let c  = copy(slice, allocator);
    return alias(c);
}

inline
String alias(String_Builder builder){
    return alias(slice_from(builder.bytes));
}

inline
char* build_cstr(String_Builder builder, Allocator allocator){
    return cstr(alias(builder), allocator);
}

inline
void add(String_Builder* builder, String str){
    For_Each(byte_iter(str)){
        append(&builder->bytes, it.value);
    }
}

inline
void add(String_Builder* builder, const char* c_str){
    while (*c_str != '\0'){ 
        append(&builder->bytes, cast(u8)*c_str);
        c_str++;
    }
}

inline
void add_rune(String_Builder* builder, Rune rune){
    u8 buffer[5];
    let len = encode_to_utf8(rune, buffer);
    for (usize i = 0; i < len; i++){
        append(&builder->bytes, buffer[i]);
    }
}

inline
void clear(String_Builder* builder){
    clear(&builder->bytes);
}

inline
usize write(String_Builder* builder, Slice<u8> slice){
    For_Each(iter(slice)){
        append(&builder->bytes, it.value);
    }

    return slice.len;
}

inline
void add(String_Builder* builder, u64 number){
    write_fmt(builder, number);
}

inline
void add(String_Builder* builder, u32 number){
    add(builder, cast(u64) number);
}

inline
void add(String_Builder* builder, u16 number){
    add(builder, cast(u64) number);
}

inline
void add(String_Builder* builder, u8 number){
    add(builder, cast(u64) number);
}

inline
void add(String_Builder* builder, bool boolean){
    if (boolean)
        add(builder, "true");
    else
        add(builder, "false");
}
//Todo(Ferenc): Add remaining 'add' procedures
