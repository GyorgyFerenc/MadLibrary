#include <cstdio>
#include "Core.hpp"

using Utf8_Scanner_Proc = Option<u8>(*)(void *);

struct Utf8_Scanner{
    Rune current = 0;
    Rune peek    = 0;

    Utf8_Scanner_Proc scan_proc;
    void* user_data;
};

Rune Utf8_Scanner_advance(Utf8_Scanner* scanner, usize nr = 1);

/*
 * It calls it fills up current and peek as well
 */
Utf8_Scanner Utf8_Scanner_create(Utf8_Scanner_Proc proc, void* user_data = NULL){
    let scanner = Utf8_Scanner {
        .current   = 0,
        .peek      = 0,
        .scan_proc = proc,
        .user_data = user_data,
    };

    Utf8_Scanner_advance(&scanner, 2);
    return scanner;
}

/*
 * Returns current
 */
Rune Utf8_Scanner_advance(Utf8_Scanner* scanner, usize nr){
    let scan_unwrap = [](Utf8_Scanner_Proc proc, void* data) -> u8{
        return Option_::unwrap(proc(data));
    };

    for (usize i = 0; i < nr; i++){
        scanner->current = scanner->peek;

        let try_byte = scanner->scan_proc(scanner->user_data);
        if (!try_byte.some) {
            scanner->peek = 0;
            continue;
        } 

        u8 bytes[4] = {0};
        bytes[0] = try_byte.value;

        switch (Rune_::len_in_byes_from_first_byte(bytes[0])) {
        case 2:{
            bytes[1] = scan_unwrap(scanner->scan_proc, scanner->user_data);
        }break;
        case 3:{
            bytes[1] = scan_unwrap(scanner->scan_proc, scanner->user_data);
            bytes[2] = scan_unwrap(scanner->scan_proc, scanner->user_data);
        }break;
        case 4:{
            bytes[1] = scan_unwrap(scanner->scan_proc, scanner->user_data);
            bytes[2] = scan_unwrap(scanner->scan_proc, scanner->user_data);
            bytes[3] = scan_unwrap(scanner->scan_proc, scanner->user_data);
        }break;
        }
        let [r, _] = Rune_::decode_from_utf8(bytes);
        scanner->peek = r;
    }
    return scanner->current;
}

/*
 * Returns the number of runes skipped
 */
inline
usize Utf8_Scanner_skip(Utf8_Scanner* scanner, bool(*predicate)(Rune)){
    let nr = 0;
    while (predicate(scanner->current)) {
        Utf8_Scanner_advance(scanner);
        nr++;
    }
    return nr;
}

/*
 * Returns the number of runes skipped
 */
inline
usize Utf8_Scanner_skip_whitespace(Utf8_Scanner* scanner){
    return Utf8_Scanner_skip(scanner, Rune_::whitespace);
}


/*
 * Returns the number of runes skipped
 */
inline
usize Utf8_Scanner_skip_rune(Utf8_Scanner* scanner, Rune rune){
    let nr = 0;
    while (scanner->current == rune) {
        Utf8_Scanner_advance(scanner);
        nr++;
    }
    return nr;
}


/*
 * Returns the number of runes skipped
 */
inline
usize Utf8_Scanner_skip_char(Utf8_Scanner* scanner, char chr){
    let rune = Rune_::from_char(chr);
    let nr = 0;
    while (scanner->current == rune) {
        Utf8_Scanner_advance(scanner);
        nr++;
    }
    return nr;
}


/*
 * returns true if it can be lexed
 *  -> current is set to after the last rune
 * returns false if it could not be
 *  -> current is set to be the last rune that did'nt match
 */
inline
bool Utf8_Scanner_scan_exact(Utf8_Scanner* scanner, String str){
    For_Each(String_::iter_rune(str)){
        if (scanner->current != it.rune) return false;
        Utf8_Scanner_advance(scanner);
    }
    return true;
}

/*
 * Reads an integer.
 * Supported formats are
 *  - decimal e.g. 123
 *  - hexadecimal e.g. 0xaF
 *  - binary e.g. 0b101
 *  a '+' or '-' sign can be before the number.
 *  _ can be put between digits
 *  Leading zeros are supported
 */
template <class Integer = i64>
inline
Option<Integer> Utf8_Scanner_scan_int(Utf8_Scanner* scanner){
    let negative = false;
    Integer number = 0;

    if (scanner->current == '+'){
        negative = false;
        Utf8_Scanner_advance(scanner);
    }else if (scanner->current == '-'){
        negative = true;
        Utf8_Scanner_advance(scanner);
    }

    let scanned_one = false;
    let scan_hexa = scanner->current == '0' &&
                    scanner->peek    == 'x';
    let scan_binary = scanner->current == '0' &&
                      scanner->peek    == 'b';

    if (scan_hexa){ // hexadecimal
        Utf8_Scanner_advance(scanner, 2);
        scanned_one = Utf8_Scanner_skip_char(scanner, '0') != 0;
        while (true){
            let digit = 0;
            if (Rune_::digit(scanner->current)){
                digit = scanner->current - '0';
            } else if('A' <= scanner->current && scanner->current <= 'F'){
                digit = 10 + scanner->current - 'A';
            } else if('a' <= scanner->current && scanner->current <= 'f'){
                digit = 10 + scanner->current - 'a';
            } else if(scanner->current == '_'){
                Utf8_Scanner_advance(scanner);
                continue;
            } else {
                break;
            }

            number = number * 16 + digit;
            Utf8_Scanner_advance(scanner);
            scanned_one = true;
        }

    } else if (scan_binary){ // binary
        Utf8_Scanner_advance(scanner, 2);
        scanned_one = Utf8_Scanner_skip_char(scanner, '0') != 0;
        while (true){
            let digit = 0;
            if ('0' <= scanner->current && scanner->current <= '1'){
                digit = scanner->current - '0';
            } else if(scanner->current == '_'){
                Utf8_Scanner_advance(scanner);
                continue;
            } else {
                break;
            }
            number = number * 2 + digit;
            Utf8_Scanner_advance(scanner);
            scanned_one = true;
        }
    } else { // Decimal
        Utf8_Scanner_skip_char(scanner, '0');
        while (true){
            let digit = 0;
            if (Rune_::digit(scanner->current)){
                digit = scanner->current - '0';
            } else if(scanner->current == '_'){
                Utf8_Scanner_advance(scanner);
                continue;
            } else {
                break;
            }
            number = number * 10 + digit;
            Utf8_Scanner_advance(scanner);
            scanned_one = true;
        }
    }

    if (!scanned_one){ return {false}; }

    if (negative) return {true, (Integer) -number};
    else          return {true, (Integer)  number};
}


/*
 * Reads a float.
 * Supported formats are
 *  - decimal e.g.  123
 *  - precison e.g. 123.12
 *  a '+' or '-' sign can be before the number.
 *  Leading zeros are supported
 *
 *  TODO(Ferenc): Support hexadecimal for exact bits of 
 *  the float for perfect presision
 */
template <class Float = f64>
inline
Option<Float> Utf8_Scanner_scan_float(Utf8_Scanner* scanner){
    let negative = false;
    Float number = 0;

    if (scanner->current == '+'){
        negative = false;
        Utf8_Scanner_advance(scanner);
    }else if (scanner->current == '-'){
        negative = true;
        Utf8_Scanner_advance(scanner);
    }

    let nr = Utf8_Scanner_skip_char(scanner, '0');
    if (nr == 0 && !Rune_::digit(scanner->current)){ return {false}; }
    while (Rune_::digit(scanner->current)){
        number = number * 10 + (scanner->current - '0');
        Utf8_Scanner_advance(scanner);
    }
    Float fractional = 10.0;
    if (scanner->current == '.'){ 
        Utf8_Scanner_advance(scanner);
        while (Rune_::digit(scanner->current)){
            number += (scanner->current - '0') / fractional;
            fractional *= 10;
            Utf8_Scanner_advance(scanner);
        }
    }

    if (negative) return {true, -number};
    else          return {true,  number};
}


inline
Option<bool> Utf8_Scanner_scan_bool(Utf8_Scanner* scanner){
    if (scanner->current == 't'){
        let ok = Utf8_Scanner_scan_exact(scanner, String_::alias("true"));
        return {ok, true};

    } else if (scanner->current == 'f'){
        let ok = Utf8_Scanner_scan_exact(scanner, String_::alias("false"));
        return {ok, false};

    } else { 
        return {false};
    }
    UNREACHABLE;
}

Utf8_Scanner Utf8_Scanner_cin_scanner(){
    return Utf8_Scanner_create([](void* data) -> Option<u8>{
        u8 byte;
        fread(&byte, sizeof(u8), 1, stdin);
        return {true, byte};
    });
}
