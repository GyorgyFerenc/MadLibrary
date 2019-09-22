/* Copyright (C) György Ferenc - All Rights Reserved
 * You are authorized to use this file in every project if you mention the creator!
 * Written by György Ferenc <gyferenc2002@gmail.com>, May 2019
 */

#ifndef __MadLibrary_H_INCLUDED__
#define __MadLibrary_H_INCLUDED__
    ///Inicializalas
namespace MadLibrary{
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
}
#include "LinkedList.cpp"
#endif 