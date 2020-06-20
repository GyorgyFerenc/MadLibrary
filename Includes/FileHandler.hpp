#include "MadLibrary.hpp"

bool MadLibrary::CheckForFileExists(const std::string& name){
    std::ifstream f(name.c_str());
    return f.good();
}