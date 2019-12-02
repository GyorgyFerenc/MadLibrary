#ifndef __STLLinkedList_HPP_INCLUDED__
#define __STLLinkedList_HPP_INCLUDED__

#include "MadLibrary.hpp"

///STLLinkedList

//operator[]
template <typename DataType>
DataType& MadLibrary::STLLinkedList<DataType>::operator[](size_t position){
    auto it=this->begin();
    for (int i=0;i<position;i++) it++;
    return *(it);
}

//RandBegin
template <class DataType>
MadLibrary::STLLinkedListIterator<DataType> MadLibrary::STLLinkedList<DataType>::RandBegin(){
    STLLinkedListIterator<DataType> temp(this);
    return temp;
}

//RandEnd
template <class DataType>
MadLibrary::STLLinkedListIterator<DataType> MadLibrary::STLLinkedList<DataType>::RandEnd(){
    STLLinkedListIterator<DataType> temp(this);
    temp+=this->size()-1;
    return temp;
}

///STLLinkedListIterator

//constructor
template <class DataType>
MadLibrary::STLLinkedListIterator<DataType>::STLLinkedListIterator(STLLinkedList<DataType> *List){
    this->List=List;
}

//GetListPonter
template <class DataType>
const MadLibrary::STLLinkedList<DataType>* MadLibrary::STLLinkedListIterator<DataType>::GetListPonter(){
    return this->List;
}
//GetPos
template <class DataType>
size_t MadLibrary::STLLinkedListIterator<DataType>::GetPos(){
    return this->pos;
}

//==
template <class DataType>
bool MadLibrary::STLLinkedListIterator<DataType>::operator==(STLLinkedListIterator<DataType> &temp){
    return this->GetListPonter() == temp.GetListPonter();
}

//!=
template <class DataType>
bool MadLibrary::STLLinkedListIterator<DataType>::operator!=(STLLinkedListIterator<DataType> &temp){
    return !(this->operator==(temp));
}

//--
template <class DataType>
void MadLibrary::STLLinkedListIterator<DataType>::operator--(){
    this->operator-=(1);
}

//++
template <class DataType>
void MadLibrary::STLLinkedListIterator<DataType>::operator++(){
    this->operator+=(1);
}

//--
template <class DataType>
void MadLibrary::STLLinkedListIterator<DataType>::operator--(int){
    this->operator-=(1);
}

//++
template <class DataType>
void MadLibrary::STLLinkedListIterator<DataType>::operator++(int){
    this->operator+=(1);
}

//+
template <class DataType>
MadLibrary::STLLinkedListIterator<DataType> MadLibrary::STLLinkedListIterator<DataType>::operator+(int number){
    STLLinkedListIterator<DataType> temp(this->List);
    temp+=number;
    return temp;
}

//*
template <class DataType>
DataType& MadLibrary::STLLinkedListIterator<DataType>::operator*(){
    return this->List->operator[](pos);
}
//-=
template <class DataType>
void MadLibrary::STLLinkedListIterator<DataType>::operator-=(int number){
    this->pos-=number;
}

//+=
template <class DataType>
void MadLibrary::STLLinkedListIterator<DataType>::operator+=(int number){
    this->pos+=number;
}

#endif