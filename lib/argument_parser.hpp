#pragma once

#include "core.hpp"

struct Argument_Parser{
    Slice<String> args;
};

inline
Argument_Parser create_argument_parser(int argc, const char** argv, Allocator allocator){
    let parser = Argument_Parser{
        .args = create_slice<String>(allocator, argc - 1),
    };
    for (let i = 1; i < argc; i++){
        parser.args[i - 1] = alias(argv[i]);
    }
    return parser;
}

inline
usize len(Argument_Parser p){
    return p.args.len;
}

inline
bool empty(Argument_Parser p){
    return len(p) == 0;
}

inline
String current(Argument_Parser p){
    return p.args[0];
}

void shift(Argument_Parser* p){
    p->args = slice_from(p->args, 1);
}
