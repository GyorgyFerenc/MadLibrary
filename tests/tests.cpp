#include <iostream>
#include "../MadLibrary/Matrix.hpp"

int main()
{
    MadLibrary::Matrix<int> asd(2, 2, 0);
    asd[0][0] = 12;
    std::cout << asd;
}