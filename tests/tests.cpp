#include <iostream>
#include <map>
#include "../Includes/MadLibrary.hpp"

int main(){
    std::vector<std::string> Parsed = MadLibrary::StringParser("alm a");
    std::cout<<MadLibrary::toCleverString(Parsed);
}
/*
    MadLibrary::Graph<uint32_t,uint32_t> G(9);

    G.AddEdge(0, 1, 4); 
    G.AddEdge(0, 7, 8); 
    G.AddEdge(1, 2, 8); 
    G.AddEdge(1, 7, 11); 
    G.AddEdge(2, 3, 7); 
    G.AddEdge(2, 8, 2); 
    G.AddEdge(2, 5, 4); 
    G.AddEdge(3, 4, 9); 
    G.AddEdge(3, 5, 14); 
    G.AddEdge(4, 5, 10); 
    G.AddEdge(5, 6, 2); 
    G.AddEdge(6, 7, 1); 
    G.AddEdge(6, 8, 6); 
    G.AddEdge(7, 8, 7);


    0, 1, 0, 0, 0, 0, 0, 1, 0, 
    0, 0, 1, 0, 0, 0, 0, 1, 0, 
    0, 0, 0, 1, 0, 1, 0, 0, 1, 
    0, 0, 0, 0, 1, 1, 0, 0, 0, 
    0, 0, 0, 0, 0, 1, 0, 0, 0, 
    0, 0, 0, 0, 0, 0, 1, 0, 0, 
    0, 0, 0, 0, 0, 0, 0, 1, 1, 
    0, 0, 0, 0, 0, 0, 0, 0, 1, 
    0, 0, 0, 0, 0, 0, 0, 0, 0, 

*/