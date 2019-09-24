#include "MadLibrary.h"
template <class DataType>
bool MadLibrary::PrimeCheck(DataType prime){
    if (prime<=1) return false;
    if (prime==2) return true;
    if (prime%2==0) return false;
    for (size_t i = 3; (i*i) <=prime ; i+=2)
    {
        if (prime%i==0) return false;
        
    }
    return true;
}