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

    //TODO: 3D DDA Grid Traversal
    for (auto& cell : grid.GetCells())
    {
        RayHit cellHit = {};
        if (cell.bounds.Intersects(r, cellHit)) {
            for (uint64_t i = 0; i < cell.intersections.size(); i++) {

                EDX::Primitive* pPrimitive = cell.intersections[i];

                //if (std::find(intersections.begin(), intersections.end(), pPrimitive) == intersections.end()) {
                    EDX::RayHit l_result = {};
                    bool intersects = pPrimitive->Intersects(r, l_result);

                    /*
                    switch (pPrimitive->GetType()) {
                    case EDX::Primitive::EPrimitiveType::SPHERE:
                        intersects = cell.intersections[i]->Intersects(r, l_result);
                        break;
                    case EDX::Primitive::EPrimitiveType::TRIANGLE:
                        intersects = cell.intersections[i]->Intersects(r, l_result);
                        break;
                    case EDX::Primitive::EPrimitiveType::PLANE:
                        intersects = cell.intersections[i]->Intersects(r, l_result);
                        break;
                    case EDX::Primitive::EPrimitiveType::NONE:
                    default:
                        break;
                    }
                    */
                    if (intersects) {
                        if (l_result.t > 0.0f && l_result.t < nearest) {
                            nearest = l_result.t;
                            l_result.pMat = pPrimitive->GetMaterial();
                            intersections++; 
                            result = l_result; 
                        }
                    }
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
