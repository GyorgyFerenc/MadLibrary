#include "../Includes/MadLibrary.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>



int main(){
    MadLibrary::Matrix<uint32_t> a(3,3,4);
    a=MadLibrary::SimplePow(a,3);
    std::cout<<a[0][0];
}