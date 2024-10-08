#ifndef __SCENE_H
#define __SCENE_H
/**
 * @file Scene.h
 * @brief Scene Representation
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-30
*/
#include "Primitives/Plane.h"
#include "Primitives/Triangle.h"
#include "Primitives/Sphere.h"
#include "Lights/DirectionalLight.h"
#include "Lights/PointLight.h"
#include "Ray.h"
#include "RayHit.h"
#include <vector>
#include "Acceleration/Grid.h"

namespace EDX {

    class Scene {
    public: 
        Scene(); 

        bool TraceRay(const Ray& r, RayHit& hitResult, EDX::Acceleration::Grid& grid) const; 

        std::vector<Plane>& Planes(); 
        std::vector<Triangle>& Triangles(); 
        std::vector<Sphere>& Spheres(); 

        std::vector<DirectionalLight>& DirectionalLights();
        std::vector<PointLight>& PointLights();

    private:
        std::vector<Plane> m_Planes;
        std::vector<Triangle> m_Triangles;
        std::vector<Sphere> m_Spheres;

        std::vector<DirectionalLight> m_DirectionalLights;
        std::vector<PointLight> m_PointLights;
    };

}
#endif