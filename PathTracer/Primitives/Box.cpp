#include "Box.h"
#include "Plane.h"
#include "Triangle.h"
#include "Sphere.h"

EDX::Box::Box()
{
    m_BoundsMin = { 0.0f, 0.0f, 0.0f };
    m_BoundsMax = { 0.0f, 0.0f, 0.0f };
}

EDX::Box::Box(Maths::Vector3f boundsMin, Maths::Vector3f boundsMax)
{
    //TODO: Coord Checking (assert(boundsMin < boundsMax))
    m_BoundsMin = boundsMin;
    m_BoundsMax = boundsMax;
}

bool EDX::Box::Intersects(Ray ray, RayHit& hitResult) const
{
    /*
    //Test intersection using the Ray Slope method
    Maths::Vector3f tMin = {}; 
    tMin.Set(-Maths::Infinity);

    Maths::Vector3f tMax = {}; 
    tMax.Set(Maths::Infinity);

    //Loop over each axis (xyz)
    for(int i = 0; i < 3; i++){
        float t_min = (tMin[i] - ray.Origin()[i]) / ray.Direction()[i]; 
        float t_max = (tMax[i] - ray.Origin()[i]) / ray.Direction()[i];

        if (t_min > t_max) {
            std::swap(t_min, t_max); 
        }


    }
    */


    Maths::Vector3f tMin = (m_BoundsMin - ray.Origin()) / ray.Direction();
    Maths::Vector3f tMax = (m_BoundsMax - ray.Origin()) / ray.Direction();

    //Intersect using Smits' Method
    {
        if ((tMin.x > tMax.y) || (tMin.y > tMax.x)) {
            return false; 
        }

        if (tMin.y > tMin.x) {
            tMin.x = tMin.y; 
        }
        if (tMax.y < tMax.x) {
            tMax.x = tMax.y; 
        }
    
        if ((tMin.x > tMax.z) || (tMin.z > tMax.x)) {
            return false; 
        }

        if (tMin.z > tMin.x) {
            tMin.x = tMin.z; 
        }
        if (tMax.z < tMax.x) {
            tMax.x = tMax.z;
        }

    }



    float t = std::min(std::min(tMin.x, tMin.y), tMin.z);
    if (t < 0.0f) {
        t = std::min(std::min(tMax.x, tMax.y), tMax.z);
        if (t < 0.0f) {
            return false;
        }
    }

    hitResult.t = t;
    hitResult.point = ray.At(t);

    return true;
}

void EDX::Box::SetMaterial(BlinnPhong material)
{
}

EDX::BlinnPhong EDX::Box::GetMaterial() const
{
    return BlinnPhong();
}



bool EDX::Box::Intersects(Sphere s)
{
    //Retrieve the sphere's bounds
    auto sphere_min = s.GetBoundsMin(); 
    auto sphere_max = s.GetBoundsMax(); 

    //Compare each axis

    if (sphere_min.x <= m_BoundsMax.x && sphere_max.x >= m_BoundsMin.x) {
        if (sphere_min.y <= m_BoundsMax.y && sphere_max.y >= m_BoundsMin.y) {
            if (sphere_min.z <= m_BoundsMax.z && sphere_max.z >= m_BoundsMin.z) {
                return true; 
            }
        }
    }

    return false;
}

bool EDX::Box::Intersects(Triangle t)
{
  //Retrieve the tri's bounds
    auto tri_min = t.GetBoundsMin(); 
    auto tri_max = t.GetBoundsMax(); 

    //Compare each axis

    if (tri_min.x <= m_BoundsMax.x && tri_max.x >= m_BoundsMin.x) {
        if (tri_min.y <= m_BoundsMax.y && tri_max.y >= m_BoundsMin.y) {
            if (tri_min.z <= m_BoundsMax.z && tri_max.z >= m_BoundsMin.z) {
                return true; 
            }
        }
    }

    return false;
}

