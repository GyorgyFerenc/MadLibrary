// CPP program to illustrate 
// concept of Virtual Functions 
  
#include <iostream> 
using namespace std; 
  
class base { 
public: 
    virtual void print() 
    { 
        cout << "print base class" << endl; 
    } 
    virtual base* GetThis(){
        return this;
    }
  
    void show() 
    { 
        cout << "show base class" << endl; 
    } 

    virtual void ThePrint(){
        std::cout<<typeid(this->GetThis()).name();
    }
}; 
  
class derived : public base { 
public:  
    derived* GetThis(){
        return this;
    }

    void show() 
    { 
        cout << "show derived class" << endl; 
    } 
}; 
  
int main() 
{ 
    base* bptr; 
    derived d; 
    bptr = &d; 
  
    d.ThePrint();
} 