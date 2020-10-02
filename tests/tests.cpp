#include <iostream>
#include "../MadLibrary/Graph.hpp"
#include "../MadLibrary/SimpleFunctions.hpp"

int main()
{
    MadLibrary::UniqueGraph<int, int, int> a({0, 1, 2, 3});
    a.AddBidirectionalEdge(0, 1, 12);
    a.AddBidirectionalEdge(1, 2, 12);
    a.AddBidirectionalEdge(0, 2, 12);
    a.AddBidirectionalEdge(0, 3, 12);
    std::cout << MadLibrary::toCleverString(a.DepthFirstSearch(0));
}