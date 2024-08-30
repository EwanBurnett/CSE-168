#ifndef __COLOUR_H
#define __COLOUR_H
/**
 * @file Colour.h
 * @brief RGBA Colour Representation
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-26
*/
#include <cstdint>
#include <cmath> 
namespace EDX {
    /**
     * @brief Represents a RGBA unorm colour (bounded [0..1])
    */
    class Colour {
    public:
        Colour() : r(0.0), g(0.0), b(0.0), a(1.0) {};
        Colour(float r_, float g_, float b_, float a_) : r(r_), g(g_), b(b_), a(a_) {}
        Colour(int r_, int g_, int b_, int a_) {
            r = (float)r_ / (float)0xff;
            g = (float)g_ / (float)0xff;
            b = (float)b_ / (float)0xff;
            a = (float)a_ / (float)0xff;
        }

        friend Colour operator +(Colour lhs, const Colour& rhs) { return { lhs.r + rhs.r, lhs.g + rhs.g ,lhs.b + rhs.b, lhs.a + rhs.a }; }
        friend Colour operator -(Colour lhs, const Colour& rhs) { return { lhs.r - rhs.r, lhs.g - rhs.g ,lhs.b - rhs.b, lhs.a - rhs.a }; }
        friend Colour operator *(Colour lhs, const Colour& rhs) { return { lhs.r * rhs.r, lhs.g * rhs.g ,lhs.b * rhs.b, lhs.a * rhs.a }; }
        friend Colour operator /(Colour lhs, const Colour& rhs) { return { lhs.r / rhs.r, lhs.g / rhs.g ,lhs.b / rhs.b, lhs.a / rhs.a }; }


        friend Colour operator +(Colour lhs, const float& rhs) { return { lhs.r + rhs, lhs.g + rhs, lhs.b + rhs, lhs.a + rhs }; }
        friend Colour operator -(Colour lhs, const float& rhs) { return { lhs.r - rhs, lhs.g - rhs, lhs.b - rhs, lhs.a - rhs }; }
        friend Colour operator *(Colour lhs, const float& rhs) { return { lhs.r * rhs, lhs.g * rhs, lhs.b * rhs, lhs.a * rhs }; }
        friend Colour operator /(Colour lhs, const float& rhs) { return { lhs.r / rhs, lhs.g / rhs, lhs.b / rhs, lhs.a / rhs }; }

        /**
         * @brief Returns a Gamma Corrected version of this colour
         * @param gamma Gamma value. Recommended to be between 0.01 and 4.0.
        */
        inline Colour GammaCorrect(const float gamma) {
            Colour out = {};
            out.r = powf(r, gamma);
            out.g = powf(g, gamma);
            out.b = powf(b, gamma);
            out.a = powf(a, gamma);

            return out;
        }

        float r, g, b, a;
    };
}

#endif