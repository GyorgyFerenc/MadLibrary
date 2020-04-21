#include "../Includes/MadLibrary.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>
#include <typeinfo>

using namespace MadLibrary;

template <class alma>
class ASD{
    public: 
        ASD(){
            std::cout<<typeid(alma).name();
        }
};

int main(){
    ASD<std::vector<Graph<int,int>>> a;
    
}