#include "Plane.h"

EDX::Plane::Plane(Maths::Vector3f normal, float offset)
{
    m_Normal = normal.Normalize();
    m_Offset = offset;
}

bool EDX::Plane::Intersects(Ray ray, RayHit& hitResult)
{
    float vd = Maths::Vector3f::Dot(ray.Direction(), m_Normal);
    if (vd < 0.0) {   //Ray is Parallel to / Pointing away from the Plane. 
        return false;
    }

    float t = -(Maths::Vector3f::Dot(m_Normal , ray.Origin()) + m_Offset) / vd;

    if (t < 0.0f) { //Ray intersects the plane Behind the origin. 
        return false;
    }


    hitResult.point = ray.At(t);
    hitResult.t = t;
    hitResult.normal = m_Normal;

    return true;
}

void EDX::Plane::SetNormal(Maths::Vector3f normal)
{
    m_Normal = normal;
}

void EDX::Plane::SetOffset(float offset)
{
    m_Offset = offset;
}
