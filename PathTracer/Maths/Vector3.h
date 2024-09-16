#ifndef __MATHS_VECTOR3_H
#define __MATHS_VECTOR3_H
/**
* @file Vector3.h
* @brief Vector3 Helpers
* @author Ewan Burnett (EwanBurnettSK@outlook.com)
* @date 2024-07-20
*/
#include <type_traits>
#include <cmath> 
#include <cstdint>
#include "Utils.h"

namespace EDX {
    namespace Maths {
        /**
         * @brief A Three-Component contiguous Vector
         * @tparam T The type to contain within the vector. This is stored as a union, with {x, y, z}, {u, v, w} and array index members.
         * @note The size of a Vector3 is always sizeof(T) * 3.
         * @remark Data alignment is implementation defined.
        */
        template<typename T>
        struct Vector3 {
            Vector3(T X = static_cast<T>(0), T Y = static_cast<T>(0), T Z = static_cast<T>(0)) {
                static_assert(std::is_arithmetic<T>(), "Error: T is not Arithmetic!\n");

                x = X;
                y = Y;
                z = Z;
            }

            template<typename T1>
            Vector3(Vector3<T1> v) : x(v.x), y(v.y), z(v.z) {};

            union {
                struct { T x, y, z; };
                struct { T u, v, w; };
                struct { T r, g, b; };
                T arr[3];
            };

            T& operator[](int idx) { return this->arr[idx]; };

            friend Vector3<T> operator -(Vector3<T> lhs) { return { -lhs.x, -lhs.y, -lhs.z }; }

            friend Vector3<T> operator +(Vector3<T> lhs, const Vector3<T>& rhs) { return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z }; }
            friend Vector3<T> operator -(Vector3<T> lhs, const Vector3<T>& rhs) { return { lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z }; }
            friend Vector3<T> operator *(Vector3<T> lhs, const Vector3<T>& rhs) { return { lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z }; }
            friend Vector3<T> operator /(Vector3<T> lhs, const Vector3<T>& rhs) { return { lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z }; }

            friend Vector3<T> operator +(Vector3<T> lhs, const T& rhs) { return { lhs.x + rhs, lhs.y + rhs, lhs.z + rhs }; }
            friend Vector3<T> operator -(Vector3<T> lhs, const T& rhs) { return { lhs.x - rhs, lhs.y - rhs, lhs.z - rhs }; }
            friend Vector3<T> operator *(Vector3<T> lhs, const T& rhs) { return { lhs.x * rhs, lhs.y * rhs, lhs.z * rhs }; }
            friend Vector3<T> operator /(Vector3<T> lhs, const T& rhs) { return { lhs.x / rhs, lhs.y / rhs, lhs.z / rhs }; }

            friend Vector3<T> operator +(const T& lhs, Vector3<T> rhs ) { return { rhs.x + lhs, rhs.y + lhs, rhs.z + lhs }; }
            friend Vector3<T> operator -(const T& lhs, Vector3<T> rhs ) { return { rhs.x - lhs, rhs.y - lhs, rhs.z - lhs }; }
            friend Vector3<T> operator *(const T& lhs, Vector3<T> rhs ) { return { rhs.x * lhs, rhs.y * lhs, rhs.z * lhs }; }
            friend Vector3<T> operator /(const T& lhs, Vector3<T> rhs ) { return { rhs.x / lhs, rhs.y / lhs, rhs.z / lhs }; }

            inline Vector3& operator +=(const Vector3<T>& rhs) { this->x += rhs.x; this->y += rhs.y; this->z += rhs.z; return *this; }
            inline Vector3& operator -=(const Vector3<T>& rhs) { this->x -= rhs.x; this->y -= rhs.y; this->z -= rhs.z; return *this; }
            inline Vector3& operator *=(const Vector3<T>& rhs) { this->x *= rhs.x; this->y *= rhs.y; this->z *= rhs.z; return *this; }
            inline Vector3& operator /=(const Vector3<T>& rhs) { this->x /= rhs.x; this->y /= rhs.y; this->z /= rhs.z; return *this; }

            inline Vector3& operator +=(const T& rhs) { this->x += rhs; this->y += rhs; this->z += rhs; return *this; }
            inline Vector3& operator -=(const T& rhs) { this->x -= rhs; this->y -= rhs; this->z -= rhs; return *this; }
            inline Vector3& operator *=(const T& rhs) { this->x *= rhs; this->y *= rhs; this->z *= rhs; return *this; }
            inline Vector3& operator /=(const T& rhs) { this->x /= rhs; this->y /= rhs; this->z /= rhs; return *this; }

            friend bool operator ==(const Vector3<T>& lhs, const Vector3<T>& rhs) { return { lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z }; }
            friend bool operator !=(const Vector3<T>& lhs, const Vector3<T>& rhs) { return !(lhs == rhs); }


            /**
             * @brief Equivalent to Vector3<T>(0, 1, 0)
             * @return A direction Vector pointing Upwards
            */
            inline static constexpr Vector3<T> Up() { return{ static_cast<T>(0.0), static_cast<T>(1.0), static_cast<T>(0.0) }; }

            /**
             * @brief Equivalent to Vector3<T>(0, -1, 0)
             * @return A direction Vector pointing Upwards
            */
            inline static constexpr Vector3<T> Down() { return{ static_cast<T>(0.0), static_cast<T>(-1.0), static_cast<T>(0.0) }; }

            /**
             * @brief Equivalent to Vector3<T>(-1, 0, 0)
             * @return A direction Vector pointing Left
            */
            inline static constexpr Vector3<T> Left() { return{ static_cast<T>(-1.0), static_cast<T>(0.0), static_cast<T>(0.0) }; }

            /**
             * @brief Equivalent to Vector3<T>(1, 0, 0)
             * @return A direction Vector pointing Right
            */
            inline static constexpr Vector3<T> Right() { return{ static_cast<T>(1.0), static_cast<T>(0.0), static_cast<T>(0.0) }; }

            /**
             * @brief Equivalent to Vector3<T>(0, 0, 1)
             * @return A direction Vector pointing Forwards
            */
            inline static constexpr Vector3<T> Forwards() { return{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(1.0) }; }

            /**
             * @brief Equivalent to Vector3<T>(0, 0, -1)
             * @return A direction Vector pointing Backwards
            */
            inline static constexpr Vector3<T> Backwards() { return{ static_cast<T>(0.0), static_cast<T>(0.0), static_cast<T>(-1.0) }; }

            /**
             * @brief Computes the dot product of two vectors.
            */
            inline double Dot(const Vector3<T>& other) const { return (double)((x * other.x) + (y * other.y) + (z * other.z)); }

            /**
             * @brief Computes the dot product of two vectors.
            */
            inline static double Dot(const Vector3<T>& a, const Vector3<T>& b) { return static_cast<double>((a.x * b.x) + (a.y * b.y) + (a.z * b.z)); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline double Length() const { return sqrt(LengthSquared()); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline static double Length(const Vector3<T>& vector) { return sqrt(LengthSquared(vector)); }

            /**
             * @brief Computes the Squared Length of a Vector.
            */
            inline double LengthSquared() const { return static_cast<double>((x * x) + (y * y) + (z * z)); }

            /**
             * @brief Computes the Magnitude of a Vector.
            */
            inline static double LengthSquared(const Vector3<T>& vector) { return static_cast<double>((vector.x * vector.x) + (vector.y * vector.y) + (vector.z * vector.z)); }


            /**
             * @brief Returns the Normalized form of a vector, dividing each component by its length.
             * @return The normalized vector.
            */
            inline Vector3 Normalize() { return (*this / this->Length()); }

            /**
             * @brief Returns the Normalized form of a vector, dividing each component by its length.
             * @return The normalized vector.
            */
            inline static Vector3 Normalize(const Vector3<T>& vector) { return (vector / vector.Length()); }


            /**
             * @brief Computes the Cross product of two vectors.
            */
            inline Vector3 Cross(const Vector3<T>& other) {
                return { (y * other.z) - (z * other.y), (z * other.x) - (x * other.z) , (x * other.y) - (y * other.x) };
            };

            /**
             * @brief Computes the Cross product of two vectors.
            */
            inline static Vector3 Cross(const Vector3<T>& a, const Vector3<T>& b) {
                return { (a.y * b.z) - (a.z * b.y), (a.z * b.x) - (a.x * b.z), (a.x * b.y) - (a.y * b.x) };
            };


            /**
             * @brief Sets each component of this Vector to a value.
             * @param val The value to set.
            */
            inline void Set(const T& val) { x = val; y = val; z = val; }

            /**
             * @brief Sets each component of this Vector to a value.
             * @param val The value to set.
            */
            inline static void Set(Vector3<T>& vector, const T& val) { vector.x = val; vector.y = val; vector.z = val; }

            /**
             * @brief Returns the Distance between two points.
             * @param a
             * @param b
             * @return
            */
            inline static double Distance(const Vector3<T>& a, const Vector3<T>& b) { return sqrt(DistanceSquared(a, b)); };

            /**
             * @brief Returns the Squared Distance between two points.
             * @param a
             * @param b
             * @return
            */
            inline static double DistanceSquared(const Vector3<T>& a, const Vector3<T>& b) { return static_cast<double>(((b.x - a.x) * (b.x - a.x)) + ((b.y - a.y) * (b.y - a.y)) + ((b.z - a.z) * (b.z - a.z))); };

            /**
             * @brief Linearly Interpolates between two vectors.
             * @param a
             * @param b
             * @param t
             * @return
            */
            inline static Vector3 Lerp(const Vector3<T>& a, const Vector3<T>& b, const double t) { return a + (t * (b - a)); };

            /**
             * @brief Spherically Interpolates between two vectors.
             * @param a
             * @param b
             * @param t
             * @return
            */
            static Vector3 Slerp(const Vector3<T>& a, const Vector3<T>& b, const double t) {
                double cosTheta = Dot(a, b);
                cosTheta = Clamp(cosTheta, -1.0, 1.0);   //Clamp the cosine to the range of ArcCos, to avoid floating point precision errors. 

                double theta = acos(cosTheta) * t;
                Vector3 v = Normalize(b - (a * cosTheta)); //Compute an intermediate vector

                return ((a * cos(theta)) + (v * sin(theta)));
            };

            /**
             * @brief Orthonormalizes 3 Vectors using the Modified Gram-Schmidt Algorithm.
            */
            static void Orthonormalize(Vector3<T>& a, Vector3<T>& b, Vector3<T>& c) {
                Vector3<T> v[3] = { a, b, c };

                //Orthonormalize using the Modified Gram-Schmidt Algorithm 
                for (uint8_t i = 0; i < 3; i++) {
                    for (uint8_t j = 0; j < i; j++) {
                        const double f = static_cast<double>(v[j].Dot(v[i]) / v[j].Dot(v[j]));

                        for (uint8_t k = 0; k < 3; k++) {
                            v[i].arr[k] -= f * v[j].arr[k];
                        }
                    }
                }

                a = v[0].Normalize();
                b = v[1].Normalize();
                c = v[2].Normalize();
            }

        };

        typedef Vector3<int> Vector3i;
        typedef Vector3<float> Vector3f;
        typedef Vector3<double> Vector3d;
        template struct Maths::Vector3<int>;
        template struct Maths::Vector3<float>;
        template struct Maths::Vector3<double>;
    }

}
#endif