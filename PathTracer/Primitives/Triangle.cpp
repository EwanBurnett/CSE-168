#include "Triangle.h"
#include "Plane.h"

EDX::Triangle::Triangle(Maths::Vector3f pointA, Maths::Vector3f pointB, Maths::Vector3f pointC)
{
    m_Type = EPrimitiveType::TRIANGLE;
    m_PointA = pointA;
    m_PointB = pointB;
    m_PointC = pointC;

    //m_Normal = EDX::Maths::Vector3f::Cross((m_PointB - m_PointA), (m_PointC - m_PointA)).Normalize();
}

bool EDX::Triangle::Intersects(Ray ray, RayHit& hitResult) const
{
    //Apply the Inverse of this primitive's transformation to the ray. 
    //TODO: compute this externally to the intersection test, to allow for simple instancing. 
    bool isInvertable = false;
    const Maths::Matrix4x4<float> inverseTransform = EDX::Maths::Matrix4x4<float>::Inverse(m_World, isInvertable);
    if (!isInvertable) {
        return false;
    }

    {
        Maths::Vector4f inv_ray_origin = { ray.Origin().x, ray.Origin().y, ray.Origin().z, 1.0f };
        Maths::Vector4f inv_ray_dir = { ray.Direction().x, ray.Direction().y, ray.Direction().z, 0.0f };


        inv_ray_origin = inv_ray_origin * inverseTransform;
        inv_ray_dir = inv_ray_dir * inverseTransform;
        Maths::Vector3f d = { inv_ray_dir.x, inv_ray_dir.y, inv_ray_dir.z };
        //d = d.Normalize();

        ray = Ray({ inv_ray_origin.x, inv_ray_origin.y, inv_ray_origin.z }, d);
    }

    
    //Compute intersection using the M�ller-Trumbore Algorithm

    Maths::Vector3f e1 = (m_PointB - m_PointA);
    Maths::Vector3f e2 = (m_PointC - m_PointA);

    Maths::Vector3f r_x_e2 = Maths::Vector3f::Cross(ray.Direction(), e2);

    float det = Maths::Vector3f::Dot(e1, r_x_e2);

    if (det < 0.0f) {
        return false; //Triangle is back-facing
    }
    if (std::fabs(det) < Maths::Epsilon) {
        return false;   //Ray is parallel to the triangle
    }

    float inv_det = 1.0f / det;

    Maths::Vector3f s = ray.Origin() - m_PointA;
    float u = inv_det * Maths::Vector3f::Dot(s, r_x_e2);

    if (u < 0.0f || u > 1.0f) {
        return false;
    }

    Maths::Vector3f s_x_e1 = Maths::Vector3f::Cross(s, e1);
    float v = inv_det * (Maths::Vector3f::Dot(ray.Direction(), s_x_e1));

    if ((v < 0.0f) || (u + v > 1.0f)) {
        return false;
    }

    float t = inv_det * Maths::Vector3f::Dot(e2, s_x_e1);


    if (t < 0.0f) {
        return false;
    }

    hitResult.t = t;
    {
        const Maths::Vector3f p = ray.At(t);
        Maths::Vector4f hit_point = { p.x, p.y, p.z, 1.0f };
        hit_point = hit_point * m_World;
        hitResult.point = { hit_point.x, hit_point.y, hit_point.z };
    }

    {
        const Maths::Matrix4x4<float> invTranspose = Maths::Matrix4x4<float>::Transpose(inverseTransform);

        Maths::Vector3f n = Maths::Vector3f::Cross(e1, e2).Normalize();
        Maths::Vector4f normal = { n.x, n.y, n.z, 0.0f };
        normal = normal * invTranspose;
        hitResult.normal = Maths::Vector3f::Normalize({ normal.x, normal.y, normal.z });
    }

    hitResult.pMat = const_cast<BlinnPhong*>(&m_Material);

    return true;

}

EDX::Maths::Vector3f EDX::Triangle::GetBoundsMin() const
{
    Maths::Vector3f min = {};

    min.x = std::min(m_PointA.x, std::min(m_PointB.x, m_PointC.x));
    min.y = std::min(m_PointA.y, std::min(m_PointB.y, m_PointC.y));
    min.z = std::min(m_PointA.z, std::min(m_PointB.z, m_PointC.z));

    auto transformed = EDX::Maths::Vector4f({ min.x, min.y, min.z, 1.0f }) * m_World;

    return { transformed.x, transformed.y, transformed.z };
}

EDX::Maths::Vector3f EDX::Triangle::GetBoundsMax() const
{
    Maths::Vector3f max = {};

    max.x = std::max(m_PointA.x, std::max(m_PointB.x, m_PointC.x));
    max.y = std::max(m_PointA.y, std::max(m_PointB.y, m_PointC.y));
    max.z = std::max(m_PointA.z, std::max(m_PointB.z, m_PointC.z));

    auto transformed = EDX::Maths::Vector4f({ max.x, max.y, max.z, 1.0f }) * m_World;

    return { transformed.x, transformed.y, transformed.z };
}
