#include "../Includes/MadLibrary.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>



int main(){
    MadLibrary::Matrix<uint32_t> a(5,5);
    std::ifstream iF("tests/matrix.in");
    int n;
    iF>>n;
    std::cout<<n<<std::endl;
    for (uint32_t i=0;i<n;i++){
        for (uint32_t j=0;j<n;j++){
            iF>>a[i][j];
        }
    }
    for (uint32_t i=0;i<n;i++){
        for (uint32_t j=0;j<n;j++){
            std::cout<<a[i][j]<<" ";
        }
        std::cout<<std::endl;
    }
    std::vector<uint32_t> prev,dist;
    MadLibrary::Dijkstra(a,0,prev,dist);
    std::cout<<dist[3];
}