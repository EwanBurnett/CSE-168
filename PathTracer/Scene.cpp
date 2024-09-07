#include "Scene.h"
#include "Maths/Utils.h"

EDX::Scene::Scene()
{
 
}

bool EDX::Scene::TraceRay(const Ray& r, RayHit& hitResult, Acceleration::Grid& grid) const
{
    //Trace the ray through each object in the scene. 
    //TODO: Acceleration

    float nearest = Maths::Infinity;
    RayHit result = {}; 
    bool anyHit = false; 

    for (auto& cell : grid.GetCells())
    {
        RayHit cellHit = {};
        if (cell.bounds.Intersects(r, cellHit)) {
            for (uint64_t i = 0; i < cell.intersections.size(); i++) {
                EDX::RayHit l_result = {};
                if (cell.intersections[i]->Intersects(r, l_result)) {
                    if (nearest > l_result.t) {
                        nearest = l_result.t;
                        result = l_result;
                        anyHit = true;
                    }
                }
            }
        }
    }
    
    //Planes are an "infinite" primitive; test intersection seperately. 
    for (int i = 0; i < m_Planes.size(); i++)
    {
        EDX::RayHit l_result = {};
        if (m_Planes[i].Intersects(r, l_result)) {
            if (nearest > l_result.t) {
                nearest = l_result.t;
                result = l_result;
                anyHit = true;
            }
        }
    }
    
    /*
    for (int i = 0; i < m_Triangles.size(); i++)
    {
        EDX::RayHit l_result = {};
        if (m_Triangles[i].Intersects(r, l_result)) {
            if (nearest > l_result.t) {
                nearest = l_result.t;
                result = l_result;
                anyHit = true;
            }
        }
    }
    for (int i = 0; i < m_Spheres.size(); i++) {
        EDX::RayHit l_result = {};
        if (m_Spheres[i].Intersects(r, l_result)) {
            if (nearest > l_result.t) {
                nearest = l_result.t;
                result = l_result;
                anyHit = true;
            }
        }
    }
    */
    

    if (!anyHit) {
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
