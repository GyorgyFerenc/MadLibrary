#include <cstdio>
#include "Core.hpp"

using Utf8_Scanner_Proc = Array<u8>(*)(void *);

struct Utf8_Scanner{
    Rune current = 0;

    usize peek_idx = 0;
    usize idx = 0;
    Array<u8> array;

    void* user_data = NULL;
    Utf8_Scanner_Proc scan_proc = NULL;
};

/*
 * we asume that the returned array contains correct utf8 and aligned currectly
 */
Rune next(Utf8_Scanner* scanner){
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
    let [rune, nr] = decode_from_utf8(ptr);
    scanner->idx = scanner->peek_idx;
    scanner->peek_idx += nr;
    scanner->current = rune;
    return scanner->current;
}
