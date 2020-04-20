#include "../Includes/MadLibrary.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

int main(){
    std::vector<char> a={'a','b'};
    std::cout<<MadLibrary::toString(a);
}