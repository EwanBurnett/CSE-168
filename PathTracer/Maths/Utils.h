#ifndef __MATHS_UTILS_H
#define __MATHS_UTILS_H
/**
* @file Utils.h
* @author Ewan Burnett (EwanBurnettSK@outlook.com)
* @date 2024-07-20
*/
#include <type_traits>
#include <limits>
#include <cmath>
#include <cfloat>
#include <algorithm>

namespace EDX {
    namespace Maths {
        //Constants
        constexpr double PI = 3.1415926535897932385;
        constexpr float Float_Max = std::numeric_limits<float>::max();
        constexpr float Float_Min = std::numeric_limits<float>::min();

        constexpr int Int_Max = std::numeric_limits<int>::max();
        constexpr int Int_Min = std::numeric_limits<int>::min();

        constexpr float Infinity = std::numeric_limits<float>::infinity();
        constexpr float Epsilon = FLT_EPSILON;

        constexpr double Deg2Rad = PI / 180.0;
        constexpr double Rad2Deg = 180.0 / PI;


        //Helpers

        /**
         * @brief Clamps a Value to a given range.
         * @tparam T Arithmetic type to Clamp
         * @param value The value to clamp.
         * @param min The minimum value of the range.
         * @param max The maximum value of the range.
         * @return The clamped value.
        */
        template <typename T>
        inline T Clamp(const T value, const T min, const T max) {

            T v = value;

            if (value < min) {
                v = min;
            }
            else if (value > max) {
                v = max;
            }

            return v;
        };

        /**
         * @brief Linearly Interpolates a value between a[0] and b[1].
         * @tparam T Arithmetic type to Lerp
         * @param a the start value, where t = 0.0
         * @param b the end value, where t = 1.0
         * @param t Interpolation constant
         * @return A linearly interpolated value.
        */
        template <typename T>
        inline T Lerp(const T& a, const T& b, const double t) {
            return a + (t * (b - a));
        }

        /**
         * @brief Converts Degrees to Radians.
         * @tparam T Arithmetic type to Convert.
         * @param degrees Angle in degrees to convert to Radians.
         * @return 'degrees' parameter in Radians.
        */
        template <typename T>
        inline constexpr T DegToRad(const T& degrees) {
            static_assert(std::is_arithmetic<T>(), "Error: T is not Arithmetic!\n");
            return static_cast<T>(degrees * Maths::Deg2Rad);
        }

        /**
         * @brief Converts Radians to Degrees.
         * @tparam T Arithmetic type to Convert.
         * @param radians Angle in Radians to convert to Degrees.
         * @return 'radians' parameter in Degrees.
        */
        template <typename T>
        inline constexpr T RadToDeg(const T& radians) {
            static_assert(std::is_arithmetic<T>(), "Error: T is not Arithmetic!\n");
            return radians * Maths::Rad2Deg;
        }


        /**
         * @brief Computes an approximation of the Inverse Square Root of a number.
         * @param number
         * @return The Inverse Square Root of a number.
         * @remark This is an implementation of the infamous Fast inverse square root algorithm found in Quake 3. This function is pure black magic.
        */
        inline float RSqrt(float number) {
            long i;
            float x2, y;
            const float threeHalfs = 1.5f;

            x2 = number * 0.5f;
            y = number;
            i = *(long*)&y;
            i = 0x5f3759df - (i >> 1);  //?????
            y = *(float*)&i;
            y = y * (threeHalfs - (x2 * y * y));

            return y;
        }


        inline bool SolveQuadratic(const float a, const float b, const float c, float& tmin, float& tmax) {

            const float discriminant = b * b - 4 * a * c;
            if (discriminant < 0.0) {
                return false; //There are no valid roots, so just return. 
            }

            float min = (-b - sqrtf(discriminant)) / 2.0f * a;
            float max = (-b + sqrtf(discriminant)) / 2.0f * a;

            if (min > max) {
                std::swap(tmin, tmax);
            }

            tmin = min; 
            tmax = max; 

            return true;
        }
    }
}

#endif
