#ifndef __Graph_HPP_INCLUDED__
#define __Graph_HPP_INCLUDED__

#include "MadLibrary.hpp"

//Vertex
//Constructor
template<typename VertexData>
MadLibrary::Vertex<VertexData>::Vertex(uint32_t SerialNumber,VertexData Data){
    this->SerialNumber=SerialNumber;
    this->Data=Data;
}

//SetSerialNumber
template<typename VertexData>
void MadLibrary::Vertex<VertexData>::SetSerialNumber(uint32_t SerialNumber){
    this->SerialNumber=SerialNumber;
}

//GetSerialNumber
template<typename VertexData>
uint32_t MadLibrary::Vertex<VertexData>::GetSerialNumber(){
    return this->SerialNumber;
}

//SetData
template<typename VertexData>
void MadLibrary::Vertex<VertexData>::SetData(VertexData Data){
    this->Data=Data;
}

//GetData
template<typename VertexData>
VertexData MadLibrary::Vertex<VertexData>::GetData(){
    return this->Data;
}

//Edge
//Constructor
template <typename EdgeData>
MadLibrary::Edge<EdgeData>::Edge(uint32_t VertexTo,uint32_t VertexFrom,EdgeData Data){
    this->VertexFrom=VertexFrom;
    this->VertexTo=VertexTo;
    this->Data=Data;
}
//GetEdge
template <typename EdgeData>
std::pair<uint32_t,uint32_t> MadLibrary::Edge<EdgeData>::GetEdge(){
    return std::make_pair(this->VertexFrom,this->VertexTo);
}

//SetEdge
template <typename EdgeData>
void MadLibrary::Edge<EdgeData>::SetEdge(uint32_t VertexFrom, uint32_t VertexTo){
    this->VertexFrom=VertexFrom;
    this->VertexTo=VertexTo;
}

template <typename EdgeData>
void MadLibrary::Edge<EdgeData>::SetEdge(std::pair<uint32_t,uint32_t> NewEdge){
    this->VertexFrom=NewEdge.first;
    this->VertexTo=NewEdge.second;
}

//GetVertexFrom
template <typename EdgeData>
uint32_t MadLibrary::Edge<EdgeData>::GetVertexFrom(){
    return this->VertexFrom;
}

//GetVertexTo
template <typename EdgeData>
uint32_t MadLibrary::Edge<EdgeData>::GetVertexTo(){
    return this->VertexTo;
}

//SetVertexFrom
template <typename EdgeData>
void MadLibrary::Edge<EdgeData>::SetVertexFrom(uint32_t VertexFrom){
    this->VertexFrom=VertexFrom;
}

//SetVertexTo
template <typename EdgeData>
void MadLibrary::Edge<EdgeData>::SetVertexTo(uint32_t VertexTo){
    this->VertexTo=VertexTo;
}

//SetEdgeData
template <typename EdgeData>
void MadLibrary::Edge<EdgeData>::SetEdgeData(EdgeData Data){
    this->Data=Data;
}

//GetEdgeData
template <typename EdgeData>
EdgeData MadLibrary::Edge<EdgeData>::GetEdgeData(){
    return this->Data;
}

//Graph


#endif