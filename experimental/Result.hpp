#pragma once

#include <functional>
#include <iostream>
#include <string>
#include <variant>

#define RETURN_ON_ERROR(result) \
    if (result.is_error()) return result;

// writes the message to cerr and exits the program
void panic(const char* message) {
    std::cerr << "Paniced!\n" << message;
    exit(-1);
}

template <class T, class E>
struct Result {
    // Creating an error from error
    static Result<T, E> error(E error) {
        Result<T, E> res{};
        res.value = error;
        return res;
    }

    // Creating an ok value from datas
    static Result<T, E> ok(T data) {
        Result<T, E> res{};
        res.value = data;
        return res;
    }

    bool is_error() const {
        return this->value.index() == 1;
    }

    bool is_ok() const {
        return !this->is_error();
    }

    // Gets the value if it is ok panic otherwise
    T unwrap() {
        if (!is_ok()) {
            panic("Result: There is an error present!");
        }
        return std::get<T>(value);
    }

    // Gets the value if it is ok returns the data otherwise
    T unwrap_or(T data) {
        if (!is_ok()) {
            return data;
        }

        return this->unwrap();
    }

    // Gets the value if it is ok calls the function otherwise
    T unwrap_or_else(std::function<T(E)> f) {
        if (!is_ok()) {
            return f(this->get_error());
        }

        return this->unwrap();
    }

    // Gets the error if it is error panics otherwise
    E get_error() const {
        if (!is_error()) {
            panic("Result: No error present!");
        }

        return std::get<E>(value);
    }

    // If the value is ok calls the function on it returns
    // error as a result otherwise
    Result<T, E> and_then(std::function<Result<T, E>(T)> f) {
        if (this->is_error()) {
            return *this;
        }
        return f(this->unwrap());
    }

    // If the value is not ok calls the function on it returns
    // ok value as a result otherwise
    Result<T, E> or_else(std::function<Result<T, E>(E)> f) {
        if (this->is_ok()) {
            return *this;
        }
        return f(this->get_error());
    }

    // Converts the result to another type if it holds an error,
    // panics otherwise
    template <class NewT>
    operator Result<NewT, E>() const {
        if (!is_error()) {
            panic("Result: No error present!");
        }

        auto message = this->get_message();
        auto error = this->get_error();
        return Result<NewT, E>::error(error, message);
    }

   private:
    std::variant<T, E> value;
};
