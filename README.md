# MadLibrary
 Provide usefull resources, codes etc.
 
[Installation](#Installation)
## Installation
 It provides and easy installation.
 Download the Includes folder with the files and put in your project folder, then in your project include the MadLibrary.hpp.
```cpp
#include "Includes/MadLibrary.hpp"
```
 The you ready to go.
## Documentation
 ____Dijsktra____
 
```cpp
void Dijkstra(std::vector<std::vector<uint32_t>> Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<uint32_t>& distance);

void Dijkstra(MadLibrary::Matrix<uint32_t> Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<uint32_t>& distance);
```
It is a Dijkstra pathfinding algorithm to find the shortest path from the source point to the all other in a graph.
See: [Dijkstra](https://en.wikipedia.org/wiki/Dijkstra%27s_algorithm)
__Parameters__:
- std::vector<std::vector<uint32_t>> Graph: A 2d uint32_t Adjacency matrix using the std::vector. See: [Adjacency_matrix](https://en.wikipedia.org/wiki/Adjacency_matrix)
- MadLibrary::Matrix<uint32_t> Graph: A 2d uint32_t Adjacency matrix using the MadLibrary::Matrix. See: [Adjacency_matrix](https://en.wikipedia.org/wiki/Adjacency_matrix)
- uint32_t source: The source node, from where the paths are measured.
- std::vector<uint32_t>& previous: The std::vector where is the previous nodes are stored.
- std::vector<uint32_t>& distance: The std::vector where is the distance from the source are stored.

__Return Value__:
    There is no return value.
