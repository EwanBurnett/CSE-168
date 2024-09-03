#include "Box.h"
#include "Plane.h"

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
    const Maths::Vector3f tMin = (m_BoundsMin - m_BoundsMax) / ray.Direction(); 
    const Maths::Vector3f tMax = (m_BoundsMax - m_BoundsMax) / ray.Direction(); 


    Maths::Vector3f t0 = {
        std::min(tMin.x, tMax.x),
        std::min(tMin.y, tMax.y),
        std::min(tMin.z, tMax.z),
    };

    Maths::Vector3f t1 = {
        std::max(tMin.x, tMax.x),
        std::max(tMin.y, tMax.y),
        std::max(tMin.z, tMax.z),
    };


    //Compare the ray against each Axis 
    if (t0.x > t1.x) {
        return false; 
    }

    else if (t0.y > t1.y) {
        return false; 
    }
    else if (t0.z > t1.z) {
        return false; 
    }

    return true;
}

void EDX::Box::SetMaterial(BlinnPhong material)
{
}

EDX::BlinnPhong EDX::Box::GetMaterial() const
{
    return BlinnPhong();
}
