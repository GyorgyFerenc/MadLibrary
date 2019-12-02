#ifndef __Time_HPP_INCLUDED__
#define __Time_HPP_INCLUDED__

#include "MadLibrary.hpp"

//GetRow
template <class DataType>
uint32_t MadLibrary::Matrix<DataType>::GetRow() const{
    return this->row;
}

//GetColumn
template <class DataType>
uint32_t MadLibrary::Matrix<DataType>::GetColumn() const{
    return this->col;
}

//ZeroMatrix
template <class DataType>
MadLibrary::Matrix<DataType> MadLibrary::Matrix<DataType>::ZeroMatrix(uint32_t row,uint32_t col){
    MadLibrary::Matrix<DataType> temp(row,col,0);
    return temp;
}

//ZeroMatrix
template <class DataType>
MadLibrary::Matrix<DataType> MadLibrary::Matrix<DataType>::IdentityMatrix(uint32_t size){
    MadLibrary::Matrix<DataType> temp(size,size,0);
    for (uint32_t i=0;i<size;i++){
        for (uint32_t j=0;j<size;j++){
            if (i==j){
                temp[i][j]=1;
            }
        }
    }
    return temp;
}

//operator=
template <class DataType>
void MadLibrary::Matrix<DataType>::operator=(const Matrix& other){
    this->Resize(other.GetRow(),other.GetColumn());
    for (uint32_t i=0;i<other.GetRow();i++){
        vect[i]=other[i];
    }
}

//operator+
template <class DataType>
MadLibrary::Matrix<DataType> MadLibrary::Matrix<DataType>::operator+(MadLibrary::Matrix<DataType> other){
    MadLibrary::Matrix<DataType> temp(this->GetRow(),this->GetColumn());
    for (size_t i=0;i<this->GetRow();i++){
        for (size_t j=0;j<this->GetColumn();j++){
            temp[i][j]=this->GetData(i,j)+other.GetData(i,j);
        }
    }
    return temp;
}

//operator*
template <class DataType>
MadLibrary::Matrix<DataType> MadLibrary::Matrix<DataType>::operator*(MadLibrary::Matrix<DataType> other){
    MadLibrary::Matrix<DataType> temp(this->GetRow(),other.GetColumn());
    for (size_t i=0;i<temp.GetRow();i++){
        for (size_t j=0;j<temp.GetColumn();j++){
            DataType tempData=this->GetData(0,0)-this->GetData(0,0);
            for (size_t z=0;z<temp.GetRow();z++){
                tempData+=this->GetData(i,z)*other.GetData(z,j);
            }
            temp[i][j]=tempData;
        }
    }
    return temp;
}

//GetData
template <class DataType>
DataType MadLibrary::Matrix<DataType>::GetData(uint32_t row,uint32_t col) const{
    return this->vect[row][col];
}

//operator[]
template <class DataType>
std::vector<DataType>& MadLibrary::Matrix<DataType>::operator[](size_t position){
    return vect[position];
}

template <class DataType>
std::vector<DataType> MadLibrary::Matrix<DataType>::operator[](size_t position) const{
    return vect[position];
}

//Resize
template <class DataType>
void MadLibrary::Matrix<DataType>::Resize(const uint32_t row,const uint32_t col){
    this->row=row;
    this->col=col;
    this->Clean();
    vect.resize(row);
    for (size_t i=0;i<vect.size();i++){
        std::vector<DataType> v(col);
        vect[i]=v;
    }
}

//Clean
template <class DataType>
void MadLibrary::Matrix<DataType>::Matrix::Clean(){
    this->vect.clear();
}

//Fill
template <class DataType>
void MadLibrary::Matrix<DataType>::Fill(DataType data){
    for (uint64_t i=0;i<this->row;i++){
        for (uint64_t j=0;j<this->col;j++){
            this->vect[i][j]=data;
        }
    }
}

//Constructor
template <class DataType>
MadLibrary::Matrix<DataType>::Matrix(uint32_t row,uint32_t col, DataType data){
    this->Resize(row,col);
    this->Fill(data);
}

template <class DataType>
MadLibrary::Matrix<DataType>::Matrix(const Matrix& other){
    this->operator=(other);
}

template <class DataType>
MadLibrary::Matrix<DataType>::Matrix(uint32_t row,uint32_t col){
    this->Resize(row,col);
}

template <class DataType>
MadLibrary::Matrix<DataType>::Matrix(){
    this->Resize(0,0);
}


#endif