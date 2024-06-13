#pragma once

#include "core.hpp"
#include "fmt.hpp"
#include "string.hpp"
#include "os.hpp"
#include <cstdlib>

struct Command{
    String_Builder builder;
    String         name;
};

inline
Command create_command(Allocator allocator, String name = alias("CMD")){
    return { 
        .builder = create_string_builder(allocator),
        .name    = name,
    };
}

inline
void destroy(Command cmd){
    destroy(cmd.builder);
}

inline
int run(Command cmd, Allocator allocator){
    let a    = alias(cmd.builder);
    let cstr = build_cstr(cmd.builder, allocator);
    println_fmt("[%] Running: %", cmd.name, cstr);
    return system(cstr);
}

inline
void add(Command* cmd, String str){
    add(&cmd->builder, str);
    add(&cmd->builder, " ");
}

inline
void add(Command* cmd, const char* cstr){
    add(&cmd->builder, cstr);
    add(&cmd->builder, " ");
}

inline
void clear(Command* cmd){
    clear(&cmd->builder);
}

void self_build(const char* source_path, 
                const char* output, 
                Allocator allocator,
                int argc,
                const char** argv){

    let [source_file, ok] = file_open(source_path);
    if (!ok) panic("Could not open source_path");
    let source = string_from(file_read_all(source_file, allocator));
    let builder = create_string_builder(allocator);
    defer(destroy(builder));
    add(&builder, source_path);
    add(&builder, ".stored");
    let stored_file_path = alias(builder);
    let [stored_file, success] = file_open(stored_file_path, allocator, File::Mode::Read);
    let need_rebuild = false;
    if (!success){
        need_rebuild = true;
    } else {
        let stored_source = string_from(file_read_all(stored_file, allocator));
        close(stored_file);
        need_rebuild = !equal(stored_source, source);
    }
    if (need_rebuild){
        println_fmt("Need rebuild");
        let [stored_file_write, ok] = file_open(stored_file_path, allocator, File::Mode::Write);
        if (!ok) panic("Could not open stored_file_write");
        write(stored_file_write, raw(source));
        close(stored_file_write);
        let builder = create_command(allocator, alias("Builder"));
        add(&builder, "g++");
        add(&builder, source_path);
        add(&builder, "-o");
        add(&builder, output);
        let correct = run(builder, allocator);
        if (correct != 0) panic("Could not rebuild builder");

        clear(&builder);
        For_Each(range(0, argc)){
            add(&builder, argv[it.value]);
        }

        run(builder, allocator);
        exit(1);
    }
}

