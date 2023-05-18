#pragma once

#include <filesystem>
#include <fstream>
#include <tuple>
#include <vector>

#include "Core.hpp"
#include "Process.hpp"

struct Target {
    // TODO Do it better lol

    std::vector<std::string> m_sources;
    std::vector<std::string> m_flag;
    std::vector<std::string> m_search_folder;
    std::vector<std::string> m_libraries;
    std::string              m_output;

    static Target from(std::initializer_list<std::string> list) {
        Target t;
        t.add_source(list);
        return t;
    }

    void add_source(std::initializer_list<std::string> list) {
        for (auto&& source : list) {
            add_source(source);
        }
    }

    void add_source(std::string source) {
        m_sources.push_back(source);
    }

    void add_source_folder(std::string path) {
        namespace fs = std::filesystem;
        auto files = fs::directory_iterator{path};
        for (auto&& file : files) {
            auto file_string = file.path().string();
            auto regular_file = file.is_regular_file();
            auto ends_with_cpp = ends_with(file_string, ".cpp");
            auto ends_with_c = ends_with(file_string, ".c");
            if (regular_file && ends_with_cpp && ends_with_c) add_source(file_string);
        }
    }

    void add_flag(std::string flag) {
        m_flag.push_back(flag);
    }

    void add_search_folder(std::string path) {
        m_search_folder.push_back(path);
    }

    void set_output(std::string output) {
        m_output = output;
    }

    void add_library(std::string library) {
        m_libraries.push_back(library);
    }

    void build() {
        std::string cmd = "g++ ";
        for (auto&& source : m_sources) {
            cmd += " ";
            cmd += source;
        }

        for (auto&& flag : m_flag) {
            cmd += " -";
            cmd += flag;
        }

        for (auto&& search : m_search_folder) {
            cmd += " -I";
            cmd += search;
        }

        for (auto&& library : m_libraries) {
            cmd += " -l";
            cmd += library;
        }

        cmd += " -o ";
        cmd += m_output;

        println_format("[NOTE] Building %", m_output);

        let p = Process::open(cmd.c_str()).unwrap();
        p.join();
        p.destroy();
    }

    bool ends_with(std::string text, std::string suffix) {
        if (text.size() < suffix.size()) return false;

        for (int i = 0; i < suffix.size(); i++) {
            let text_chr = text[text.size() - 1 - i];
            let suffix_chr = suffix[suffix.size() - 1 - i];
            if (suffix_chr != text_chr) return false;
        }
        return true;
    }
};

void self_rebuild(std::string source, std::string output) {
    {
        std::ifstream     source_in{source};
        std::stringstream buffer;
        buffer << source_in.rdbuf();
        let new_hash = std::hash<std::string>{}(buffer.str());

        std::size_t   saved_hash;
        std::ifstream in_build_hash{"build.hash"};

        if (in_build_hash.good())
            in_build_hash >> saved_hash;
        else
            saved_hash = 0;

        if (new_hash == saved_hash) return;

        std::ofstream out_build_hash{"build.hash"};
        out_build_hash << new_hash;
    }
    println("[NOTE] Rebuilding self");

    let builder = Target{};
    builder.add_source(source);
    builder.set_output(output);
    builder.build();

    let cmd = "./" + output;

    println("[NOTE] Run new build");

    let p = Process::open(cmd.c_str()).unwrap();

    char a[100];
    memset(a, 0, 100);
    int read_bytes = 0;
    while ((read_bytes = read(p.from_child, a, 4)) > 0) {
        print(a);
        memset(a, 0, read_bytes);
    }

    p.join();
    p.destroy();

    exit(0);
}

#define replace_me(name)

// void replace_me_in_file(std::string file, std::string name, std::string code) {
//     std::ifstream     in{file};
//     std::stringstream buffer;
//     buffer << source_in.rdbuf();
// }