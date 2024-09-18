#ifndef __RAYTRACER_H
#define __RAYTRACER_H

#include "RenderData.h"
#include "Image.h"
#include "Ray.h"
#include "Colour.h"

namespace EDX {

    struct RenderSettings {
        uint32_t numThreads;
        Maths::Vector3i gridDim;
        Maths::Vector2i blockDim;
    };

    class RayTracer {
    public: 
        RayTracer(); 
        ~RayTracer(); 

        void Render(EDX::RenderData& renderData, EDX::Image& img);

        RenderSettings& Settings(); 

        static bool LoadSceneFile(const char* filePath, RenderData& renderData);
    private:
        static Colour RenderPixel(const uint32_t x, const uint32_t y, RenderData& renderData);
        static Colour RayColour(const Ray ray, uint32_t depth, RenderData& renderData);
        //static Maths::Vector3f OrientRay(const uint32_t x, const uint32_t y, const RenderData& renderData);

        RenderSettings m_Settings; 
        
    };
}


#endif