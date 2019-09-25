#include "MadLibrary.h"
#include <ctgmath>

//NumberSize
template <class DataType>
unsigned int MadLibrary::NumberSize(DataType Number){
    unsigned int size=0;
    do{
        Number/=10;
        size++;
    }while (Number!=0);
    return size;
}

//SimplePow
template <class DataType>
DataType MadLibrary::SimplePow(DataType Number,int exp){
    if (exp==0) return 1;
    DataType temp=Number;
    for (int i=0;i<exp-1;i++){
        Number*=temp;
    }
    return Number;
}

//DeleteDigit
template <class DataType>
DataType MadLibrary::DeleteDigit(DataType Number,unsigned int pos){
    return Number/MadLibrary::SimplePow(10,MadLibrary::NumberSize(Number)-pos)*MadLibrary::SimplePow(10,MadLibrary::NumberSize(Number)-pos-1)+Number%MadLibrary::SimplePow(10,MadLibrary::NumberSize(Number)-pos-1);
}

//PrimeCheck
template <class DataType>
bool MadLibrary::PrimeCheck(DataType prime){
    if (prime<=1) return false;
    if (prime==2) return true;
    if (prime%2==0) return false;
    for (DataType i = 3; i <=sqrt(prime) ; i+=2)
    {
        if (prime%i==0) return false;
        
    }
    return true;
}