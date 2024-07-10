#pragma once

#include <cmath>
#include "core.hpp"
#include "fmt.hpp"

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

using v2usize = Vector2D<usize>;
using v2isize = Vector2D<isize>;


template <class T>
inline
T dot(Vector2D<T> v1, Vector2D<T> v2){
    return v1.x * v2.x + v1.y * v2.y;
}

template <class T>
inline
T distance(Vector2D<T> v1, Vector2D<T> v2){
    let v = v1 - v2;
    return sqrt(dot(v, v));
}



template <interface Out_Stream, class T>
usize write_fmt(Out_Stream stream, Vector2D<T> v){
    return write_fmt(stream, "(%, %)", v.x, v.y);
}


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










/*
   Util
*/

template<class T>
struct Triangle{
    T a;
    T b;
    T c;
};

template<class T>
bool is_point_in_triangle(Vector2D<T> point, Vector2D<T> a, Vector2D<T> b, Vector2D<T> c){  
    // point = a  + w1 * (b - a) + w2 * (c - a)
    // w1 < 1, w2 < 1, 0 < w1 + w2 < 1 has to be true
    let aux = [](Vector2D<T> p1, Vector2D<T> p2, Vector2D<T> p3){  
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };

    bool b1 = aux(point, a, b) < 0.0f;  
    bool b2 = aux(point, b, c) < 0.0f;
    bool b3 = aux(point, c, a) < 0.0f;
    return ((b1 == b2) && (b2 == b3));
}

template<class T>
bool is_point_in_triangle(T point, Triangle<T> t){  
    return is_point_in_triangle(point, t.a, t.b, t.c);
}
// t in [0, 1]
f32 lerp(f32 a, f32 b, f32 t){ return t * a + (1 - t) * b; }
// t in [0, 1]
f64 lerp(f64 a, f64 b, f64 t){ return t * a + (1 - t) * b; }
