#include <exception>
#include <iostream>
#include <tuple>
#include <variant>

#include "include/MadLibrary.hpp"

struct a {
    int          a = 1;
    virtual void asd() {
        ml::println("a");
    }
};

struct b : public a {
    int          b = 2;
    virtual void asd() {
        ml::println("b");
    }
};

template <class T, class poly_T>
struct reference_ptr;

template <class T>
struct var_ptr {
    var_ptr(T* data) {
        this->data = data;
    }
    ~var_ptr() {
        if (this->data = nullptr) {
            return;
        }
        delete this->data;
    }

    T* release() {
        auto temp = this->data;
        this->data = nullptr;
        return temp;
    }
    constexpr var_ptr(var_ptr<T>&& ptr) {
        this->release();
        this->data = ptr.release();
    }
    constexpr var_ptr<T>& operator=(var_ptr<T>&& ptr) {
        this->release();
        this->data = ptr.release();
        return this;
    }

    template <class to_T>
    operator reference_ptr<T, to_T>() {
        reference_ptr<T, to_T> temp{*this, (to_T*)this->data};
        return temp;
    }

    T* data;
};

template <class T, class poly_T>
struct reference_ptr {
    var_ptr<T>& ptr;
    poly_T*     data;

   private:
    friend class var_ptr<T>;
    reference_ptr(var_ptr<T>& _ptr, poly_T* _data) : ptr{_ptr}, data{_data} {
    }
};

int main() {
    // std::unique_ptr<b> balma{new b};
    // std::unique_ptr<a> alma = std::move(balma);
    // alma->asd();
    var_ptr<b>          basd{new b};
    reference_ptr<b, a> asd = basd;
    asd.data->asd();
}

/*

func f(in a: object)
       ^^^^-- a can only be read
           -- a can be polymorphic

func f(inout a: object)
       ^^^^^^^-- a can be read
              -- a can be modified, (expected)
              -- a can be polymorphic
              -- the ownership of the resource can be moved

func f(out a: object)
       ^^^^^-- a must be modified







*/