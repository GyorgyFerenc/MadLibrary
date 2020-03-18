/* Copyright (C) György Ferenc - All Rights Reserved
 * You are authorized to use this file in every project if you mention the creator!
 * Written by György Ferenc <gyferenc2002@gmail.com>, May 2019
 */

#ifndef __MadLibrary_HPP_INCLUDED__
#define __MadLibrary_HPP_INCLUDED__
    ///Inicializalas
#include <cstddef>
#include <list>
#include <iterator>
#include <vector>
namespace MadLibrary{
    //INIT
    template <class DataType>
    class Matrix;
    template <typename VertexData,typename EdgeData>
    class Graph;
    template <typename VertexData>
    class Vertex;
    template <typename EdgeData>
    class Edge;

    //Dijkstra
    template <class DataType>
    void Dijkstra(std::vector<std::vector<DataType>> Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<DataType>& distance);
    template <class DataType>
    void Dijkstra(Matrix<DataType> Graph, uint32_t source, std::vector<uint32_t>& previous, std::vector<DataType>& distance);
    
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
            template <typename OutputStream>
            void Plot(OutputStream& Os) const;
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
    
    template <typename VertexData>
    class Vertex{
        protected:
            uint32_t ID;
            VertexData Data;
        public:
            Vertex(uint32_t ID,VertexData Data);
            void SetID(uint32_t ID);
            uint32_t GetID();
            void SetData(VertexData Data);
            VertexData GetData();
    };

    template <typename EdgeData>
    class Edge{
        protected:
            uint32_t VertexTo,VertexFrom;
            EdgeData Data;
        public:
            Edge(uint32_t VertexFrom, uint32_t VertexTo,EdgeData Data);
            uint32_t GetVertexFrom();
            uint32_t GetVertexTo();
            std::pair<uint32_t,uint32_t> GetEdge();
            void SetVertexFrom(uint32_t VertexFrom);
            void SetVertexTo(uint32_t VertexTo);
            void SetEdge(uint32_t VertexFrom, uint32_t VertexTo);
            void SetEdge(std::pair<uint32_t,uint32_t> NewEdge);
            void SetData(EdgeData Data);
            EdgeData GetData();
    };

    template <typename VertexData,typename EdgeData>
    class Graph
    {
        protected:
            std::vector<Vertex<VertexData>> VertexList;
            std::vector<Edge<EdgeData>> EdgeList;
            uint32_t IDCount=0;
        public:
            void AddVertex(VertexData Data);
            void AddEdge(uint32_t VertexFrom,uint32_t VertexTo,EdgeData Data);
            VertexData GetVertexData(uint32_t ID);
            EdgeData GetEdgeData(std::pair<uint32_t,uint32_t> TheEdge);
            EdgeData GetEdgeData(uint32_t VertexFrom,uint32_t VertexTo);
    };
}

#include "SimpleFunctions.hpp"
#include "Matrix.hpp"
#include "Graph.hpp"

#endif 