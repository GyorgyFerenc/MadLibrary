#pragma once

#include "core.hpp"

template <class T>
struct Vector2D{
    T x;
    T y;

    // Todo(Ferenc): Add += opertaor and so on
    Vector2D<T> operator+(Vector2D<T> v){
        return {
            .x = this->x + v.x,
            .y = this->y + v.y,
        };
    }

    Vector2D<T> operator-(Vector2D<T> v){
        return {
            .x = this->x - v.x,
            .y = this->y - v.y,
        };
    }

    Vector2D<T> operator-(){
        return {
            .x = -this->x,
            .y = -this->y,
        };
    }

    Vector2D<T> operator+(){
        return {
            .x = this->x,
            .y = this->y,
        };
    }

    Vector2D<T> operator*(T el){
        return {
            .x = this->x * el,
            .y = this->y * el,
        };
    }
};

using v2f32 = Vector2D<f32>;
using v2f64 = Vector2D<f64>;
using v2f = v2f32;
using v2  = v2f32;

using v2i64 = Vector2D<i64>;
using v2i32 = Vector2D<i32>;
using v2i16 = Vector2D<i16>;
using v2i8  = Vector2D<i8>;
using v2i   = Vector2D<int>;

using v2u64 = Vector2D<u64>;
using v2u32 = Vector2D<u32>;
using v2u16 = Vector2D<u16>;
using v2u8  = Vector2D<u8>;
using v2u   = Vector2D<uint>;


template <class T>
struct Vector3D{
    T x;
    T y;
    T z;

    // Todo(Ferenc): Add += opertaor and so on
    Vector3D<T> operator+(Vector3D<T> v){
        return {
            .x = this->x + v.x,
            .y = this->y + v.y,
            .z = this->z + v.z,
        };
    }

    Vector3D<T> operator-(Vector3D<T> v){
        return {
            .x = this->x - v.x,
            .y = this->y - v.y,
            .z = this->z - v.z,
        };
    }

    Vector3D<T> operator-(){
        return {
            .x = -this->x,
            .y = -this->y,
            .z = -this->z,
        };
    }

    Vector3D<T> operator+(){
        return {
            .x = +this->x,
            .y = +this->y,
            .z = +this->z,
        };
    }

    Vector3D<T> operator*(T el){
        return {
            .x = this->x * el,
            .y = this->y * el,
            .z = this->z * el,
        };
    }
};



using v3f32 = Vector3D<f32>;
using v3f64 = Vector3D<f64>;
using v3f = v3f32;
using v3  = v3f32;

using v3i64 = Vector3D<i64>;
using v3i32 = Vector3D<i32>;
using v3i16 = Vector3D<i16>;
using v3i8  = Vector3D<i8>;
using v3i   = Vector3D<int>;

using v3u64 = Vector3D<u64>;
using v3u32 = Vector3D<u32>;
using v3u16 = Vector3D<u16>;
using v3u8  = Vector3D<u8>;
using v3u   = Vector3D<uint>;
