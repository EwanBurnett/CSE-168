#include "Scene.h"
#include "Maths/Utils.h"
#include <vector> 

EDX::Scene::Scene()
{

}

bool EDX::Scene::TraceRay(const Ray& r, RayHit& hitResult, Acceleration::Grid& grid) const
{
    //Trace the ray through each object in the scene. 
    RayHit result = {};
    float nearest = Maths::Infinity;
    uint32_t intersections = 0;

    {
        std::vector<RayHit> results = {}; 
        results.reserve(32); 
        grid.Traverse(r, results); 

        for (const auto& res : results) {
            if (res.t > 0.0f && res.t < nearest) {
                nearest = res.t; 
                intersections++; 
                result = res; 
            }
        }
    }
    
    //Planes are an "infinite" primitive; test intersection seperately.
    for (int i = 0; i < m_Planes.size(); i++)
    {
        //float nearest = Maths::Infinity;
        EDX::RayHit l_result = {};
        if (m_Planes[i].Intersects(r, l_result)) {
            if (nearest > l_result.t) {
                nearest = l_result.t;
                result = l_result;
                intersections++;
            }
        }
    }


    if (intersections <= 0) {
        return false;
    }


    if (result.t < 0.0f) {
        return false;
    }

    hitResult = result;
    return true;
}

std::vector<EDX::Plane>& EDX::Scene::Planes()
{
    return m_Planes;
}

std::vector<EDX::Triangle>& EDX::Scene::Triangles()
{
    return m_Triangles;
}

std::vector<EDX::Sphere>& EDX::Scene::Spheres()
{
    return m_Spheres;
}

std::vector<EDX::DirectionalLight>& EDX::Scene::DirectionalLights()
{
    return m_DirectionalLights;
}

std::vector<EDX::PointLight>& EDX::Scene::PointLights()
{
    return m_PointLights;
}
