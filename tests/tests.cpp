#include "../Includes/MadLibrary.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

int main(){
    MadLibrary::Graph<uint32_t,uint32_t> G;
    G.AddVertex(0);
    G.AddVertex(0);
    G.AddEdge(0,1,124);
    MadLibrary::Edge<uint32_t> Ed(0,1,1);
    std::pair<uint32_t,uint32_t> a=std::make_pair<uint32_t,uint32_t>(0,1);
    std::cout<<G.GetEdgeData(1,1);
}