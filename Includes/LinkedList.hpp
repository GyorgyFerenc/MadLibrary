/* Copyright (C) György Ferenc - All Rights Reserved
 * You are authorized to use this file in every project if you mention the creator!
 * Written by György Ferenc <gyferenc2002@gmail.com>, May 2019
 */
#ifndef __LinkedList_HPP_INCLUDED__
#define __LinkedList_HPP_INCLUDED__

#include <iostream>
#include <stdexcept>
#include "MadLibrary.hpp"

//operator=
template <class DataType>
void MadLibrary::LinkedList<DataType>::operator=(LinkedList<DataType> fromLinkedList){
    
    this->MakeEmpty();
    for (int i=0;i<fromLinkedList.Size();i++){
        this->AddToEnd(fromLinkedList[i]);
    }
}

//Remove
template <class DataType>
void MadLibrary::LinkedList<DataType>::Remove(int position){
    MadLibrary::node<DataType>* temp=this->inPoint;
    if (position==0 && temp->next!=NULL)
    {
        this->inPoint=temp->next;
        delete temp;
        return;
    }else if (position==0)
    {
        delete temp;
        this->inPoint=NULL;
        return;
    }
    
    for (int i=0;i<position;i++)
    {
        temp=temp->next;
        if (temp==NULL){
            break;
        }
    }
    temp->previous->next=temp->next;
    delete temp;
}

//MakeEmpty
template <class DataType>
void MadLibrary::LinkedList<DataType>::MakeEmpty()
{
    MadLibrary::node<DataType>* temp=this->inPoint;
    if (temp==NULL)
    {
        return;
    }
    while(temp->next!=NULL){
        temp=temp->next;
    }
    while (1)
    {
        temp=temp->previous;
        if (temp==NULL)
        {
            delete this->inPoint;
            this->inPoint=NULL;
            break;
        }
        delete temp->next;
    }
}

//Size
template <class DataType>
size_t MadLibrary::LinkedList<DataType>::Size()
{
    MadLibrary::node<DataType>* temp=this->inPoint;
    int size=0;
    while(temp!=NULL)
    {
        temp=temp->next;
        size++;
    }
    return size;
}

//Constructors
template <class DataType>
MadLibrary::LinkedList<DataType>::LinkedList(DataType data){
    this->inPoint=new node<DataType>;
    inPoint->data=data;
    inPoint->next=NULL;
    inPoint->previous=NULL;
}

template <class DataType>
MadLibrary::LinkedList<DataType>::LinkedList(){
    this->inPoint=NULL;
}

//AddToBegin
template <class DataType>
void MadLibrary::LinkedList<DataType>::AddToBegin(DataType data){
    MadLibrary::node<DataType>* newNode=new node<DataType>;
    newNode->data=data;
    newNode->next=NULL;
    newNode->previous=NULL;
    if (this->inPoint==NULL)
    {
        this->inPoint=newNode;
        return;
    }
    newNode->next=this->inPoint;
    this->inPoint->previous=newNode;
    inPoint=newNode;
}

//Insert
template <class DataType>
void MadLibrary::LinkedList<DataType>::Insert(DataType data,unsigned int position){
    MadLibrary::node<DataType>* theNewNode=new node<DataType>;
    MadLibrary::node<DataType>* temp=this->inPoint;
    theNewNode->data=data;
    theNewNode->next=NULL;
    if (position==0)
    {
        this->AddToBegin(data);
        return;
    }
    for(int i=0;i<position-1;i++)
    {
        temp=temp->next;
        if (temp==NULL)
        {
            throw std::length_error("invalid position");
        }

    }
    theNewNode->next=temp->next;
    temp->next=theNewNode;
    theNewNode->previous=temp;

}

//AddToEnd
template <class DataType>
void MadLibrary::LinkedList<DataType>::AddToEnd(DataType data){
    MadLibrary::node<DataType>* theNewNode=new node<DataType>;
    MadLibrary::node<DataType>* temp=this->inPoint;
    theNewNode->data=data;
    theNewNode->next=NULL;
    if (temp==NULL)
    {
        this->inPoint=theNewNode;
        return;
    }
    while(temp->next!=NULL)
    {
        temp=temp->next;
    }
    temp->next=theNewNode;
    theNewNode->previous=temp;
}

//GetData
template <class DataType>
DataType& MadLibrary::LinkedList<DataType>::GetData(unsigned int position){
    MadLibrary::node<DataType>* temp=this->inPoint;
    for (int i=0;i<position;i++)
    {
        temp=temp->next;
    }
    return temp->data;
}

//operator[]
template <class DataType>
DataType& MadLibrary::LinkedList<DataType>::operator[](unsigned int position)
{
    return this->GetData(position);
}

#endif