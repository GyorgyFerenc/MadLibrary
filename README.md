# MadLibrary
 Provide usefull resources, codes etc.
 
[Installation](#Installation) 

[Documentation](#Documentation)
# Installation
 It provides and easy installation.
 Download the Includes folder with the files and put in your project folder, then in your project include the MadLibrary.hpp.
```cpp
#include "Includes/MadLibrary.hpp"
```
 The you ready to go.
# Documentation
## Dijsktra
 
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
    
## Abs
```cpp
template<class DataType>
DataType Abs(DataType data);
```
It is function returning the absolute value of any data.
__Parameters__:
- DataType data: the data.

__Return Value__:
    Absolute value of any data.
    
## PolygonArea
```cpp
template<class DataType,class DataType2>
void PolygonArea(std::vector<DataType> X, std::vector<DataType> Y,DataType2 &temp);
```
It calculates the area of a given polygon that has no holes in it.
See: [Shoelace Formula](https://en.wikipedia.org/wiki/Shoelace_formula)
__Parameters__:
- std::vector<DataType> X: The st::vector of X coordinates of the edges.
- std::vector<DataType> Y: The st::vector of Y coordinates of the edges.
- DataType2 &temp: variable where is the area is stored

__Return Value__:
    There is no return value.

    
## MergeSort
```cpp
template<typename Iter>
void MergeSort(Iter beg, Iter end);
```
Sorting the element of a container.
See: [Merge Sort](https://en.wikipedia.org/wiki/Merge_sort)
Note: This code was written by [Zach Whaley](https://github.com/zachwhaley).
__Parameters__:
- Iter beg: Iterator to the begining of the container.
- Iter end: Iterator to the end of the container.

__Return Value__:
    There is no return value.

## StringToNumber
```cpp
template <class DataType,class RandomAccesIterator>
void StringToNumber(RandomAccesIterator begin,RandomAccesIterator end,DataType& toData);
```
Extract the data out of the string.
__Parameters__:
- RandomAccesIterator begin: RandomAccesIterator to the begining of the data.
- RandomAccesIterator end: RandomAccesIterator to the end of the data.
- DataType& toData: The variable to store the data.

__Return Value__:
    There is no return value.

## NumberSize
```cpp
template <class DataType>
uint32_t NumberSize(DataType Number);
```
Return the size of the number.
__Parameters__:
- DataType Number: The number.

__Return Value__:
    Return the size of the number.
    
## SimplePow
```cpp
template <class DataType>
DataType SimplePow(DataType Number,uint32_t exp);
```
Rraises the number to a positive whole number exponent.
__Parameters__:
- DataType Number: The number.
- uint32_t exp: The exponent.

__Return Value__:
    The new number.
    
## PrimeCheck
```cpp
template <class DataType>
bool PrimeCheck(DataType prime);
```
Check if the number is a prime.
__Parameters__:
- DataType prime: The number to check.

__Return Value__:
    Returns true if it's a prime, otherwise false.

## DeleteDigit
```cpp
template <class DataType>
DataType DeleteDigit(DataType Number,uint32_t pos);
```
Delete a digit from the number.
__Parameters__:
- DataType Number: The number to delete from.
- uint32_t pos: The position from to delete, zero indexed.

__Return Value__:
    Returns the new number.

