#include <iostream>
#include "../MadLibrary/Matrix.hpp"
#include "../MadLibrary/SimpleFunctions.hpp"
#include <utility>

int main()
{
    MadLibrary::Matrix<int> asd = MadLibrary::Matrix<int>::IdentityMatrix(5);
    std::cout << MadLibrary::toCleverString(asd);
}