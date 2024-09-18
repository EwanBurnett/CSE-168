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
    m_BoundsMin = boundsMin;
    m_BoundsMax = boundsMax;

    for (int i = 0; i < 3; i++) {
        if (m_BoundsMin[i] > m_BoundsMax[i]) {
            std::swap(m_BoundsMin[i], m_BoundsMax[i]); 
        }
    }
}

bool EDX::Box::Intersects(Ray ray, RayHit& hitResult) const
{
    return Intersects(ray, m_BoundsMin, m_BoundsMax, hitResult); 
}

void EDX::Box::SetMaterial(BlinnPhong material)
{
}

EDX::BlinnPhong EDX::Box::GetMaterial() const
{
    return BlinnPhong();
}

EDX::Maths::Vector3f EDX::Box::GetBoundsMin() const
{
    return m_BoundsMin;
}

EDX::Maths::Vector3f EDX::Box::GetBoundsMax() const
{
    return m_BoundsMax;
}



bool EDX::Box::Intersects(EDX::Box b) const
{
    //Compare each axis

    const auto& a_min = m_BoundsMin;
    const auto& a_max = m_BoundsMax;

    const auto& b_min = b.m_BoundsMin; 
    const auto& b_max = b.m_BoundsMax; 

    if (a_min.x <= b_max.x && a_max.x >= b_min.x) {
        if (a_min.y <= b_max.y && a_max.y >= b_min.y) {
            if (a_min.z <= b_max.z && a_max.z >= b_min.z) {
                return true;
            }
        }
    }

    return false;
}

bool EDX::Box::Intersects(Ray ray, Maths::Vector3f boundsMin, Maths::Vector3f boundsMax, RayHit& hitResult) {
    Maths::Vector3f tMin = (boundsMin - ray.Origin()) / ray.Direction();
    Maths::Vector3f tMax = (boundsMax - ray.Origin()) / ray.Direction();

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
        t = tFar;
        if (t < 0.0f) {
            return false;
        }
    }

    hitResult.t = t;
    hitResult.point = ray.At(t);

    return true;
}
