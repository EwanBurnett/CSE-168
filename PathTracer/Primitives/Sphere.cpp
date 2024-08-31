#include "Sphere.h"
#include "../Maths/Utils.h"

EDX::Sphere::Sphere(Maths::Vector3f position, float radius)
{
    m_Position = position;
    m_Radius = radius;
}

bool EDX::Sphere::Intersects(Ray ray, RayHit& hitResult)
{
    //Solve the Quadratic to determine if the ray intersects with the sphere. 
    const EDX::Maths::Vector3f toCenter = m_Position - ray.Origin();

    const float a = EDX::Maths::Vector3f::Dot(ray.Direction(), ray.Direction());
    const float b = -2.0f * EDX::Maths::Vector3f::Dot(ray.Direction(), toCenter);
    const float c = EDX::Maths::Vector3f::Dot(toCenter, toCenter) - (m_Radius * m_Radius);

    float tmin;
    float tmax;

    if (!Maths::SolveQuadratic(a, b, c, tmin, tmax)) {
        return false;
    }

    if (tmin > 0.0f) {
        hitResult.t = tmin;
        hitResult.point = ray.At(tmin);
        hitResult.normal = (hitResult.point - m_Position).Normalize();// / m_Radius;
        return true;
    }

    return false;

}

void EDX::Sphere::SetPosition(Maths::Vector3f position)
{
    m_Position = position;
}

void EDX::Sphere::SetRadius(float radius)
{
    m_Radius = radius;
}
