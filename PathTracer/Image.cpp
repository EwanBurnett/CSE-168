#include "Image.h"
#include "Maths/Vector4.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STBI_MSC_SECURE_CRT
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

EDX::Image::Image(const uint16_t width, const uint16_t height)
{
    //Initialize the Image Data. 
    m_ImageData.resize(width * height);
    m_Dimensions = { width, height };
}

uint32_t EDX::Image::Size() const
{
    return m_Dimensions.x * m_Dimensions.y; 
}

void EDX::Image::SetPixel(const uint16_t x, const uint16_t y, Colour colour)
{
    m_Lock.lock_shared();   //Take shared ownership of the mutex
    const uint32_t idx = y * m_Dimensions.x + x;
    m_ImageData[idx] = colour;
    m_Lock.unlock_shared(); 
}

EDX::Colour& EDX::Image::GetPixel(const uint16_t x, const uint16_t y)
{
    const uint32_t idx = y * m_Dimensions.x + x;
    return m_ImageData[idx];
}

uint32_t EDX::Image::Width() const
{
    return m_Dimensions.x; 
}

uint32_t EDX::Image::Height() const
{
    return m_Dimensions.y; 
}

void EDX::Image::Clear(const Colour colour)
{
    m_Lock.lock(); 
    for (auto& pixel : m_ImageData) {
        pixel = colour;
    }
    m_Lock.unlock(); 
}

const std::vector<EDX::Colour>& EDX::Image::GetPixels() const
{
    return m_ImageData;
}

std::shared_mutex& EDX::Image::Mutex()
{
    return m_Lock; 
}

void EDX::Image::ExportToPNG(const char* fileName, const float gamma) const
{
    //To export with STBI, the image must be converted into unsigned chars. 
    const uint32_t imageSize = m_ImageData.size();
    std::vector<Maths::Vector4<uint8_t>> blob(imageSize);

    for (uint32_t i = 0; i < imageSize; i++) {
        Colour c = m_ImageData[i].GammaCorrect(gamma);
        blob[i].r = static_cast<uint8_t>(255.99 * c.r);
        blob[i].g = static_cast<uint8_t>(255.99 * c.g);
        blob[i].b = static_cast<uint8_t>(255.99 * c.b);
        blob[i].a = static_cast<uint8_t>(255.99 * c.a);
    }

    stbi_write_png(fileName, m_Dimensions.x, m_Dimensions.y, 4, blob.data(), 0);
}

void EDX::Image::ExportToHDR(const char* fileName, const float gamma) const
{
    //Store a gamma-corrected copy of the image to output. 
    const uint32_t imageSize = m_ImageData.size();
    std::vector<Colour> blob(imageSize);

    for (uint32_t i = 0; i < imageSize; i++) {
        Colour c = m_ImageData[i].GammaCorrect(gamma);
        blob[i] = c; 
    }

    stbi_write_hdr(fileName, m_Dimensions.x, m_Dimensions.y, 4, (float*)blob.data());
}
