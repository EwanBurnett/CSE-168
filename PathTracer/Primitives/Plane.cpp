#include "Plane.h"

EDX::Plane::Plane(Maths::Vector3f normal, Maths::Vector3f position)
{
    m_Normal = normal.Normalize();
    m_Position = position;

}

bool EDX::Plane::Intersects(Ray ray, RayHit& hitResult) const
{
    float n_dot_r = Maths::Vector3f::Dot(ray.Direction(), m_Normal);
    if (n_dot_r > Maths::Epsilon) {   //Ray is Parallel to / Pointing away from the Plane. 
        return false;
    }

    float d = Maths::Vector3f::Dot((m_Position - ray.Origin()), m_Normal);
    float t = d / n_dot_r;

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

