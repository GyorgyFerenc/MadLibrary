#pragma once

#include "Core.hpp"
#include <cstdio>
#include <cstdlib>


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


Target target_create(Allocator allocator){
    return {
        .allocator                  = allocator,
        .args                       = dynamic_array_create<const char*>(allocator),
        .linked_files               = dynamic_array_create<const char*>(allocator),
        .linked_search_directories  = dynamic_array_create<const char*>(allocator),
        .include_search_directories = dynamic_array_create<const char*>(allocator),
        .macros                     = dynamic_array_create<Pair<const char*, const char*>>(allocator),
    };
}

void target_destory(Target* target){
    dynamic_array_destroy(&target->args);
    dynamic_array_destroy(&target->linked_files);
    dynamic_array_destroy(&target->linked_search_directories);
    dynamic_array_destroy(&target->include_search_directories);
    dynamic_array_destroy(&target->macros);
}

void target_add_arg(Target* target, const char* arg){
    dynamic_array_append(&target->args, arg);
}

void target_add_source(Target* target, const char* source){
    target_add_arg(target, source);
}

void target_link_file(Target* target, const char* file){
    dynamic_array_append(&target->linked_files, file);
}

void target_add_link_search_directory(Target* target, const char* dir){
    dynamic_array_append(&target->linked_search_directories, dir);
}

void target_add_include_search_directory(Target* target, const char* dir){
    dynamic_array_append(&target->include_search_directories, dir);
}

void target_define_macro(Target* target, const char* macro, const char* value){
    dynamic_array_append(&target->macros, {macro, value});
}

void target_compile(Target target){
    let builder = string_builder_create(target.allocator);

    const char* link_file_flag;
    const char* output_flag;
    const char* link_search_directory_flag;
    const char* include_search_directory_flag;
    const char* debug_flag;
    const char* all_warnings_flag;
    const char* macro_definition_flag;

    switch (target.compiler) {
    case Compiler::GCC:{
        string_builder_add_c_str(&builder, "g++ ");
        link_file_flag = "-l";
        output_flag = "-o";
        link_search_directory_flag = "-L";
        include_search_directory_flag = "-I";
        debug_flag = "-g";
        all_warnings_flag = "-Wall";
        macro_definition_flag = "-D";
    }break;
    }

    For_Each(dynamic_array_iter(target.macros)){
        string_builder_add_c_str(&builder, macro_definition_flag);
        string_builder_add_c_str(&builder, " ");
        let [macro, value] = it.value;
        string_builder_add_c_str(&builder, macro);
        string_builder_add_c_str(&builder, "=");
        string_builder_add_c_str(&builder, value);
        string_builder_add_c_str(&builder, " ");
    }

    For_Each(dynamic_array_iter(target.include_search_directories)){
        string_builder_add_c_str(&builder, include_search_directory_flag);
        string_builder_add_c_str(&builder, " ");
        string_builder_add_c_str(&builder, it.value);
        string_builder_add_c_str(&builder, " ");
    }
     
    For_Each(dynamic_array_iter(target.args)){
        string_builder_add_c_str(&builder, it.value);
        string_builder_add_c_str(&builder, " ");
    }

    string_builder_add_c_str(&builder, output_flag);
    string_builder_add_c_str(&builder, " ");
    string_builder_add_c_str(&builder, target.output);
    string_builder_add_c_str(&builder, " ");

    For_Each(dynamic_array_iter(target.linked_search_directories)){
        string_builder_add_c_str(&builder, link_search_directory_flag);
        string_builder_add_c_str(&builder, " ");
        string_builder_add_c_str(&builder, it.value);
        string_builder_add_c_str(&builder, " ");
    }

    For_Each(dynamic_array_iter(target.linked_files)){
        string_builder_add_c_str(&builder, link_file_flag);
        string_builder_add_c_str(&builder, " ");
        string_builder_add_c_str(&builder, it.value);
        string_builder_add_c_str(&builder, " ");
    }

    if (target.all_warnings){
        string_builder_add_c_str(&builder, all_warnings_flag);
        string_builder_add_c_str(&builder, " ");
    }

    if (target.debug){
        string_builder_add_c_str(&builder, debug_flag);
        string_builder_add_c_str(&builder, " ");
    }



    let command = string_builder_build(builder, target.allocator);
    defer(array_destroy(&command));
    let c_str = string_c_str_unsafe(command, target.allocator);
    defer(allocator_free_array(target.allocator, c_str, command.size + 1));

    printf("[Running]: %s\n", c_str);
    system(c_str);
}

