#include <iostream>
#include <memory>
#include <ranges>

#include "include/MadLibrary.hpp"

class Student {
   public:
    var_p(name, std::string);
    var_p(age, uint32_t);

   public:
    Student() {
        this->name = "";
        this->age = 0;
    }

    Student(in(name, std::string), in(age, uint32_t)) {
        this->name = name;
        this->age = age;
    }
};

func change(inout(stud, type_u(Student))) {
    stud->age = 100;
    stud->name = "Bakk Andras";
}

func main(int argc, const char** argv)->int {
    let a = new (Student);
    let b = new_shared(Student, "Arnold", 21);
    change(a);
    ml::println(a->name);
    ml::println(a->age);
}