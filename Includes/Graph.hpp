#ifndef __Graph_HPP_INCLUDED__
#define __Graph_HPP_INCLUDED__

#include "MadLibrary.hpp"

//AddVertex
template <typename VertexData,typename EdgeData>
void MadLibrary::Graph<VertexData,EdgeData>::AddVertex(VertexData Data){
    this->Vertices[this->IDcount]=Data;
    this->IDcount++;
}

///UniqueGraph
//AddVertex
template <typename VertexType,typename VertexData,typename EdgeData>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::AddVertex(VertexType Vertex,VertexData Data){
    this->Vertices[Vertex]=Data;
}

//AddEdge
template <typename VertexType,typename VertexData,typename EdgeData>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::AddEdge(VertexType VertexFrom, VertexType VertexTo, EdgeData Data){
    this->Edges[VertexFrom][VertexTo]=Data;
}

//AddBidirectionalEdge
template <typename VertexType,typename VertexData,typename EdgeData>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::AddBidirectionalEdge(VertexType VertexFrom, VertexType VertexTo, EdgeData Data){
    this->Edges[VertexFrom][VertexTo]=Data;
    this->Edges[VertexTo][VertexFrom]=Data;
}

//GetVertexData
template <typename VertexType,typename VertexData,typename EdgeData>
VertexData MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::GetVertexData(VertexType Vertex){
    return this->Vertices[Vertex];
}

//GetEdgeData
template <typename VertexType,typename VertexData,typename EdgeData>
EdgeData MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::GetEdgeData(VertexType VertexFrom, VertexType VertexTo){
    return this->Edges[VertexFrom][VertexTo];
}

//DeleteVertex
template <typename VertexType,typename VertexData,typename EdgeData>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::DeleteVertex(VertexType Vertex){
    this->Vertices.erase(Vertex);
    this->Edges.erase(Vertex);
    for (auto it=Edges.begin();it!=Edges.end();it++){
        it->second.erase(Vertex);
    }
}

//DeleteEdge
template <typename VertexType,typename VertexData,typename EdgeData>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::DeleteEdge(VertexType VertexFrom, VertexType VertexTo){
    this->Edges[VertexFrom].erase(VertexTo);
}

//Dijkstra
template <typename VertexType,typename VertexData,typename EdgeData>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::Dijkstra(VertexType Source,std::vector<EdgeData>& Distance,std::vector<VertexType>& Previous){
    auto Compare = [](std::pair<VertexType,EdgeData> First, std::pair<VertexType,EdgeData> Second) -> bool{
        return First.second > Second.second;
    };
    this->Dijkstra(Source,Distance,Previous,Compare);
}

template <typename VertexType,typename VertexData,typename EdgeData>
template <typename Compare>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::Dijkstra(VertexType Source,std::vector<EdgeData>& Distance,std::vector<VertexType>& Previous, Compare CompAlg){
    Distance.clear();
    Previous.clear();

    for (uint32_t i=0;i<this->Vertices.size();i++){
        Distance.push_back(std::numeric_limits<EdgeData>::max());
        Previous.push_back(std::numeric_limits<VertexType>::max());
    }

    Distance[Source]=0;

    std::priority_queue<std::pair<VertexType,EdgeData>,std::vector<std::pair<VertexType,EdgeData>>,decltype(CompAlg)> PriorityQueue(CompAlg);

    PriorityQueue.push(std::pair<VertexType,EdgeData>(Source,0));

    while (!PriorityQueue.empty())
    {
        VertexType Current= PriorityQueue.top().first;
        PriorityQueue.pop();

        for (auto it=this->Edges[Current].begin();it!=this->Edges[Current].end();it++){
            VertexType TheVertex=it->first;
            EdgeData TheEdgeData=it->second;

            if (Distance[TheVertex]>Distance[Current]+TheEdgeData){
                Distance[TheVertex]=Distance[Current]+TheEdgeData;
                Previous[TheVertex]=Current;
                PriorityQueue.push(std::pair<VertexType,EdgeData>(TheVertex,Distance[TheVertex]));
            }
        }
    }
    
}

//BreadthFirstSearch
template <typename VertexType,typename VertexData,typename EdgeData>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::BreadthFirstSearch(VertexType Source, std::vector<VertexType>& Vertices){
    Vertices.clear();
    
    std::queue<VertexType> TheQueue;

    std::vector<VertexType> Visited;

    TheQueue.push(Source);
    Visited.push_back(Source);
    Vertices.push_back(Source);
    while (!TheQueue.empty())
    {
        VertexType Current=TheQueue.front();
        TheQueue.pop();

        for (auto it=this->Edges[Current].begin();it!=this->Edges[Current].end();it++){
            if (std::find(Visited.begin(),Visited.end(),it->first)!=Visited.end()) continue;
            TheQueue.push(it->first);
            Vertices.push_back(it->first);
            Visited.push_back(it->first);
        }
    }
    
}


#endif