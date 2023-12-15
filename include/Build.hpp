#pragma once

#include "Core.hpp"
#include <cstdio>
#include <cstdlib>
#include <iostream>


enum class Compiler{
    GCC,
};

struct Target {
    Allocator allocator;
    Dynamic_Array<const char*>                    args;
    Dynamic_Array<const char*>                    linked_files;
    Dynamic_Array<const char*>                    linked_search_directories;
    Dynamic_Array<const char*>                    include_search_directories;
    Dynamic_Array<Pair<const char*, const char*>> macros;

    Compiler compiler = Compiler::GCC;
    bool debug        = false;
    bool all_warnings = false;
    const char* output;
};


Target Target_create(Allocator allocator){
    return {
        .allocator                  = allocator,
        .args                       = Dynamic_Array_create<const char*>(allocator),
        .linked_files               = Dynamic_Array_create<const char*>(allocator),
        .linked_search_directories  = Dynamic_Array_create<const char*>(allocator),
        .include_search_directories = Dynamic_Array_create<const char*>(allocator),
        .macros                     = Dynamic_Array_create<Pair<const char*, const char*>>(allocator),
    };
}

void destory(Target* target){
    destroy(&target->args);
    destroy(&target->linked_files);
    destroy(&target->linked_search_directories);
    destroy(&target->include_search_directories);
    destroy(&target->macros);
}

void add_arg(Target* target, const char* arg){
    append(&target->args, arg);
}

void add_source(Target* target, const char* source){
    add_arg(target, source);
}

void link_file(Target* target, const char* file){
    append(&target->linked_files, file);
}

void add_link_search_directory(Target* target, const char* dir){
    append(&target->linked_search_directories, dir);
}

void add_include_search_directory(Target* target, const char* dir){
    append(&target->include_search_directories, dir);
}

void define_macro(Target* target, const char* macro, const char* value){
    append(&target->macros, {macro, value});
}

void compile(Target target){
    let builder = String_Builder_create(target.allocator);

    const char* link_file_flag;
    const char* output_flag;
    const char* link_search_directory_flag;
    const char* include_search_directory_flag;
    const char* debug_flag;
    const char* all_warnings_flag;
    const char* macro_definition_flag;

    switch (target.compiler) {
    case Compiler::GCC:{
        add_c_str(&builder, "g++ ");
        link_file_flag = "-l";
        output_flag = "-o";
        link_search_directory_flag = "-L";
        include_search_directory_flag = "-I";
        debug_flag = "-g";
        all_warnings_flag = "-Wall";
        macro_definition_flag = "-D";
    }break;
    }

    For_Each(iter(target.macros)){
        add_c_str(&builder, macro_definition_flag);
        add_c_str(&builder, " ");
        let [macro, value] = it.value;
        add_c_str(&builder, macro);
        add_c_str(&builder, "=");
        add_c_str(&builder, value);
        add_c_str(&builder, " ");
    }

    For_Each(iter(target.include_search_directories)){
        add_c_str(&builder, include_search_directory_flag);
        add_c_str(&builder, " ");
        add_c_str(&builder, it.value);
        add_c_str(&builder, " ");
    }
     
    For_Each(iter(target.args)){
        add_c_str(&builder, it.value);
        add_c_str(&builder, " ");
    }

    add_c_str(&builder, output_flag);
    add_c_str(&builder, " ");
    add_c_str(&builder, target.output);
    add_c_str(&builder, " ");

    For_Each(iter(target.linked_search_directories)){
        add_c_str(&builder, link_search_directory_flag);
        add_c_str(&builder, " ");
        add_c_str(&builder, it.value);
        add_c_str(&builder, " ");
    }

    For_Each(iter(target.linked_files)){
        add_c_str(&builder, link_file_flag);
        add_c_str(&builder, " ");
        add_c_str(&builder, it.value);
        add_c_str(&builder, " ");
    }

    if (target.all_warnings){
        add_c_str(&builder, all_warnings_flag);
        add_c_str(&builder, " ");
    }

    if (target.debug){
        add_c_str(&builder, debug_flag);
        add_c_str(&builder, " ");
    }



    let command = build(builder, target.allocator);
    defer(destroy(&command));
    let c_str = c_str_unsafe(command, target.allocator);
    defer(free_array(target.allocator, c_str, command.size + 1));

    printf("[Running]: %s\n", c_str);
    system(c_str);
}

