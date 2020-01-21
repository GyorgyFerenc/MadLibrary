#ifndef __SimpleFunctions_HPP_INCLUDED__
#define __SimpleFunctions_HPP_INCLUDED__

#include "MadLibrary.hpp"
#include <ctgmath>
#include <sstream>
#include <algorithm>

//Dijkstra
uint32_t ExtractMin(std::vector<uint32_t> theList, std::vector<uint32_t>& distance){
    uint32_t MinDist=distance[theList[0]],TheNode=theList[0];
    for (size_t i=1;i<theList.size();i++){
        if (distance[theList[i]]<MinDist){
            MinDist=distance[theList[i]];
            TheNode=theList[i];
        }
    }
    return TheNode;
}

std::vector<uint32_t> TheNeighbors(std::vector<std::vector<uint32_t>> Graph,uint32_t node){
    std::vector<uint32_t> TheNeighbors;
    for (size_t i=0;i<Graph.size();i++){
        if (Graph[node][i]!=0)
        {
            TheNeighbors.push_back(i);
        }
    }
    return TheNeighbors;
}

void MadLibrary::Dijkstra(std::vector<std::vector<uint32_t>> Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<uint32_t>& distance){
    uint32_t GraphSize=Graph.size();
    previous.clear();
    distance.clear();
    for (size_t i=0; i<GraphSize;i++){
        previous.push_back(-1);
        distance.push_back(-1);
    }
    distance[source]=0;
    std::vector<uint32_t> theList;
    for (size_t i=0;i<GraphSize;i++){
        theList.push_back(i);
    }
    while (!theList.empty())
    {
        uint32_t temp=ExtractMin(theList,distance);
        std::vector<uint32_t> Neighbors=TheNeighbors(Graph,temp);
        for (size_t i=0;i<Neighbors.size();i++){
            uint32_t alt=distance[temp]+Graph[temp][Neighbors[i]];
            if (alt<distance[Neighbors[i]]){
                distance[Neighbors[i]]=alt;
                previous[Neighbors[i]]=temp;
            }
        }
        uint32_t pos=0;
        for (size_t i=0;i<theList.size();i++){
            if (theList[i]==temp){
                pos=i;
                break;
            }
        }
        theList.erase(theList.begin()+pos);
    }
}

void MadLibrary::Dijkstra(MadLibrary::Matrix<uint32_t> Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<uint32_t>& distance){
    std::vector<std::vector<uint32_t>> temp;
    for (size_t i=0;i<Graph.GetRow();i++){
        temp.push_back(Graph[i]);
    }
    Dijkstra(temp,source,previous,distance);
}

//Abs
template <class DataType>
DataType MadLibrary::Abs(DataType data){
    if (data<0) return -1*data;
    return data;
}

//PolygonArea
template<class DataType,class DataType2>
void MadLibrary::PolygonArea(std::vector<DataType> X, std::vector<DataType> Y,DataType2 &temp) 
{ 
    // Initialze area 
    DataType2 area = 0.0; 
  
    // Calculate value of shoelace formula 
    int j = X.size() - 1;
    for (int i = 0; i < X.size(); i++) 
    { 
        area += (X[j] + X[i]) * (Y[j] - Y[i]); 
        j = i;  // j is previous vertex to i 
    } 
  
    // Return absolute value 
    temp=MadLibrary::Abs(area / 2.0); 
}

//StringToNumber
template <class DataType,class RandomAccesIterator>
void MadLibrary::StringToNumber(RandomAccesIterator begin,RandomAccesIterator end,DataType& toData){
    std::stringstream sS;
    for (auto i=begin; i != end;i++){
        sS<<*(i);
    }
    sS>>toData;
}

//NumberSize
template <class DataType>
uint32_t MadLibrary::NumberSize(DataType Number){
    unsigned int size=0;
    do{
        Number/=10;
        size++;
    }while (Number!=0);
    return size;
}

//SimplePow
template <class DataType>
DataType MadLibrary::SimplePow(DataType Number,uint32_t exp){
    if (exp==0) return 1;
    DataType temp=Number;
    for (int i=0;i<exp-1;i++){
        Number*=temp;
    }
    return Number;
}

//DeleteDigit
template <class DataType>
DataType MadLibrary::DeleteDigit(DataType Number,uint32_t pos){
    return Number/MadLibrary::SimplePow(10,MadLibrary::NumberSize(Number)-pos)*MadLibrary::SimplePow(10,MadLibrary::NumberSize(Number)-pos-1)+Number%MadLibrary::SimplePow(10,MadLibrary::NumberSize(Number)-pos-1);
}

//PrimeCheck
template <class DataType>
bool MadLibrary::PrimeCheck(DataType prime){
    if (prime<=1) return false;
    if (prime==2) return true;
    if (prime%2==0) return false;
    for (DataType i = 3; i <=sqrt(prime) ; i+=2)
    {
        if (prime%i==0) return false;
        
    }
    return true;
}

//MergeSort
template<typename Iter>
void merge(Iter beg, Iter mid, Iter end)
{
    std::vector<typename Iter::value_type> temp;
    temp.reserve(std::distance(beg, end));
    Iter left = beg;
    Iter right = mid;
    while (left != mid and right != end) {
        if (*right < *left) {
            temp.emplace_back(*right++);
        }
        else {
            temp.emplace_back(*left++);
        }
    }
    temp.insert(temp.end(), left, mid);
    temp.insert(temp.end(), right, end);

    std::move(temp.begin(), temp.end(), beg);
}

template<typename Iter>
void MadLibrary::MergeSort(Iter beg, Iter end) 
{
    int size = std::distance(beg, end);
    if (size <= 1) {
        return;
    }

    auto mid = std::next(beg, size / 2);
    // sort left half
    MadLibrary::MergeSort(beg, mid);
    // sort right half
    MadLibrary::MergeSort(mid, end);
    // merge halves
    merge(beg, mid, end);
}
#endif