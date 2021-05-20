#include <iostream>
#include "../MadLibrary/Graph.hpp"

class Path
{
public:
    Path(uint32_t length)
    {
        this->length = length;
    }
    Path()
    {
        this->length = 0;
    }
    Path operator+(const Path P)
    {
        return Path(this->length + P.length);
    }
    bool operator>(const Path P)
    {
        return this->length > P.length;
    }
    uint32_t length;
};

int main()
{
    MadLibrary::UniqueGraph<std::string, std::string, Path> Location;

    Location.AddVertex("A1", "a1");
    Location.AddVertex("A2", "a2");
    Location.AddVertex("A3", "a3");
    Location.AddVertex("A4", "a4");

    Location.AddBidirectionalEdge("A1", "A2", Path(10));
    Location.AddBidirectionalEdge("A3", "A2", Path(12));
    Location.AddBidirectionalEdge("A4", "A2", Path(5));
    Location.AddBidirectionalEdge("A1", "A4", Path(13));

    std::map<std::string, Path> Distance;
    std::map<std::string, std::string> Previous;

    auto CompAlg = [](std::pair<std::string, Path> P1, std::pair<std::string, Path> P2) -> bool {
        return P1.second.length > P2.second.length;
    };

    Location.Dijkstra("A1", Distance, Previous, CompAlg);

    std::cout << Previous["A3"];
}