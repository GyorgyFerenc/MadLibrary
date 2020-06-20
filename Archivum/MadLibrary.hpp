/* Copyright (C) György Ferenc - All Rights Reserved
 * You are authorized to use this file in every project if you mention the creator!
 * Written by György Ferenc <gyferenc2002@gmail.com>, May 2019
 */

/*
TODO:
*/
#ifndef __MadLibrary_HPP_INCLUDED__
#define __MadLibrary_HPP_INCLUDED__

///Inicializalas
#include <fstream>
#include <vector>
#include <sstream>
#include <algorithm>
#include <bitset>
#include <queue>
#include <map>
#include <stack>
#include <direct.h>

#if defined(_GLIBCXX_LIST) || defined(_LIST_)  
    #define ListIncluded true
#else
    #define ListIncluded false
#endif

#if defined(_GLIBCXX_ARRAY) || defined(_ARRAY_)
    #define ArrayIncluded true
#else
    #define ArrayIncluded false
#endif

#if defined(_GLIBCXX_FORWARD_LIST) || defined(_FORWARD_LIST_)
    #define ForwardListIncluded true
#else
    #define ForwardListIncluded false
#endif


namespace MadLibrary{
    ///INIT
    //Matrix
    template <class DataType>
    class Matrix;

    //Graph
    template <typename VertexType,typename VertexData,typename EdgeData>
    class UniqueGraph;
    template <typename VertexData,typename EdgeData>
    class Graph;
    
    ///FileHandler
    //CheckForFileExists
    bool CheckForFileExists(const std::string& name);

    ///SimpleFunctions
    //AppendVectors
    template <typename DataType>
    std::vector<DataType> AppendVectors(std::vector<DataType> &First,const std::vector<DataType> &Second);

    //StringParser
    std::vector<std::string> StringParser(std::string TheString,bool CheckForWhiteSpaces,std::vector<char> OtherCharToCheckFor);
    std::vector<std::string> StringParser(std::string TheString);

    //toCleverString
    template <typename DataType>
    std::string toCleverString(DataType data);
    template <typename DataType>
    std::string toCleverString(DataType data,uint32_t flag);
    std::string toCleverString(const std::string data);
    std::string toCleverString(char data);
    std::string toCleverString(const char* data);
    std::string toCleverString(char* data);
    template <typename DataType1,typename DataType2>
    std::string toCleverString(std::pair<DataType1,DataType2> thePair);
    template <typename DataType1,typename DataType2>
    std::string toCleverString(std::map<DataType1,DataType2> theMap);
    template <typename DataType>
    std::string toCleverString(std::vector<DataType> vect);
    template <typename DataType>
    std::string toCleverString(std::initializer_list<DataType> initList);
    #if ListIncluded
        template <typename DataType>
        std::string toCleverString(std::list<DataType> TheList);
    #endif
    #if ArrayIncluded
        template <typename DataType, std::size_t Size>
        std::string toCleverString(std::array<DataType,Size> TheArray);
    #endif
    #if ForwardListIncluded
        template <typename DataType>
        std::string toCleverString(std::forward_list<DataType> TheForwardList);
    #endif

    //Dijkstra
    template <class DataType>
    void Dijkstra(std::vector<std::vector<DataType>> &Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<DataType>& distance);
    template <class DataType>
    void Dijkstra(Matrix<DataType> &Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<DataType>& distance);
    
    //Abs
    template<class DataType>
    DataType Abs(DataType data);

    //PolygonArea
    template<class DataType,class DataType2>
    void PolygonArea(std::vector<DataType> X, std::vector<DataType> Y,DataType2 &temp);
    
    //MergeSort
    template<class Iter, class Comper>
    void MergeSort(Iter beg, Iter end, Comper comp);
    template<class Iter>
    void MergeSort(Iter beg, Iter end);
    
    //StringToNumber
    template <class DataType,class RandomAccesIterator>
    void StringToNumber(RandomAccesIterator begin,RandomAccesIterator end,DataType& toData);

    //NumberSize
    template <class DataType>
    uint32_t NumberSize(DataType Number);

    //SimplePow
    template <class DataType>
    DataType SimplePow(DataType Number,uint32_t exp);

    //PrimeCheck
    template <class DataType>
    bool PrimeCheck(DataType prime);

    //DeleteDigit
    template <class DataType>
    DataType DeleteDigit(DataType Number,uint32_t pos);

    //Map
    double Map(double value, double start1, double stop1, double start2, double stop2);

    //Matrix
    template <class DataType>
    class Matrix
    {
        protected:
            std::vector<std::vector<DataType>> vect;
            uint32_t row,col;
        public:
            static Matrix<DataType> ZeroMatrix(uint32_t row,uint32_t col);
            static Matrix<DataType> ZeroMatrix(uint32_t number);
            static Matrix<DataType> IdentityMatrix(uint32_t size);
            Matrix(const uint32_t row,const uint32_t col, DataType fill);
            Matrix(const uint32_t row,const uint32_t col);
            Matrix();
            Matrix(const Matrix& other);
            void Fill(DataType data);
            void Resize(const uint32_t row,const uint32_t col);
            void Clean();
            DataType GetData(uint32_t row,uint32_t col) const;
            DataType GetDeterminant() const;
            DataType GetSubDeterminant(uint32_t row, uint32_t col) const;
            uint32_t GetRow() const;
            uint32_t GetColumn() const;
            std::vector<DataType>& operator[](size_t position);
            std::vector<DataType> operator[](size_t position) const;
            Matrix<DataType> operator+(const Matrix<DataType> other);
            void operator+=(const Matrix<DataType> other);
            void operator=(const Matrix<DataType>& other);
            Matrix<DataType> operator*(const Matrix<DataType> other);
            Matrix<DataType> operator*(const DataType Data);
            void operator*=(const Matrix<DataType> other);
            void operator*=(const DataType Data);
            Matrix<DataType> operator/(Matrix<DataType> other);
            Matrix<DataType> operator/(const DataType Data);
            void operator/=(const Matrix<DataType> other);
            void operator/=(const DataType Data);
            Matrix<DataType> GetInverse();
            explicit operator std::vector<std::vector<DataType>>();
            ~Matrix();
    };

    //Graph
    template <typename VertexData,typename EdgeData>
    class Graph:public UniqueGraph<uint32_t,VertexData,EdgeData>{
        private:
            uint32_t IDcount=0;
        public:
            void AddVertex(VertexData Data);
            Graph(uint32_t NumberOfVertices, std::vector<VertexData> VertexDatas);
            Graph(uint32_t NumberOfVertices);
            Graph();
    };

    template <typename VertexType,typename VertexData,typename EdgeData>
    class UniqueGraph{
        protected:
            void TopologicalSortUtil(VertexType Current, std::vector<VertexType> &Visited, std::stack<VertexType> &TheTopologicalStack);
            std::map<VertexType,VertexData> Vertices;
            std::map<VertexType,std::map<VertexType,EdgeData>> Edges;
        public:
            UniqueGraph(std::vector<VertexType> Vertecies,std::vector<VertexData> VertexDatas);
            UniqueGraph(std::vector<VertexType> Vertecies);
            UniqueGraph();
            std::vector<VertexType> GetVertices();
            void AddVertex(VertexType Vertex,VertexData Data);
            void AddEdge(VertexType VertexFrom, VertexType VertexTo, EdgeData Data);
            void AddBidirectionalEdge(VertexType VertexFrom, VertexType VertexTo, EdgeData Data);
            VertexData GetVertexData(VertexType Vertex);
            EdgeData GetEdgeData(VertexType VertexFrom, VertexType VertexTo);
            void DeleteVertex(VertexType Vertex);
            void DeleteEdge(VertexType VertexFrom, VertexType VertexTo);
            void Dijkstra(VertexType Source,std::vector<EdgeData>& Distance,std::vector<VertexType>& Previous);
            template<typename Compare> 
            void Dijkstra(VertexType Source,std::vector<EdgeData>& Distance,std::vector<VertexType>& Previous,Compare CompAlg);
            void BreadthFirstSearch(VertexType Source, std::vector<VertexType>& Vertices);
            std::vector<VertexType> BreadthFirstSearch(VertexType Source);
            template<typename Function>
            void BreadthFirstSearch(VertexType Source, std::vector<VertexType>& Vertices,Function TheFunction); 
            template<typename Function>
            void DepthFirstSearch(VertexType Source, std::vector<VertexType>& Vertices,Function TheFunction);
            void DepthFirstSearch(VertexType Source, std::vector<VertexType>& Vertices);
            std::vector<VertexType> DepthFirstSearch(VertexType Source);
            void GetTopologicalSort(std::vector<VertexType>& Vertices);
            std::vector<VertexType> GetTopologicalSort();
            UniqueGraph<VertexType,VertexData,EdgeData> PrimSpanningTreeGraph();
            std::map<VertexType,VertexType> PrimSpanningTreeParent();
    };
}

#include "SimpleFunctions.hpp"
#include "Matrix.hpp"
#include "Graph.hpp"

#endif 