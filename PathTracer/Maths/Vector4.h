#ifndef __MATHS_VECTOR4_H
#define __MATHS_VECTOR4_H

/**
* @file Vector4.h
* @author Ewan Burnett (EwanBurnettSK@outlook.com)
* @date 2024-07-20
*/

#include <type_traits>
#include <cmath> 
#include "Quaternion.h"

namespace EDX {
    namespace Maths {
        /**
         * @brief A Four-Component continuous Vector
         * @tparam T The type to contain within the vector. This is stored as a union, with {x, y, z, w} and array index members.
         * @note The size of a Vector4 is always sizeof(T) * 4.
         * @remark Data alignment is implementation defined.
        */
        template<typename T>
        struct Vector4 {

            Vector4(Vector3<T> vec, T W = static_cast<T>(0.0)) {
                x = vec.x;
                y = vec.y;
                z = vec.z;
                w = W; 
            }

            //Vector4(Quaternion q) : x(q.v.x), y(q.v.y), z(q.v.z), w(q.w) {};

            Vector4(T X = static_cast<T>(0), T Y = static_cast<T>(0), T Z = static_cast<T>(0), T W = static_cast<T>(0)) {
                x = X;
                y = Y;
                z = Z;
                w = W;
            }

            union {
                struct { T x, y, z, w; };
                struct { T r, g, b, a; };
                T arr[4];
            };

            T& operator[](int idx) { return this->arr[idx]; };

            friend Vector4<T> operator -(Vector4<T> lhs) { return { -lhs.x, -lhs.y, -lhs.z, -lhs.w }; }

            friend Vector4<T> operator +(Vector4<T> lhs, const Vector4<T>& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y ,lhs.z + rhs.z, lhs.w + rhs.w }; }
            friend Vector4<T> operator -(Vector4<T> lhs, const Vector4<T>& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y ,lhs.z - rhs.z, lhs.w - rhs.w }; }
            friend Vector4<T> operator *(Vector4<T> lhs, const Vector4<T>& rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y ,lhs.z * rhs.z, lhs.w * rhs.w }; }
            friend Vector4<T> operator /(Vector4<T> lhs, const Vector4<T>& rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y ,lhs.z / rhs.z, lhs.w / rhs.w }; }

            friend Vector4<T> operator +(Vector4<T> lhs, const T& rhs) { return { lhs.x + rhs, lhs.y + rhs, lhs.z + rhs, lhs.w + rhs }; }
            friend Vector4<T> operator -(Vector4<T> lhs, const T& rhs) { return { lhs.x - rhs, lhs.y - rhs, lhs.z - rhs, lhs.w - rhs }; }
            friend Vector4<T> operator *(Vector4<T> lhs, const T& rhs) { return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs }; }
            friend Vector4<T> operator /(Vector4<T> lhs, const T& rhs) { return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs }; }


            friend Vector4<T> operator +(const T& lhs, Vector4<T> rhs) { return { rhs.x + lhs, rhs.y + lhs, rhs.z + lhs, rhs.w + lhs }; }
            friend Vector4<T> operator -(const T& lhs, Vector4<T> rhs) { return { rhs.x - lhs, rhs.y - lhs, rhs.z - lhs, rhs.w - lhs }; }
            friend Vector4<T> operator *(const T& lhs, Vector4<T> rhs) { return { rhs.x * lhs, rhs.y * lhs, rhs.z * lhs, rhs.w * lhs }; }
            friend Vector4<T> operator /(const T& lhs, Vector4<T> rhs) { return { rhs.x / lhs, rhs.y / lhs, rhs.z / lhs, rhs.w / lhs }; }

            inline Vector4& operator +=(const Vector4<T>& rhs) { this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; this->w += rhs.w; return *this; }
            inline Vector4& operator -=(const Vector4<T>& rhs) { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; this->w -= rhs.w; return *this; }
            inline Vector4& operator *=(const Vector4<T>& rhs) { this->x *= rhs.x; this->y *= rhs.y; this->z *= rhs.z; this->w *= rhs.w; return *this; }
            inline Vector4& operator /=(const Vector4<T>& rhs) { this->x /= rhs.x; this->y /= rhs.y; this->z /= rhs.z; this->w /= rhs.w; return *this; }

            inline Vector4& operator +=(const T& rhs) { this->x += rhs; this->y += rhs; this->z += rhs; this->w += rhs; return *this; }
            inline Vector4& operator -=(const T& rhs) { this->x -= rhs; this->y -= rhs; this->z -= rhs; this->w -= rhs; return *this; }
            inline Vector4& operator *=(const T& rhs) { this->x *= rhs; this->y *= rhs; this->z *= rhs; this->w *= rhs; return *this; }
            inline Vector4& operator /=(const T& rhs) { this->x /= rhs; this->y /= rhs; this->z /= rhs; this->w /= rhs; return *this; }

            friend bool operator ==(const Vector4<T>& lhs, const Vector4<T>& rhs) { return { lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z && lhs.w == rhs.w }; }
            friend bool operator !=(const Vector4<T>& lhs, const Vector4<T>& rhs) { return !(lhs == rhs); }


            /**
             * @brief Computes the dot product of two vectors.
            */
            inline double Dot(const Vector4<T>& other) { return ((x * other.x) + (y * other.y) + (z * other.z) + (w * other.w)); }

            /**
             * @brief Computes the dot product of two vectors.
            */
            inline static double Dot(const Vector4<T>& a, const Vector4<T>& b) { return static_cast<double>((a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w)); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline double Length() const { return sqrt(LengthSquared()); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline static double Length(const Vector4<T>& vector) { return sqrt(LengthSquared(vector)); }

            /**
             * @brief Computes the Squared Length of a Vector.
            */
            inline double LengthSquared() const { return static_cast<double>((x * x) + (y * y) + (z * z) + (w * w)); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline static double LengthSquared(const Vector4<T>& vector) { return static_cast<double>((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z) + (vector.w * vector.w)); }

            /**
             * @brief Returns the Normalized form of a vector, dividing each component by its length.
             * @return The normalized vector.
            */
            inline Vector4 Normalize() { return (*this / this->Length()); }


            /**
             * @brief Returns the Normalized form of a vector, dividing each component by its length.
             * @return The normalized vector.
            */
            inline static Vector4 Normalize(const Vector4<T> vector) { return (vector / Length(vector)); }

            /**
             * @brief Sets each component of this Vector to a value.
             * @param val The value to set.
            */
            inline void Set(const T& val) { x = val; y = val; z = val; w = val; }

            /**
             * @brief Sets each component of this Vector to a value.
             * @param val The value to set.
            */
            inline static void Set(Vector4<T>& vector, const T& val) { vector.x = val; vector.y = val; vector.z = val; vector.w = val; }

        };

        typedef Vector4<int> Vector4i;
        typedef Vector4<float> Vector4f;
        typedef Vector4<double> Vector4d;

        template struct Maths::Vector4<int>;
        template struct Maths::Vector4<float>;
        template struct Maths::Vector4<double>;

    }
}
#endif