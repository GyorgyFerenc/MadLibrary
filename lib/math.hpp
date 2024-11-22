#pragma once

#include <cmath>
#include "core.hpp"
#include "fmt.hpp"

#define PI 3.14159265359

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

    Vector2D<T> operator/(T el){
        return {
            .x = this->x / el,
            .y = this->y / el,
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

template <interface Out_Stream, class T>
usize write_fmt(Out_Stream stream, Vector2D<T> v){
    return write_fmt(stream, "(%, %)", v.x, v.y);
}

template <class T>
inline
T dot(Vector2D<T> v1, Vector2D<T> v2){
    return v1.x * v2.x + v1.y * v2.y;
}

template <class T>
inline
T len(Vector2D<T> v){
    return sqrt(dot(v, v));
}

template <class T>
inline
T distance(Vector2D<T> v1, Vector2D<T> v2){
    let v = v1 - v2;
    return len(v);
}

template <class T>
inline
Vector2D<T> normal(Vector2D<T> v){
    return {v.y, -v.x};
}

struct Line{
    v2 a;
    v2 b;
};

f32 distance(Line line, v2 point){
    let x0 = point.x;
    let y0 = point.y;
    let x1 = line.a.x;
    let y1 = line.a.y;
    let x2 = line.b.x;
    let y2 = line.b.y;
    return fabs((y2 - y1) * x0 - (x2 - x1) * y0 + x2 * y1 - y2 * x1) / distance(line.a, line.b);
}

struct Triangle{
    v2 a;
    v2 b;
    v2 c;
};

bool is_point_in_triangle(v2 point, v2 a, v2 b, v2 c){  
    // point = a  + w1 * (b - a) + w2 * (c - a)
    // w1 < 1, w2 < 1, 0 < w1 + w2 < 1 has to be true
    let aux = [](v2 p1, v2 p2, v2 p3){  
        return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
    };

    bool b1 = aux(point, a, b) < 0.0f;  
    bool b2 = aux(point, b, c) < 0.0f;
    bool b3 = aux(point, c, a) < 0.0f;
    return ((b1 == b2) && (b2 == b3));
}

bool inside(v2 point, Triangle t){  
    return is_point_in_triangle(point, t.a, t.b, t.c);
}
/* Check whether P and Q lie on the same side of line AB */
f32 same_side(v2f32 p, v2f32 q, v2f32 a, v2f32 b) {
    f32 z1 = (b.x - a.x) * (p.y - a.y) - (p.x - a.x) * (b.y - a.y);
    f32 z2 = (b.x - a.x) * (q.y - a.y) - (q.x - a.x) * (b.y - a.y);
    return z1 * z2;
}

bool intersect(Line line, Triangle triangle) {
    let p0 = line.a;
    let p1 = line.b;
    let t0 = triangle.a;
    let t1 = triangle.b;
    let t2 = triangle.c;

    /* Check whether segment is outside one of the three half-planes
     * delimited by the triangle. */
    f32 f1 = same_side(p0, t2, t0, t1), f2 = same_side(p1, t2, t0, t1);
    f32 f3 = same_side(p0, t0, t1, t2), f4 = same_side(p1, t0, t1, t2);
    f32 f5 = same_side(p0, t1, t2, t0), f6 = same_side(p1, t1, t2, t0);
    /* Check whether triangle is totally inside one of the two half-planes
     * delimited by the segment. */
    f32 f7 = same_side(t0, t1, p0, p1);
    f32 f8 = same_side(t1, t2, p0, p1);

    /* If segment is strictly outside triangle, or triangle is strictly
     * apart from the line, we're not intersecting */
    if ((f1 < 0 && f2 < 0) || (f3 < 0 && f4 < 0) || (f5 < 0 && f6 < 0)
          || (f7 > 0 && f8 > 0))
        return false;

    /* If segment is aligned with one of the edges, we're overlapping */
    if ((f1 == 0 && f2 == 0) || (f3 == 0 && f4 == 0) || (f5 == 0 && f6 == 0))
        return true;

    /* If segment is outside but not strictly, or triangle is apart but
     * not strictly, we're touching */
    if ((f1 <= 0 && f2 <= 0) || (f3 <= 0 && f4 <= 0) || (f5 <= 0 && f6 <= 0)
          || (f7 >= 0 && f8 >= 0))
        return true;

    /* If both segment points are strictly inside the triangle, we
     * are not intersecting either */
    if (f1 > 0 && f2 > 0 && f3 > 0 && f4 > 0 && f5 > 0 && f6 > 0)
        return false;

    /* Otherwise we're intersecting with at least one edge */
    return true;
}

f32 degrees_to_radian(f32 degrees) {
    return degrees * (PI / 180);
}

v2 rotate_radian(v2 p, f32 radian){
    let c = cosf(radian);
    let s = sinf(radian);
    return {
        .x = p.x * c + p.y * -1 * s,
        .y = p.x * s + p.y * c,
    };
}

v2 rotate_degrees(v2 p, f32 degrees){
    return rotate_radian(p, degrees_to_radian(degrees));
}

// t in [0, 1]
f32 lerp(f32 a, f32 b, f32 t){ return t * a + (1 - t) * b; }
// t in [0, 1]
f64 lerp(f64 a, f64 b, f64 t){ return t * a + (1 - t) * b; }

template <class T>
T clamp(T value, T smallest, T greatest){
    if (value <= smallest) return smallest;
    if (greatest <= value) return greatest;
    return value;
}
