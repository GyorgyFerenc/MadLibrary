#include <iostream>
#include <map>
#include "../Includes/MadLibrary.hpp"

int main(){
    MadLibrary::Graph<uint32_t,uint32_t> G(9,{1,2,3,4,5,6,7,8,9});
    G.AddBidirectionalEdge(0, 1, 4); 
    G.AddBidirectionalEdge(0, 7, 8); 
    G.AddBidirectionalEdge(1, 2, 8); 
    G.AddBidirectionalEdge(1, 7, 11); 
    G.AddBidirectionalEdge(2, 3, 7); 
    G.AddBidirectionalEdge(2, 8, 2); 
    G.AddBidirectionalEdge(2, 5, 4); 
    G.AddBidirectionalEdge(3, 4, 9); 
    G.AddBidirectionalEdge(3, 5, 14); 
    G.AddBidirectionalEdge(4, 5, 10); 
    G.AddBidirectionalEdge(5, 6, 2); 
    G.AddBidirectionalEdge(6, 7, 1); 
    G.AddBidirectionalEdge(6, 8, 6); 
    G.AddBidirectionalEdge(7, 8, 7);

    auto asd = G.PrimSpanningTreeGraph();
    std::cout<<MadLibrary::toCleverString(asd.DepthFirstSearch(0));
    std::cout <<"asd";
    /*
    std::vector<int> asd;
    asd.push_back(1);
    asd.push_back(15);
    asd.push_back(123);
    asd.push_back(0);
    asd.push_back(-12);

    auto CompAlg= [&asd](int First, int Second)->bool{
        return asd[First]>asd[Second];
    };

    std::priority_queue<int, std::vector<int>,decltype(CompAlg)> PriorityQueue(CompAlg);

    PriorityQueue.push(0);
    PriorityQueue.push(1);
    PriorityQueue.push(2);
    PriorityQueue.push(3);
    PriorityQueue.push(4);

    while (!PriorityQueue.empty())
    {
        std::cout<<PriorityQueue.top()<<" ";
        PriorityQueue.pop();
    }
    */

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