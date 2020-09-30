
#include <iostream>
#include "../MadLibrary/SimpleFunctions.hpp"
int main(void)
{
   std::vector<std::vector<int>> asd({1, 2, 3}, {4, 5, 6});
   std::cout << MadLibrary::toCleverString(asd);
}