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
        Colour(float r_ = 0.0, float g_ = 0.0, float b_ = 0.0, float a_ = 1.0) : r(r_), g(g_), b(b_), a(a_) {}
        Colour(uint8_t r_ = 0, uint8_t g_ = 0, uint8_t b_ = 0, uint8_t a_ = 0xff) {
            r = (float)0xff / (float)r_; 
            g = (float)0xff / (float)g_; 
            b = (float)0xff / (float)b_; 
            a = (float)0xff / (float)a_; 
        }

        /**
         * @brief Returns a Gamma Corrected version of this colour
         * @param gamma Gamma value. Recommended to be between 0.01 and 4.0. 
        */
        Colour GammaCorrect(const float gamma) {
            r = powf(r, gamma); 
            g = powf(r, gamma); 
            b = powf(r, gamma); 
            a = powf(r, gamma); 
        }

        float r, g, b, a; 
    };
}

#endif