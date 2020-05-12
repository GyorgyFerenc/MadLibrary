#ifndef __Graph_HPP_INCLUDED__
#define __Graph_HPP_INCLUDED__

#include "MadLibrary.hpp"

///Graph
//AddVertex
template <typename VertexData,typename EdgeData>
void MadLibrary::Graph<VertexData,EdgeData>::AddVertex(VertexData Data){
    this->Vertices[this->IDcount]=Data;
    this->IDcount++;
}

//Constructor
template <typename VertexData,typename EdgeData>
MadLibrary::Graph<VertexData,EdgeData>::Graph(uint32_t NumberOfVertices, std::vector<VertexData> VertexDatas){
    for (uint32_t i=0;i<NumberOfVertices;i++){
        this->Vertices[IDcount]=VertexDatas[IDcount];
        IDcount++;
    }
}

template <typename VertexData,typename EdgeData>
MadLibrary::Graph<VertexData,EdgeData>::Graph(uint32_t NumberOfVertices){
    for (uint32_t i=0;i<NumberOfVertices;i++){
        this->Vertices[IDcount];
        IDcount++;
    }
}

template <typename VertexData,typename EdgeData>
MadLibrary::Graph<VertexData,EdgeData>::Graph(){
    return;
}

///UniqueGraph
//GetVertices
template <typename VertexType,typename VertexData,typename EdgeData>
std::vector<VertexType> MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::GetVertices(){
    std::vector<VertexType> Vertices;
    for (auto it=this->Vertices.begin();it!=this->Vertices.end();it++){
        Vertices.push_back(it->first);
    }
    return Vertices;
}

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

template <typename VertexType,typename VertexData,typename EdgeData>
template <typename Function>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::BreadthFirstSearch(VertexType Source, std::vector<VertexType>& Vertices,Function TheFunction){
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

        TheFunction(this,Current);

        for (auto it=this->Edges[Current].begin();it!=this->Edges[Current].end();it++){
            if (std::find(Visited.begin(),Visited.end(),it->first)!=Visited.end()) continue;
            TheQueue.push(it->first);
            Vertices.push_back(it->first);
            Visited.push_back(it->first);
        }
    }
    
}

template <typename VertexType,typename VertexData,typename EdgeData>
std::vector<VertexType> MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::BreadthFirstSearch(VertexType Source){
    std::vector<VertexType> Vertices;
    this->BreadthFirstSearch(Source,Vertices);
    return Vertices;
}

//DepthFirstSearch
template <typename VertexType,typename VertexData,typename EdgeData>
template<typename Function>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::DepthFirstSearch(VertexType Source, std::vector<VertexType>& Vertices,Function TheFunction){
    Vertices.clear();
    
    std::stack<VertexType> TheStack;

    std::vector<VertexType> Visited;

    TheStack.push(Source);
    Visited.push_back(Source);
    Vertices.push_back(Source);
    while (!TheStack.empty())
    {
        VertexType Current=TheStack.top();
        TheStack.pop();

        TheFunction(this,Current);

        for (auto it=this->Edges[Current].begin();it!=this->Edges[Current].end();it++){
            if (std::find(Visited.begin(),Visited.end(),it->first)!=Visited.end()) continue;
            TheStack.push(it->first);
            Vertices.push_back(it->first);
            Visited.push_back(it->first);
        }
    }
}

template <typename VertexType,typename VertexData,typename EdgeData>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::DepthFirstSearch(VertexType Source, std::vector<VertexType>& Vertices){
    Vertices.clear();
    
    std::stack<VertexType> TheStack;

    std::vector<VertexType> Visited;

    TheStack.push(Source);
    Visited.push_back(Source);
    Vertices.push_back(Source);
    while (!TheStack.empty())
    {
        VertexType Current=TheStack.top();
        TheStack.pop();

        for (auto it=this->Edges[Current].begin();it!=this->Edges[Current].end();it++){
            if (std::find(Visited.begin(),Visited.end(),it->first)!=Visited.end()) continue;
            TheStack.push(it->first);
            Vertices.push_back(it->first);
            Visited.push_back(it->first);
        }
    }
}

//Constructor
template <typename VertexType,typename VertexData,typename EdgeData>
MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::UniqueGraph(std::vector<VertexType> Vertecies,std::vector<VertexData> VertexDatas){
    for (uint32_t i=0;i<Vertices.size();i++){
        this->Vertices[Vertices[i]]=VertexDatas[i];
    }
}

template <typename VertexType,typename VertexData,typename EdgeData>
MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::UniqueGraph(std::vector<VertexType> Vertecies){
    for (uint32_t i=0;i<Vertices.size();i++){
        this->Vertices[Vertices[i]];
    }
}

template <typename VertexType,typename VertexData,typename EdgeData>
MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::UniqueGraph(){
    return;
}

//GetTopologicalSort
template <typename VertexType,typename VertexData,typename EdgeData>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::TopologicalSortUtil(VertexType Current, std::vector<VertexType> &Visited, std::stack<VertexType> &TheTopologicalStack){
    for (auto it=this->Edges[Current].begin();it!=this->Edges[Current].end();it++){
        if (std::find(Visited.begin(),Visited.end(),it->first)!=Visited.end()) continue;
        Visited.push_back(it->first);
        this->TopologicalSortUtil(it->first,Visited, TheTopologicalStack);
    }
    TheTopologicalStack.push(Current);
}

template <typename VertexType,typename VertexData,typename EdgeData>
void MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::GetTopologicalSort(std::vector<VertexType>& Vertices){
    Vertices.clear();
    
    std::stack<VertexType> TheStack, TheTopologicalStack;

    std::vector<VertexType> Visited;

    for (auto Element=this->Vertices.begin();Element!=this->Vertices.end();Element++){

        VertexType Source=Element->first;
        if (std::find(Visited.begin(),Visited.end(),Source)==Visited.end()){
            this->TopologicalSortUtil(Source,Visited, TheTopologicalStack);
        }
    }

    while (!TheTopologicalStack.empty())
    {
        Vertices.push_back(TheTopologicalStack.top());
        TheTopologicalStack.pop();
    }
}

template <typename VertexType,typename VertexData,typename EdgeData>
std::vector<VertexType> MadLibrary::UniqueGraph<VertexType,VertexData,EdgeData>::GetTopologicalSort(){
    std::vector<VertexType> Vertices;
    this->GetTopologicalSort(Vertices);
    return Vertices;
}
#endif