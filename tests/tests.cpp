#include <iostream>

int asd(int a)
{
    return a;
}

int main()
{
    auto asd2 = asd;
    std::cout << asd2(12);
}