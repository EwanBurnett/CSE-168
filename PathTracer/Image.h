#ifndef __IMAGE_H
#define __IMAGE_H
/**
 * @file Image.h
 * @brief Image Representation
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-26
*/

#include "Colour.h"
#include "Maths/Vector2.h"
#include <cstdint>
#include <vector> 

namespace EDX {
    class Image {
    public: 
        Image(const uint16_t width, const uint16_t height);

        void SetPixel(const uint16_t x, const uint16_t y, Colour colour); 
        Colour& GetPixel(const uint16_t x, const uint16_t y); 

        void Clear(const Colour colour); 

        void ExportToPNG(const char* fileName, const float gamma = 1.0f);
        void ExportToHDR(const char* fileName, const float gamma = 1.0f);
    private:
        std::vector<Colour> m_ImageData; 
        Maths::Vector2<uint16_t> m_Dimensions; 
    };
}
#endif