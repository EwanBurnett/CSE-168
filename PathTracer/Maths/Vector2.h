#ifndef __MATHS_VECTOR2_H
#define __MATHS_VECTOR2_H
/**
* @file Vector2.h
* @author Ewan Burnett (EwanBurnettSK@outlook.com)
* @date 2024-07-20
*/

#include <type_traits>
#include <cmath> 
#include "Utils.h"

namespace EDX {
    namespace Maths {
        /**
         * @brief A Two-Component contiguous Vector
         * @tparam T The type to contain within the vector. This is stored as a union, with {x, y}, {u, v} and array index members.
         * @note The size of a Vector2 is always sizeof(T) * 2.
         * @remark Data alignment is implementation defined.
        */
        template<typename T>
        struct Vector2 {
            Vector2(T X = static_cast<T>(0), T Y = static_cast<T>(0)) {
                this->x = X;
                this->y = Y;
            }

            union {
                struct { T x, y; };
                struct { T u, v; };
                T arr[2];
            };

            T& operator[](int idx) { return this->arr[idx]; };

            friend Vector2<T> operator -(Vector2<T> lhs) { return { -lhs.x, -lhs.y }; }

            friend Vector2<T> operator +(Vector2<T> lhs, const Vector2<T>& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y }; }
            friend Vector2<T> operator -(Vector2<T> lhs, const Vector2<T>& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y }; }
            friend Vector2<T> operator *(Vector2<T> lhs, const Vector2<T>& rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y }; }
            friend Vector2<T> operator /(Vector2<T> lhs, const Vector2<T>& rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y }; }

            friend Vector2<T> operator +(Vector2<T> lhs, const T& rhs) { return { lhs.x + rhs, lhs.y + rhs }; }
            friend Vector2<T> operator -(Vector2<T> lhs, const T& rhs) { return { lhs.x - rhs, lhs.y - rhs }; }
            friend Vector2<T> operator *(Vector2<T> lhs, const T& rhs) { return { lhs.x * rhs, lhs.y * rhs }; }
            friend Vector2<T> operator /(Vector2<T> lhs, const T& rhs) { return { lhs.x / rhs, lhs.y / rhs }; }


            friend Vector2<T> operator +(const T& lhs, Vector2<T> rhs) { return { rhs.x + lhs, rhs.y + lhs }; }
            friend Vector2<T> operator -(const T& lhs, Vector2<T> rhs) { return { rhs.x - lhs, rhs.y - lhs }; }
            friend Vector2<T> operator *(const T& lhs, Vector2<T> rhs) { return { rhs.x * lhs, rhs.y * lhs }; }
            friend Vector2<T> operator /(const T& lhs, Vector2<T> rhs) { return { rhs.x / lhs, rhs.y / lhs }; }

            inline Vector2& operator +=(const Vector2<T>& rhs) { this->x += rhs.x; this->y += rhs.y; return *this; }
            inline Vector2& operator -=(const Vector2<T>& rhs) { this->x -= rhs.x; this->y -= rhs.y; return *this; }
            inline Vector2& operator *=(const Vector2<T>& rhs) { this->x *= rhs.x; this->y *= rhs.y; return *this; }
            inline Vector2& operator /=(const Vector2<T>& rhs) { this->x /= rhs.x; this->y /= rhs.y; return *this; }

            inline Vector2& operator +=(const T& rhs) { this->x += rhs; this->y += rhs; return *this; }
            inline Vector2& operator -=(const T& rhs) { this->x -= rhs; this->y -= rhs; return *this; }
            inline Vector2& operator *=(const T& rhs) { this->x *= rhs; this->y *= rhs; return *this; }
            inline Vector2& operator /=(const T& rhs) { this->x /= rhs; this->y /= rhs; return *this; }

            friend bool operator ==(const Vector2<T>& lhs, const Vector2<T>& rhs) { return { lhs.x == rhs.x && lhs.y == rhs.y }; }
            friend bool operator !=(const Vector2<T>& lhs, const Vector2<T>& rhs) { return !(lhs == rhs); }

            /**
             * @brief Equivalent to Vector2<T>(0, 1)
             * @return A direction Vector pointing Upwards
            */
            inline constexpr Vector2<T> Up() const { return{ static_cast<T>(0.0), static_cast<T>(1.0) }; }

            /**
             * @brief Equivalent to Vector2<T>(0, -1)
             * @return A direction Vector pointing Down
            */
            inline constexpr Vector2<T> Down() const { return{ static_cast<T>(0.0), static_cast<T>(-1.0) }; }

            /**
             * @brief Equivalent to Vector2<T>(-1, 0)
             * @return A direction Vector pointing Left
            */
            inline constexpr Vector2<T> Left() const { return{ static_cast<T>(-1.0), static_cast<T>(0.0) }; }

            /**
             * @brief Equivalent to Vector2<T>(1, 0)
             * @return A direction Vector pointing Right
            */
            inline constexpr Vector2<T> Right() const { return{ static_cast<T>(1.0), static_cast<T>(0.0) }; }

            /**
             * @brief Computes the dot product of two vectors.
            */
            inline double Dot(const Vector2<T>& other) { return static_cast<double>((x * other.x) + (y * other.y)); }

            /**
             * @brief Computes the dot product of two vectors.
            */

            inline static double Dot(const Vector2<T>& a, const Vector2<T>& b) { return static_cast<double>((a.x * b.x) + (a.y * b.y)); }
            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline double Length() const { return sqrt((x * x) + (y * y)); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline static double Length(const Vector2<T>& vector) { return sqrt((vector.x * vector.x) + (vector.y * vector.y)); }

            /**
             * @brief Computes the squared length of a vector.
             * @return
            */
            inline double LengthSquared() const { return static_cast<double>((x * x) + (y * y)); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline static double LengthSquared(const Vector2<T>& vector) { return static_cast<double>((vector.x * vector.x) + (vector.y * vector.y)); }

            /**
             * @brief Returns the Normalized form of a vector, dividing each component by its length.
             * @return The normalized vector.
            */
            inline const Vector2<T> Normalize() { return Vector2<T>(*this / this->Length()); }

            /**
             * @brief Returns the Normalized form of a vector, dividing each component by its length.
             * @return The normalized vector.
            */
            inline static Vector2 Normalize(const Vector2<T>& vector) { return (vector / vector.Length()); }

            /**
             * @brief Sets each component of this Vector to a value.
             * @param val The value to set.
            */
            inline void Set(const T& val) { x = val; y = val; }

            /**
             * @brief Returns the Distance between two points
             * @param a
             * @param b
             * @return
            */
            inline static double Distance(const Vector2<T>& a, const Vector2<T>& b) { return sqrt(DistanceSquared(a, b)); };

            /**
             * @brief Returns the Squared Distance between two points
             * @param a
             * @param b
             * @return
            */
            inline static double DistanceSquared(const Vector2<T>& a, const Vector2<T>& b) { return static_cast<double>(((b.x - a.x) * (b.x - a.x)) + ((b.y - a.y) * (b.y - a.y))); };

            /**
             * @brief Linearly Interpolates between two Vectors
             * @param a
             * @param b
             * @param t
             * @return
            */
            inline static Vector2 Lerp(const Vector2<T>& a, const Vector2<T>& b, const double t) { return a + (t * (b - a)); };

            /**
             * @brief Spherically Interpolates between two Vectors
             * @param a
             * @param b
             * @param t
             * @return
            */
            static Vector2 Slerp(const Vector2<T>& a, const Vector2<T>& b, const double t) {
                double cosTheta = Dot(a, b);
                Clamp(cosTheta, -1.0, 1.0);   //Clamp the cosine to the range of ArcCos, to avoid floating point precision errors. 

                double theta = acos(cosTheta) * t;
                Vector2 v = Normalize(b - (a * cosTheta)); //Compute an intermediate vector

                return ((a * cos(theta)) + (v * sin(theta)));
            };
        };

        typedef Vector2<int> Vector2i;
        typedef Vector2<float> Vector2f;
        typedef Vector2<double> Vector2d;

        template struct Maths::Vector2<int>;
        template struct Maths::Vector2<float>;
        template struct Maths::Vector2<double>;

    }
}
#endif