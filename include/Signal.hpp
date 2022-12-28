#pragma once

#ifndef MADLIBRARY_SIGNAL
#define MADLIBRARY_SIGNAL

#include <functional>
#include <vector>

namespace MadLibrary {

template <class T>
struct Signal {};

template <class Return, class... Args>
struct Signal<Return(Args...)> {
    using Function = std::function<Return(Args...)>;

    /*
        Connect the function to be called when the signal is emited
    */
    void connect(Function func) {
        this->connected_slots.push_back(func);
    }

    /*
        Connect the member function of the object to be called when the signal is emited
    */
    template <class T>
    void connect(Return (T::*member_function)(Args...), T* obj) {
        auto function = [member_function, obj](Args... args) { (obj->*member_function)(args...); };
        this->connect(function);
    }

    /*
        Emits the signal aka. calls all the connected functions.
    */
    void emit(Args... args) {
        for (auto&& func : this->connected_slots) {
            func(args...);
        }
    }

   private:
    std::vector<Function> connected_slots;
};

}  // namespace MadLibrary

#endif