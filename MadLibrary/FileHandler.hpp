#ifndef FileHandler__HPP
#define FileHandler__HPP

#include <fstream>
#include <string>

namespace MadLibrary{
    ///INIT
    bool CheckForFileExists(const std::string& name);

    ///Exposition
    bool CheckForFileExists(const std::string& name){
        std::ifstream f(name.c_str());
        return f.good();
    }

}
#endif