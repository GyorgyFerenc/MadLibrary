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
    template<typename Iter>
    void MergeSort(Iter beg, Iter end); //by  zachwhaley
    
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
        private:
            std::vector<std::vector<DataType>> vect;
            uint32_t row,col;
        public:
            operator std::vector<std::vector<DataType>>();
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
            uint32_t GetRow() const;
            uint32_t GetColumn() const;
            std::vector<DataType>& operator[](size_t position);
            std::vector<DataType> operator[](size_t position) const;
            Matrix<DataType> operator+(const Matrix<DataType> other);
            void operator=(const Matrix<DataType>& other);
            Matrix<DataType> operator*(const Matrix<DataType> other);
            ~Matrix();
    };
    
    //Node
    template <class DataType>
    struct node{
        DataType data;
        node* next;
        node* previous;
    };
    

    //LinkedList
    template <class DataType>
    class LinkedList{
        private:
            node<DataType>* inPoint;
        public:
            LinkedList(DataType data);
            LinkedList();
            size_t Size();
            DataType& GetData(unsigned int position);
            DataType& operator[](unsigned int position);
            void operator=(LinkedList<DataType> fromLinkedList);
            void Remove(int position);
            void MakeEmpty();
            void AddToEnd(DataType data);
            void Insert(DataType data,unsigned int position);
            void AddToBegin(DataType data);
    };

    //STLLinkedList and his Iterator
    template <class DataType>
    class STLLinkedListIterator;

    template <class DataType>
    class STLLinkedList:public std::list<DataType>{
        public:
            DataType& operator[](size_t position);
            STLLinkedListIterator<DataType> RandBegin();
            STLLinkedListIterator<DataType> RandEnd();
    };

    template <class DataType>
    class STLLinkedListIterator:public std::random_access_iterator_tag,std::_List_iterator<DataType>{
        private:
            STLLinkedList<DataType> *List;
            size_t pos=0;
        public:
            STLLinkedListIterator(STLLinkedList<DataType> *List);
            const STLLinkedList<DataType>* GetListPonter();
            size_t GetPos();
            void operator+=(int number); 
            void operator-=(int number);
            void operator++();
            void operator--();
            void operator++(int);
            void operator--(int);
            bool operator==(STLLinkedListIterator<DataType> &temp);
            bool operator!=(STLLinkedListIterator<DataType> &temp);
            void operator=(STLLinkedListIterator<DataType> &temp);
            STLLinkedListIterator<DataType> operator+(int number);
            DataType& operator*();
    };
    
}
#include "LinkedList.hpp"
#include "SimpleFunctions.hpp"
#include "STLLinkedList.hpp"
#include "Matrix.hpp"
#endif 