#include "../Includes/MadLibrary.hpp"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

int main(){
    std::vector<int> b;
    std::vector<int> a; //declaralod ,size=0
    a.push_back(12); //a vegere 12 , size=1
    a.push_back(124); //a vegere 124, size=2
    a.pop_back(); // kiveszem a 124, siz=1
    std::cout<<a.size();// vissza adja a sizet, 1
    a.insert(a.begin(),2);// az elejere berakja a 2-est, a tobbit eltolja, a.begin()-vissza ad egy iteratort az elso helyre
    a.insert(a.begin()+1,3);//az elso indexre berakja a 3-est, a tobbit eltolja, a.begin()-vissza ad egy iteratort az elso helyre
    a.clear();//torli az elemeket, size=0
    a[0]=12;//0-indexu elem 12 re valtja
    a.erase(a.begin()+2);//kitorli a 2-indexu elemet, tobbit elore hozza
    b=a;//b vektort egyenlove teszi az a vektorral
}