#include "../Includes/MadLibrary.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

bool m(const int& a,const int& b){
    return a>b;
}

int main(){
    std::vector<int> a={2,-125,10,5,1,5621,2,51,6,512,-125,1,1,562,124,21,-251,2};
    MadLibrary::MergeSort(a.begin(),a.end(),m);
    for (uint32_t i=0;i<a.size();i++){
        std::cout<<a[i]<<" ";
    }
}