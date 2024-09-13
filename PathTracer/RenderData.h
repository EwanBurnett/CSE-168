#ifndef __RENDERDATA_H
#define __RENDERDATA_H

#include <string>
#include "Maths.h"
#include "Camera.h"
#include "Acceleration/Grid.h"
#include "Scene.h"

namespace EDX {
    struct RenderData {
        std::string outputName;
        Maths::Vector2<uint16_t> dimensions;
        Maths::Vector2<float> FoV;
        EDX::Camera camera;
        Scene scene;
        uint32_t maxDepth = 1;
        EDX::Acceleration::Grid accelGrid; 
    };
}
#endif