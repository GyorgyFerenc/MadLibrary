#pragma once
#include <cstdio>
#include "Core.hpp"

using Scanner_Proc = Array<u8>(*)(void *);

/*
 * Neet to call scanner_next() so current is set
 */
struct Scanner{
    Rune current = 0;

    usize peek_idx = 0;
    usize idx = 0;
    Array<u8> array;

    void* user_data = NULL;
    Scanner_Proc scan_proc = NULL;
};

/*
 * we asume that the returned array contains correct utf8 and aligned currectly
 */
Rune scanner_next(Scanner* scanner){
    if (scanner->array.size == 0 || scanner->peek_idx == scanner->array.size){
        scanner->current = 0;
        scanner->peek_idx = 0;
        scanner->array = scanner->scan_proc(scanner->user_data);
        if (scanner->array.size == 0) {
            scanner->current = 0;
            return 0;
        }
    }

    let ptr = scanner->array.ptr + scanner->peek_idx;
    let [rune, nr] = rune_decode_from_utf8(ptr);
    scanner->idx = scanner->peek_idx;
    scanner->peek_idx += nr;
    scanner->current = rune;
    return scanner->current;
}

Option<u64> scanner_scan_uint(Scanner* scanner){
    if (!rune_digit(scanner->current)){
        return {false};
    }

    u64 number = 0;
    while (rune_digit(scanner->current)) {
        let d = scanner->current - '0';
        number = number * 10 + d;
        scanner_next(scanner);
    }

    return {true, number};
}

Option<u64> string_parse_uint(String str){
    struct Data{
        String str;
        bool returned = false;
    } data;
    data.str = str;
    
    let scanner = Scanner{
        .user_data = &data,
        .scan_proc = [](void* ptr) -> Array<u8>{
            let data = (Data*)ptr;
            if (data->returned) {
                return array_empty<u8>();
            }
            data->returned = true;
            return data->str;
        },
    };

    scanner_next(&scanner);

    return scanner_scan_uint(&scanner);
}

