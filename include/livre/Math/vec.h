#pragma once

namespace livre
{
    template<typename T>
    struct Vec3
    {
        T x, y, z;

        Vec3<T>() : 
            Vec3<T>(0, 0, 0)
        {

        }

        Vec3<T>(T _x, T _y, T _z) :
            x(_x), y(_y), z(_z)
        {   }

        Vec3<T> operator+(const Vec3<T>& v)
        {
            return { x + v.x, y + v.y, z + v.z };
        }

        Vec3<T> operator*(T f)
        {
            return { x * f, y * f, z * f };
        }

        Vec3<T> operator-(const Vec3<T>& v)
        {
            return this + (-1.f * v);
        }
    };


    template<typename T>
    struct Vec2
    {
        T x, y;

        Vec2<T>() : 
            Vec2<T>(0, 0, 0)
        {

        }

        Vec2<T>(T _x, T _y) :
            x(_x), y(_y)
        {   }

        Vec2<T> operator+(const Vec2<T>& v)
        {
            return { x + v.x, y + v.y };
        }

        Vec2<T> operator*(T f)
        {
            return { x * f, y * f };
        }

        Vec2<T> operator-(const Vec2<T>& v)
        {
            return this + (-1.f * v);
        }
    };

    typedef Vec3<float> Vec3f;
    typedef Vec2<float> Vec2f;
}