#ifndef __SimpleFunctions_HPP_INCLUDED__
#define __SimpleFunctions_HPP_INCLUDED__

#include "MadLibrary.hpp"

//toCleverString
template <typename DataType>
std::string MadLibrary::toCleverString(DataType& data){
    std::stringstream SStream;
    SStream<<data;
    std::string theString=SStream.str();
    return theString;
}

std::string MadLibrary::toCleverString(const std::string& data){
    std::stringstream SStream;
    SStream<<"\""<<data<<"\"";
    std::string theString=SStream.str();
    return theString;
}

std::string MadLibrary::toCleverString(char data){
    std::stringstream SStream;
    SStream<<"\'"<<data<<"\'";
    std::string theString=SStream.str();
    return theString;
}

std::string MadLibrary::toCleverString(char& data){
    std::stringstream SStream;
    SStream<<"\'"<<data<<"\'";
    std::string theString=SStream.str();
    return theString;
}

std::string MadLibrary::toCleverString(const char* data){
    std::stringstream SStream;
    SStream<<"\""<<data<<"\"";
    std::string theString=SStream.str();
    return theString;
}

std::string MadLibrary::toCleverString(char* data){
    std::stringstream SStream;
    SStream<<"\""<<data<<"\"";
    std::string theString=SStream.str();
    return theString;
}

template <typename DataType1,typename DataType2>
std::string MadLibrary::toCleverString(std::pair<DataType1,DataType2>& thePair){
    std::stringstream SStream;
    SStream<<MadLibrary::toCleverString(thePair.first)<<":"<<MadLibrary::toCleverString(thePair.second);
    std::string theString=SStream.str();
    return theString;
}

template <typename DataType1,typename DataType2>
std::string MadLibrary::toCleverString(std::map<DataType1,DataType2>& theMap){
    std::stringstream SStream;
    SStream<<"("<<MadLibrary::toCleverString(*(theMap.begin()));
    for (auto it=++theMap.begin();it!=theMap.end();it++){
        SStream<<","<<MadLibrary::toCleverString(*it);
    }
    SStream<<")";
    std::string theString=SStream.str();
    return theString;
}

template <typename DataType>
std::string MadLibrary::toCleverString(DataType& data,uint32_t flag){
    std::stringstream SStream;
    std::bitset<sizeof(data)*8> b(data);
    switch (flag)
    {
    case 1:
        SStream<<"0x"<<std::hex<<data;
        break;
    case 2:
        SStream<<"0b"<<b.to_string();
        break;
    case 3:
        SStream<<"Octx"<<std::oct<<data;
        break;
    default:
        SStream<<MadLibrary::toCleverString(data);
        break;
    }
    std::string theString=SStream.str();
    return theString;
}

template <typename DataType>
std::string MadLibrary::toCleverString(std::vector<DataType>& vect){
    std::stringstream SStream;
    SStream<<"(";
    for (uint32_t i=0;i<vect.size()-1;i++){
        SStream<<MadLibrary::toCleverString(vect[i])<<",";
    }
    SStream<<MadLibrary::toCleverString(vect[vect.size()-1]);
    SStream<<")";
    std::string theString=SStream.str();
    return theString;
}

template <typename DataType>
std::string MadLibrary::toCleverString(std::initializer_list<DataType>& initList){
    std::stringstream SStream;
        auto iTinitList=initList.begin();
        SStream<<"("<<MadLibrary::toCleverString(*iTinitList);
        for (iTinitList=++iTinitList;iTinitList!=initList.end();++iTinitList){
            SStream<<","<<MadLibrary::toCleverString(*iTinitList);
        }
        SStream<<")";
        std::string theString=SStream.str();
        return theString;
}

#if ListIncluded
    template <typename DataType>
    std::string MadLibrary::toCleverString(std::list<DataType>& TheList){
        std::stringstream SStream;
        auto itList=TheList.begin();
        SStream<<"(";
        for (uint32_t i=0;i<TheList.size()-1;i++){
            SStream<<MadLibrary::toCleverString(*itList)<<",";
            itList++;
        }
        SStream<<MadLibrary::toCleverString(*itList);
        SStream<<")";
        std::string theString=SStream.str();
        return theString;
    }
#endif

#if ArrayIncluded
    template <typename DataType, std::size_t Size>
    std::string MadLibrary::toCleverString(std::array<DataType,Size>& TheArray){
        std::stringstream SStream;
        SStream<<"(";
        for (uint32_t i=0;i<TheArray.size()-1;i++){
            SStream<<MadLibrary::toCleverString(TheArray[i])<<",";
        }
        SStream<<MadLibrary::toCleverString(TheArray[TheArray.size()-1]);
        SStream<<")";
        std::string theString=SStream.str();
        return theString;
    }
#endif

#if ForwardListIncluded
    template <typename DataType>
    std::string MadLibrary::toCleverString(std::forward_list<DataType>& TheForwardList){
        std::stringstream SStream;
        auto itForwardList=TheForwardList.begin();
        SStream<<"("<<MadLibrary::toCleverString(*itForwardList);
        for (itForwardList=++TheForwardList.begin();itForwardList!=TheForwardList.end();itForwardList++){
            SStream<<","<<MadLibrary::toCleverString(*itForwardList);
        }
        SStream<<")";
        std::string theString=SStream.str();
        return theString;
    }
#endif

//Map
double MadLibrary::Map(double value, double start1, double stop1, double start2, double stop2) {
    double outgoing = start2 + (stop2 - start2) * ((value - start1) / (stop1 - start1));
    return outgoing;
}

//Dijkstra
template <class DataType>
uint32_t ExtractMin(std::vector<uint32_t> theList, std::vector<DataType>& distance){
    uint32_t MinDist=distance[theList[0]],TheNode=theList[0];
    for (size_t i=1;i<theList.size();i++){
        if (distance[theList[i]]<MinDist){
            MinDist=distance[theList[i]];
            TheNode=theList[i];
        }
    }
    return TheNode;
}

template <class DataType>
std::vector<DataType> TheNeighbors(std::vector<std::vector<DataType>> &Graph,uint32_t node){
    std::vector<DataType> TheNeighbors;
    for (size_t i=0;i<Graph.size();i++){
        if (Graph[node][i]!=0)
        {
            TheNeighbors.push_back(i);
        }
    }
    return TheNeighbors;
}

template <class DataType>
void MadLibrary::Dijkstra(std::vector<std::vector<DataType>> &Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<DataType>& distance){
    uint32_t GraphSize=Graph.size();
    previous.clear();
    distance.clear();
    for (size_t i=0; i<GraphSize;i++){
        previous.push_back(std::numeric_limits<uint32_t>::max());
        distance.push_back(std::numeric_limits<DataType>::max());
    }
    distance[source]=0;
    std::vector<uint32_t> theList;
    for (size_t i=0;i<GraphSize;i++){
        theList.push_back(i);
    }
    while (!theList.empty())
    {
        uint32_t temp=ExtractMin(theList,distance);
        std::vector<DataType> Neighbors=TheNeighbors(Graph,temp);
        for (size_t i=0;i<Neighbors.size();i++){
            DataType alt=distance[temp]+Graph[temp][Neighbors[i]];
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

template <class DataType>
void MadLibrary::Dijkstra(MadLibrary::Matrix<DataType> &Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<DataType>& distance){
    Dijkstra((std::vector<std::vector<DataType>>)Graph,source,previous,distance);
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
    if (exp==0) return (Number/Number);
    if (exp==1) return Number;
    DataType temp=Number;
    uint32_t MomentExp=1;
    std::vector<DataType> Numbers;//0->2^1, 1->2^2
    std::vector<uint32_t> Exponents;//0->1,1->2
    while (MomentExp*2<=exp)
    {
        Number*=Number;
        MomentExp*=2;
        Exponents.push_back(MomentExp);
        Numbers.push_back(Number);
        if (MomentExp>=std::numeric_limits<uint32_t>::max()/2){
            Numbers.pop_back();
            Exponents.pop_back();
            break;
        }
    }
    for (uint32_t i=0;i<Numbers.size();i++){
        if (MomentExp+Exponents[Numbers.size()-i-1]<exp){
            Number*=Numbers[Numbers.size()-i-1];
            MomentExp+=Exponents[Numbers.size()-i-1];
        }
    }
    while (MomentExp<exp)
    {
        Number*=temp;
        MomentExp++;
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
template<class Iter, class Comper>
void MadLibrary::MergeSort(Iter beg, Iter end, Comper comp)
{
    int size = std::distance(beg, end);
    if (size <= 1) {
        return;
    }

    auto mid = std::next(beg, size / 2);
    MadLibrary::MergeSort(beg, mid, comp);
    MadLibrary::MergeSort(mid, end, comp);
    std::inplace_merge(beg, mid, end, comp);
}

template<class Iter>
void MadLibrary::MergeSort(Iter beg, Iter end)
{
    int size = std::distance(beg, end);
    if (size <= 1) {
        return;
    }

    auto mid = std::next(beg, size / 2);
    MadLibrary::MergeSort(beg, mid);
    MadLibrary::MergeSort(mid, end);
    std::inplace_merge(beg, mid, end);
}

#endif