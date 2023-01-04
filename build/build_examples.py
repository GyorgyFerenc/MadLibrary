import sys
import subprocess
import os

PATH = "../example/"


def get_cpp(dir: str) -> tuple[list[str], list[str]]:
    cpp_list = []
    dir_list = []
    # print(cpp)
    for cpp in os.listdir(dir):
        if os.path.isdir(dir + "/" + cpp):
            dir_list.append(cpp)
            continue

        if cpp.endswith(".cpp"):
            cpp_list.append(cpp)

    return (cpp_list, dir_list)


def create_dir(dir: str):
    if not os.path.exists(dir):
        os.makedirs(dir)


def build_example(cpp_file: str, out_file: str):
    build_line = f"g++ -Wall -std=c++20 {cpp_file} -o {out_file}"
    subprocess.run(build_line, shell=True)


def build_directory(dir: str):
    create_dir(dir)

    path_to_dir = PATH + dir
    cpp_files, dir_list = get_cpp(path_to_dir)

    for d in dir_list:
        build_directory(dir+"/"+d)

    for cpp in cpp_files:
        path_cpp = path_to_dir + "/" + cpp
        path_out = dir + "/" + cpp.removesuffix(".cpp") + ".out"
        print(f"Building {path_cpp}")
        build_example(path_cpp, path_out)


def build_all():
    dir_list = os.listdir(PATH)
    for dir in dir_list:
        build_directory(dir)


def build_one(argv: list[str]):
    for i in range(1, len(argv)):
        path = argv[i]
        print(f"Building {path}")
        build_from_path(path)


def build_from_path(path: str):
    splited_path = path.split("/")
    dir_path = ""

    for i in range(len(splited_path)-1):
        dir_path += splited_path[i] + "/"

    cpp_file = splited_path[-1]

    create_dir(dir_path)

    cpp = PATH + dir_path + cpp_file
    out = dir_path + cpp_file.removesuffix(".cpp") + ".out"

    build_example(cpp, out)


if __name__ == "__main__":
    if len(sys.argv) <= 1:
        build_all()
    else:
        build_one(sys.argv)
