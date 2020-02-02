#include "../Includes/MadLibrary.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>



int main(){
    MadLibrary::Matrix<int> a(20,20,4);
    std::cout<<a.GetDeterminant()<<std::endl;
}