#include "../Includes/MadLibrary.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>



int main(){
    MadLibrary::Matrix<int> a(2,3,4);
    MadLibrary::Matrix<int> b(3,4,5);
 
    a[0][0]=1;
    a[0][1]=2;
    a[0][2]=3;
    a[1][0]=4;
    a[1][1]=5;
    a[1][2]=6;
    
    b=a*b;
    std::cout<<b[0][2]<<std::endl;
}