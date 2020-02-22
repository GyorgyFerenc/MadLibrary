#ifndef __Matrix_HPP_INCLUDED__
#define __Matrix_HPP_INCLUDED__

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

template <class DataType>
MadLibrary::Matrix<DataType> MadLibrary::Matrix<DataType>::ZeroMatrix(uint32_t number){
    return MadLibrary::Matrix<DataType>::ZeroMatrix(number,number);
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
            for (size_t z=0;z<other.GetRow();z++){
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

//GetDeterminant
template <class DataType>
DataType MadLibrary::Matrix<DataType>::GetDeterminant() const{
    DataType Det=0;
    //Check for a row with zeros
    for (size_t i = 0; i < this->GetRow(); i++)
    {
        bool a=true;
        for (size_t j = 0; j < this->GetColumn(); j++)
        {
            if (this->GetData(i,j)!=0){
                a=false;
                break;
            }
        }
        if (a)
        {
            return 0;
        }
    }
    //Check for identical rows
    for (size_t i = 0; i < this->GetRow()-1; i++)
    {
        bool a=true;
        for (size_t j = 0; j < this->GetColumn(); j++)
        {
            if (this->GetData(i,j)!=this->GetData(i+1,j)){
                a=false;
                break;
            }
        }
        if (a)
        {
            return 0;
        }
    }
    //Check for a column with zeros
    for (size_t i = 0; i < this->GetColumn(); i++)
    {
        bool a=true;
        for (size_t j = 0; j < this->GetRow(); j++)
        {
            if (this->GetData(j,i)!=0){
                a=false;
                break;
            }
        }
        if (a)
        {
            return 0;
        }
    }
    //Check for identical columns
    for (size_t i = 0; i < this->GetColumn()-1; i++)
    {
        bool a=true;
        for (size_t j = 0; j < this->GetRow(); j++)
        {
            if (this->GetData(j,i)!=this->GetData(j,i+1)){
                a=false;
                break;
            }
        }
        if (a)
        {
            return 0;
        }
    }
    if (this->GetRow()<=2) return this->GetData(0,0)*this->GetData(1,1)-this->GetData(1,0)*this->GetData(0,1);
    for (size_t i = 0; i < this->GetColumn(); i++)
    {
        MadLibrary::Matrix<DataType> newMatrix(this->GetRow()-1,this->GetColumn()-1,0);
        for (size_t j = 1; j < this->GetRow(); j++)
        {
            size_t t=0;
            for (size_t z = 0; z < newMatrix.GetColumn(); z++)
            {
                if (i==z){
                    t++;
                }
                newMatrix[j-1][z]=this->GetData(j,z+t);
                
            }
        }
        Det+=MadLibrary::SimplePow(-1,i)*this->GetData(0,i)*newMatrix.GetDeterminant();
    }
    return Det;
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

//cast operator
template <class DataType>
MadLibrary::Matrix<DataType>::operator std::vector<std::vector<DataType>>(){
    return this->vect;
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

//Destructor
template <class DataType>
MadLibrary::Matrix<DataType>::~Matrix(){
    this->Clean();
}
#endif