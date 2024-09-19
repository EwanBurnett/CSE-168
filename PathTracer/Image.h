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
#include <shared_mutex>

namespace EDX {
    class Image {
    public: 
        Image(const uint16_t width, const uint16_t height);

        uint32_t Size() const; 
        void SetPixel(const uint16_t x, const uint16_t y, Colour colour); 
        Colour& GetPixel(const uint16_t x, const uint16_t y); 

        uint32_t Width() const; 
        uint32_t Height() const; 

        void Clear(const Colour colour); 

        const std::vector<Colour>& GetPixels() const; 
        std::shared_mutex& Mutex(); 

        void ExportToPNG(const char* fileName, const float gamma = 1.0f) const;
        void ExportToHDR(const char* fileName, const float gamma = 1.0f) const;
    private:
        std::vector<Colour> m_ImageData; 
        Maths::Vector2<uint16_t> m_Dimensions; 
        std::shared_mutex m_Lock; 
    };
}
#endif