#include <cstdio>
#include "Core.hpp"

using Utf8_Scanner_Proc = Option<u8>(*)(void *);

struct Utf8_Scanner{
    Rune current = 0;

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
        //.peek      = 0,
        .scan_proc = proc,
        .user_data = user_data,
    };

    Utf8_Scanner_advance(&scanner, 1);
    return scanner;
}

/*
 * Returns current
 */
Rune Utf8_Scanner_advance(Utf8_Scanner* scanner, usize nr){
    let scan_unwrap = [](Utf8_Scanner_Proc proc, void* data) -> u8{
        return unwrap(proc(data));
    };

    for (usize i = 0; i < nr; i++){
        //scanner->current = scanner->peek;

        let try_byte = scanner->scan_proc(scanner->user_data);
        if (!try_byte.some) {
            scanner->current = 0;
            break;
        } 

        u8 bytes[4] = {0};
        bytes[0] = try_byte.value;

        switch (len_in_byes_from_first_byte(bytes[0])) {
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
        let [r, _] = decode_from_utf8(bytes);
        scanner->current = r;
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
    return Utf8_Scanner_skip(scanner, whitespace);
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
    let rune = from_char(chr);
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
    For_Each(iter_rune(str)){
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


    let scan_hexa    = false;    
    let scan_binary  = false;
    let scan_decimal = false;

    if (scanner->current == '0'){
        scan_decimal = true;
        let r = Utf8_Scanner_advance(scanner);
        if (r == 'x'){
            scan_hexa = true;
            Utf8_Scanner_advance(scanner);
        } else if (r == 'b'){
            scan_binary = true;
            Utf8_Scanner_advance(scanner);
        } else if (digit(scanner->current)){
            scan_decimal = true;
        } else {
            return {true, 0};
        }
    } else if (digit(scanner->current)){
        scan_decimal = true;
    } else {
        return {false};
    }

    if (scan_hexa){ // hexadecimal
        let scanned_one  = Utf8_Scanner_skip_char(scanner, '0') != 0;
        while (true){
            let d = 0;
            if (digit(scanner->current)){
                d = scanner->current - '0';
            } else if('A' <= scanner->current && scanner->current <= 'F'){
                d = 10 + scanner->current - 'A';
            } else if('a' <= scanner->current && scanner->current <= 'f'){
                d = 10 + scanner->current - 'a';
            } else if(scanner->current == '_'){
                Utf8_Scanner_advance(scanner);
                continue;
            } else {
                break;
            }

            number = number * 16 + d;
            Utf8_Scanner_advance(scanner);
            scanned_one = true;
        }
        if (!scanned_one) return {false};

    } else if (scan_binary){ // binary
        let scanned_one = Utf8_Scanner_skip_char(scanner, '0') != 0;
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
        if (!scanned_one) return {false};
    } else if (scan_decimal){ // Decimal
        Utf8_Scanner_skip_char(scanner, '0');
        while (true){
            let d = 0;
            if (digit(scanner->current)){
                d = scanner->current - '0';
            } else if(scanner->current == '_'){
                Utf8_Scanner_advance(scanner);
                continue;
            } else {
                break;
            }
            number = number * 10 + d;
            Utf8_Scanner_advance(scanner);
        }
    } else {
        return {false};
    }


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
    if (nr == 0 && !digit(scanner->current)){ return {false}; }
    while (digit(scanner->current)){
        number = number * 10 + (scanner->current - '0');
        Utf8_Scanner_advance(scanner);
    }
    Float fractional = 10.0;
    if (scanner->current == '.'){ 
        Utf8_Scanner_advance(scanner);
        while (digit(scanner->current)){
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
        let ok = Utf8_Scanner_scan_exact(scanner, alias("true"));
        return {ok, true};

    } else if (scanner->current == 'f'){
        let ok = Utf8_Scanner_scan_exact(scanner, alias("false"));
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
