/* Copyright (C) György Ferenc - All Rights Reserved
 * You are authorized to use this file in every project if you mention the creator!
 * Written by György Ferenc <gyferenc2002@gmail.com>, May 2019
 */

#ifndef __MadLibrary_HPP_INCLUDED__
#define __MadLibrary_HPP_INCLUDED__
    ///Inicializalas
#include <cstddef>
#include <list>
namespace MadLibrary{
    template <class DataType>
    unsigned int NumberSize(DataType Number);
    template <class DataType>
    DataType SimplePow(DataType Number,int exp);
    template <class DataType>
    bool PrimeCheck(DataType prime);
    template <class DataType>
    DataType DeleteDigit(DataType Number,unsigned int pos);
    template <class DataType>
    struct node{
        DataType data;
        node* next;
        node* previous;
    };
    template <class DataType>
    class LinkedList{
        private:
            node<DataType>* inPoint;
        public:
            LinkedList(DataType data);
            LinkedList();
            size_t Size();
            DataType GetData(unsigned int position);
            DataType operator[](unsigned int position);
            void operator=(LinkedList<DataType> fromLinkedList);
            void Remove(int position);
            void MakeEmpty();
            void AddToEnd(DataType data);
            void Insert(DataType data,unsigned int position);
            void AddToBegin(DataType data);
    };
    template <class DataType>
    class STLLinkedList:public std::list<DataType>{
        public:
            DataType operator[](size_t position);
    };
}
#include "LinkedList.hpp"
#include "SimpleFunctions.hpp"
#include "STLLinkedList.hpp"
#endif 