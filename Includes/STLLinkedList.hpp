#ifndef __STLLinkedList_HPP_INCLUDED__
#define __STLLinkedList_HPP_INCLUDED__

#include "MadLibrary.hpp"

//operator[]
template <typename DataType>
DataType MadLibrary::STLLinkedList<DataType>::operator[](size_t position){
    auto it=this->begin();
    for (int i=0;i<position;i++) it++;
    return *(it);
}
#endif