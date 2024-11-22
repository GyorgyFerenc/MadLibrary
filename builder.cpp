#include "lib/build.hpp"
#include "lib/core.hpp"
#include "lib/fmt.hpp"

int main(int argc, char const* argv[]) {
    // Don't care about memory it is a script
    let alloc = mallocator;
    self_build("builder.cpp", "build", alloc, argc, argv);

    if (argc < 2){
        println_fmt("Need args lol");
        return 1;
    }

    if (strcmp(argv[1], "playground") == 0){
        let playground = create_command(alloc, alias("Playground"));
        add(&playground, "g++");
        add(&playground, "-o playground");
        add(&playground, "-D ML_DEBUG");
        add(&playground, "playground.cpp");
        add(&playground, "-g");

        let code = run(playground, alloc);
        if (code == 0) 
        {
            //println_fmt("[%] Running: %", "Playground", "./playground");
            //system("./playground");
        }
    } 
}
