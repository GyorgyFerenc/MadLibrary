#include <iostream>
#include <thread>

#include "../../include/Format.hpp"
#include "State.hpp"

using MadLibrary::println;

proc clear_screen() {
    system("clear");
}

proc print_menu() {
    println("Choose an option");
    println("1. Shows todos");
    println("2. Add todo");
    println("3. Remove todo");
    println("4. Uppercase words");
    println("0. Exit");
}

proc read_character()->char {
    char chr;
    std::cin >> chr;
    std::cin.ignore();

    return chr;
}

proc get_option()->char {
    return read_character();
}

proc read_line()->std::string {
    var ret = std::string{};
    while (true) {
        let chr = std::cin.get();

        if (chr == '\n' || std::cin.eof()) break;

        ret.push_back(chr);
    }

    return ret;
}

proc exec_option(char option) {
    switch (option) {
        case '1': {
            let& todos = State::todos;
            for (size_t i = 0; i < todos.size(); i++) {
                let todo = todos[i];
                println("%: %", i, todo);
            }

            break;
        }
        case '2': {
            println("Write a todo to add:");
            let todo = read_line();

            State::add_todo(todo);
            break;
        }

        case '3': {
            println("Give me an index:");
            var index = std::size_t{};
            std::cin >> index;
            std::cin.ignore();

            State::remove_todo(index);
        } break;
        case '4':
            State::uppercase_all_word();
            break;

        case '0':
            exit(0);
            break;

        default:
            println("Option doesnt exist");
            break;
    }
}

proc start_menu() {
    let running = true;
    while (running) {
        print_menu();
        let option = get_option();
        exec_option(option);
    }
}