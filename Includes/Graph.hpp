#ifndef __Graph_HPP_INCLUDED__
#define __Graph_HPP_INCLUDED__

#include "MadLibrary.hpp"


///Vertex
//Constructor
template<typename VertexData>
MadLibrary::Vertex<VertexData>::Vertex(uint32_t ID,VertexData Data){
    this->ID=ID;
    this->Data=Data;
}

//SetID
template<typename VertexData>
void MadLibrary::Vertex<VertexData>::SetID(uint32_t ID){
    this->ID=ID;
}

//GetID
template<typename VertexData>
uint32_t MadLibrary::Vertex<VertexData>::GetID(){
    return this->ID;
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

///Edge
//Constructor
template <typename EdgeData>
MadLibrary::Edge<EdgeData>::Edge(uint32_t VertexFrom,uint32_t VertexTo,EdgeData Data){
    this->VertexFrom=VertexFrom;
    this->VertexTo=VertexTo;
    this->Data=Data;
}
//GetEdge
template <typename EdgeData>
std::pair<uint32_t,uint32_t> MadLibrary::Edge<EdgeData>::GetEdge(){
    std::pair<uint32_t,uint32_t> a;
    a.first=this->VertexFrom;
    a.second=this->VertexTo;
    return a;
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
void MadLibrary::Edge<EdgeData>::SetData(EdgeData Data){
    this->Data=Data;
}

//GetEdgeData
template <typename EdgeData>
EdgeData MadLibrary::Edge<EdgeData>::GetData(){
    return this->Data;
}

///Graph
//AddEdge
template <typename VertexData,typename EdgeData>
void MadLibrary::Graph<VertexData,EdgeData>::AddEdge(uint32_t VertexFrom,uint32_t VertexTo,EdgeData Data){
    MadLibrary::Edge<EdgeData> newEdge(VertexFrom,VertexTo,Data);
    this->EdgeList.push_back(newEdge);
} 

//AddVertex
template <typename VertexData,typename EdgeData>
void MadLibrary::Graph<VertexData,EdgeData>::AddVertex(VertexData Data){
    MadLibrary::Vertex<VertexData> newVertex(this->IDCount++,Data);
    this->VertexList.push_back(newVertex);
}

//GetVertexData
template <typename VertexData,typename EdgeData>
VertexData MadLibrary::Graph<VertexData,EdgeData>::GetVertexData(uint32_t ID){
    for (auto Vertex:this->VertexList){
        if (Vertex.GetID()==ID) return Vertex.GetData();
    }
}

//GetEdgeData
template <typename VertexData,typename EdgeData>
EdgeData MadLibrary::Graph<VertexData,EdgeData>::GetEdgeData(std::pair<uint32_t,uint32_t> TheEdge){
    uint32_t VertexFrom=TheEdge.first,VertexTo=TheEdge.second;
    for (uint32_t i=0;i<this->EdgeList.size();i++){
        std::pair<uint32_t,uint32_t> tempEdge=this->EdgeList[i].GetEdge();
        uint32_t tempVertexFrom=tempEdge.first,tempVertexTo=tempEdge.second;
        if (VertexFrom == tempVertexFrom && VertexTo == tempVertexTo) return this->EdgeList[i].GetData();
    }
    return EdgeData();
}

template <typename VertexData,typename EdgeData>
EdgeData MadLibrary::Graph<VertexData,EdgeData>::GetEdgeData(uint32_t VertexFrom,uint32_t VertexTo){
    for (uint32_t i=0;i<this->EdgeList.size();i++){
        std::pair<uint32_t,uint32_t> tempEdge=this->EdgeList[i].GetEdge();
        uint32_t tempVertexFrom=tempEdge.first,tempVertexTo=tempEdge.second;
        if (VertexFrom == tempVertexFrom && VertexTo == tempVertexTo) return this->EdgeList[i].GetData();
    }
    return EdgeData();
}

//DeleteVertex
template <typename VertexData,typename EdgeData>
void MadLibrary::Graph<VertexData,EdgeData>::DeleteVertex(uint32_t VertexID){
    for (uint32_t i=0;i<this->VertexList.size();i++){
        if (this->VertexList[i].GetID()==VertexID){
            this->VertexList.erase(VertexList.begin()+i);
            return;
        }
    }
}

//DeleteEdge
template <typename VertexData,typename EdgeData>
void MadLibrary::Graph<VertexData,EdgeData>::DeleteEdge(std::pair<uint32_t,uint32_t> TheEdge){
    for (uint32_t i=0; i<this->EdgeList.size();i++){
        if (this->EdgeList[i].GetEdge()==TheEdge){
            this->EdgeList.erase(EdgeList.begin()+i);
        }
    }
}

template <typename VertexData,typename EdgeData>
void MadLibrary::Graph<VertexData,EdgeData>::DeleteEdge(uint32_t VertexFrom,uint32_t VertexTo){
    for (uint32_t i=0; i<this->EdgeList.size();i++){
        if (this->EdgeList[i].GetVertexFrom()==VertexFrom && this->EdgeList[i].GetVertexTo()==VertexTo){
            this->EdgeList.erase(EdgeList.begin()+i);
        }
    }
}

//GetVertexIDs
template <typename VertexData,typename EdgeData>
std::vector<uint32_t> MadLibrary::Graph<VertexData,EdgeData>::GetVertexIDs(){
    std::vector<uint32_t> newVector;
    for (uint32_t i=0;i<this->VertexList.size();i++){
        newVector.push_back(this->VertexList[i].GetID());
    }
    return newVector;
}

//GetEdges
template <typename VertexData,typename EdgeData>
std::vector<std::pair<uint32_t,uint32_t>> MadLibrary::Graph<VertexData,EdgeData>::GetEdgeVertices(){
    std::vector<std::pair<uint32_t,uint32_t>> newVector;
    for (uint32_t i=0;i<this->EdgeList.size();i++){
        newVector.push_back(this->EdgeList[i].GetEdge());
    }
    return newVector;
}

//GetVertexList
template <typename VertexData,typename EdgeData>
std::vector<MadLibrary::Vertex<VertexData>> MadLibrary::Graph<VertexData,EdgeData>::GetVertexList(){
    return this->VertexList;
}

//GetEdgeList
template <typename VertexData,typename EdgeData>
std::vector<MadLibrary::Edge<EdgeData>> MadLibrary::Graph<VertexData,EdgeData>::GetEdgeList(){
    return this->EdgeList;
}

//SetVertexList
template <typename VertexData,typename EdgeData>
void MadLibrary::Graph<VertexData,EdgeData>::SetVertexList(std::vector<MadLibrary::Vertex<VertexData>> newVertexList){
    this->VertexList=newVertexList;
}

//SetEdgeList
template <typename VertexData,typename EdgeData>
void MadLibrary::Graph<VertexData,EdgeData>::SetEdgeList(std::vector<MadLibrary::Edge<EdgeData>> newEdgeList){
    this->EdgeList=newEdgeList;
}


/*
            void SetVertexList(std::vector<Vertex<VertexData>> newVertexList);
            void SetEdgeList(std::vector<Edge<EdgeData>> newEdgeList);
*/
#endif