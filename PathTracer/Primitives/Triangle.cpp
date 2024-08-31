#include "Triangle.h"
#include "Plane.h"

EDX::Triangle::Triangle(Maths::Vector3f pointA, Maths::Vector3f pointB, Maths::Vector3f pointC)
{
    m_PointA = pointA;
    m_PointB = pointB;
    m_PointC = pointC;

    m_Normal = EDX::Maths::Vector3f::Cross((m_PointB - m_PointA), (m_PointC - m_PointA)).Normalize();
}

bool EDX::Triangle::Intersects(Ray ray, RayHit& hitResult) const
{
    Plane p(m_Normal, m_PointA);

    RayHit plane_hit = {};
    if (!p.Intersects(ray, plane_hit)) {
        return false;
    }

    //Compute Barycentric Coordinates using Cramer's rule.

    Maths::Vector3f bary_coords = {};
    {

        Maths::Vector3f v0 = m_PointB - m_PointA;
        Maths::Vector3f v1 = m_PointC - m_PointA;
        Maths::Vector3f v2 = plane_hit.point - m_PointA;

        float d00 = Maths::Vector3f::Dot(v0, v0);
        float d01 = Maths::Vector3f::Dot(v0, v1);
        float d11 = Maths::Vector3f::Dot(v1, v1);
        float d20 = Maths::Vector3f::Dot(v2, v0);
        float d21 = Maths::Vector3f::Dot(v2, v1);

        float denom = d00 * d11 - d01 * d01;

        bary_coords.x = (d11 * d20 - d01 * d21) / denom;
        bary_coords.z = (d00 * d21 - d01 * d20) / denom;
        bary_coords.y = 1.0f - bary_coords.x - bary_coords.z;
    }

    if (!(bary_coords.x >= 0.0f && bary_coords.y >= 0.0f && (bary_coords.x + bary_coords.y) <= 1.0f))
    {
        return false;
    }

    hitResult = plane_hit;
    hitResult.pMat = const_cast<BlinnPhong*>(&m_Material);

    return true;
}

void EDX::Triangle::SetMaterial(BlinnPhong material)
{
    m_Material = material;
}

EDX::BlinnPhong EDX::Triangle::GetMaterial() const
{
    return m_Material;
}
