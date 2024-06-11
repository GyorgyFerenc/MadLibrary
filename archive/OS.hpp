#pragma once

#if MADLIBRARY_OS == MADLIBRARY_LINUX
    #include "OS/Linux.hpp" // IWYU pragma: export
#endif

proc read_all_filepath_c_str(const char* filepath, Allocator allocator) -> Option<Array<u8>>{
    let try_file = file_open_c_str(filepath);
    return_none(try_file);

    let file = try_file.value;
    defer(file_close(file));

    return {true, file_read_all(file, allocator)};
}

proc read_all_filepath(String filepath, Allocator allocator) -> Option<Array<u8>>{
    let try_file = file_open(filepath, allocator);
    return_none(try_file);

    let file = try_file.value;
    defer(file_close(file));

    return {true, file_read_all(file, allocator)};
}
