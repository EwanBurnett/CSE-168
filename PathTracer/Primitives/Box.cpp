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

    if (tMin.x > tMax.x) {
        std::swap(tMin.x, tMax.x);
    }
    if (tMin.y > tMax.y) {
        std::swap(tMin.y, tMax.y);
    }
    if (tMin.z > tMax.z) {
        std::swap(tMin.z, tMax.z);
    }

    float tNear = std::max(std::max(tMin.x, tMin.y), tMin.z);
    float tFar = std::min(std::min(tMax.x, tMax.y), tMax.z);

    if (tNear > tFar) {
        return false;
    }
    float t = tNear;
    if (t < 0.0f) {
        t = tMax.x;
        if (t < 0.0f) {
            return false;
        }
    }

    hitResult.t = t;
    hitResult.point = ray.At(t);

    return true;

    /*
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



    float t = tMin.x;
    /*
    if (t < 0.0f) {
        t = tMax.x;
        if (t < 0.0f) {
            return false;
        }
    }

    hitResult.t = t;
    hitResult.point = ray.At(t);
    */

    return true;
}

void EDX::Box::SetMaterial(BlinnPhong material)
{
}

EDX::BlinnPhong EDX::Box::GetMaterial() const
{
    return BlinnPhong();
}



bool EDX::Box::Intersects(EDX::Box b) const
{
    //Compare each axis
    const auto& a_min = m_BoundsMin; 
    const auto& a_max = m_BoundsMax; 

    const auto& b_min = b.m_BoundsMin;
    const auto& b_max = b.m_BoundsMax;

    if (b_min.x <= m_BoundsMax.x || b_max.x >= m_BoundsMin.x) {
        if (b_min.y <= m_BoundsMax.y || b_max.y >= m_BoundsMin.y) {
            if (b_min.z <= m_BoundsMax.z || b_max.z >= m_BoundsMin.z) {
                return true;
            }
        }
    }

    return false;
}
