#pragma once

#include "Core.hpp"
#include "OS.hpp"
#include <cstdio>
#include <cstdlib>


proc run(const char* c_str) -> int{
    printf("[Running]: %s\n", c_str);
    return system(c_str);
}

enum struct Compiler{
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

void target_define_macro(Target* target, const char* macro, const char* value = "1"){
    dynamic_array_append(&target->macros, {macro, value});
}

int target_compile(Target target){
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
    let command_c_str = string_c_str_unsafe(command, target.allocator);
    defer(allocator_free_array(target.allocator, command_c_str, command.size + 1));

    return run(command_c_str);
}


proc self_build(const char* source_path, 
                const char* output, 
                Allocator allocator,
                int argc,
                const char** argv){

    let source_file = file_open_c_str(source_path).unwrap();
    let source = cast(String) file_read_all(source_file, allocator);
    let source_hash = string_hash(source);

    let builder = string_builder_create(allocator);
    defer(string_builder_destroy(&builder));
    string_builder_add_c_str(&builder, source_path);
    string_builder_add_c_str(&builder, ".hash");

    let hash_file_path = string_builder_build_alias(builder);
    let [success, hash_file_reader] = file_open(hash_file_path, allocator, File::Mode::Read);

    let need_rebuild = false;

    if (!success){
        need_rebuild = true;
    } else {
        let stored_hash_bytes = file_read_all(hash_file_reader, allocator);
        file_close(hash_file_reader);

        let stored_hash = *cast(usize*)stored_hash_bytes.ptr;// transmute out the 
        need_rebuild = stored_hash != source_hash;
    }

    if (need_rebuild){
        // save the hash
        let hash_file_write = file_open(hash_file_path, allocator, File::Mode::Write).unwrap();
        file_write(hash_file_write, bytes_of(&source_hash));
        file_close(hash_file_write);

        let target = target_create(allocator);
        defer(target_destory(&target));

        target_add_source(&target, source_path);
        target.output = output;
        target_compile(target);

        let rerun_buldier = builder; // reuse the builder for saving memory
        string_builder_clear(&rerun_buldier);

        For_Each(range(0, argc)){
            string_builder_add_c_str(&rerun_buldier, argv[it.value]);
            string_builder_add_char(&rerun_buldier, ' ');
        }

        let rerun_cmd = string_builder_build_alias(rerun_buldier);
        let rerun_cmd_c_str = string_c_str(rerun_cmd, allocator).unwrap();
        defer(allocator_free_array(allocator, rerun_cmd_c_str, rerun_cmd.size + 1));

        run(rerun_cmd_c_str);
        exit(1);
    }
}

