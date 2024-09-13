#include "Sphere.h"
#include "../Maths/Utils.h"

EDX::Sphere::Sphere(Maths::Vector3f position, float radius)
{
    m_Position = position;
    m_Radius = radius;
}

bool EDX::Sphere::Intersects(Ray ray, RayHit& hitResult) const
{
    //Apply the Inverse of this primitive's transformation to the ray. 
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
        d = d.Normalize();

        ray = Ray({ inv_ray_origin.x, inv_ray_origin.y, inv_ray_origin.z }, d);
    }

    //Solve the Quadratic to determine if the ray intersects with the sphere. 
    const Maths::Vector3f toCenter = m_Position - ray.Origin();

    const float a = Maths::Vector3f::Dot(ray.Direction(), ray.Direction());
    const float b = -2.0f * Maths::Vector3f::Dot(ray.Direction(), toCenter);
    const float c = Maths::Vector3f::Dot(toCenter, toCenter) - (m_Radius * m_Radius);

    float tmin;
    float tmax;

    if (!Maths::SolveQuadratic(a, b, c, tmin, tmax)) {
        return false;
    }

    float t = tmin;
    if (t < 0.0f) {
        t = tmax;
        if (t < 0.0f) {
            return false;
        }
    }
    hitResult.t = t;
    const Maths::Vector3f p = ray.At(t);
    //Compute transformed intersection point
    {
        Maths::Vector4f hit_point = { p.x, p.y, p.z, 1.0f };
        hit_point = hit_point * m_World;
        hitResult.point = { hit_point.x, hit_point.y, hit_point.z };
    }

    //Compute transformed intersection normal by applying the inverse-transpose of the world matrix. 
    {
        const Maths::Matrix4x4<float> invTranspose = Maths::Matrix4x4<float>::Transpose(inverseTransform);

        Maths::Vector3f n = (p - m_Position).Normalize();
        Maths::Vector4f normal = { n.x, n.y, n.z, 0.0f };
        normal = normal * invTranspose;
        hitResult.normal = Maths::Vector3f::Normalize({ normal.x, normal.y, normal.z });
    }
    hitResult.pMat = const_cast<BlinnPhong*>(&m_Material);
    return true;
}

EDX::Maths::Vector3f EDX::Sphere::GetPosition() const
{
    return m_Position;
}

void EDX::Sphere::SetPosition(Maths::Vector3f position)
{
    m_Position = position;
}

float EDX::Sphere::GetRadius() const
{
    return m_Radius;
}

void EDX::Sphere::SetRadius(float radius)
{
    m_Radius = radius;
}

EDX::Maths::Vector3f EDX::Sphere::GetBoundsMin() const
{
    EDX::Maths::Vector3f min = {};

    min.x = m_Position.x - m_Radius;
    min.y = m_Position.y - m_Radius;
    min.z = m_Position.z - m_Radius;

    auto transformed = EDX::Maths::Vector4f({ min.x, min.y, min.z, 1.0f }) * m_World;

    return { transformed.x, transformed.y, transformed.z };
}

EDX::Maths::Vector3f EDX::Sphere::GetBoundsMax() const
{
    EDX::Maths::Vector3f max = {};

    max.x = m_Position.x + m_Radius;
    max.y = m_Position.y + m_Radius;
    max.z = m_Position.z + m_Radius;

    auto transformed = EDX::Maths::Vector4f({ max.x, max.y, max.z, 1.0f }) * m_World;

    return { transformed.x, transformed.y, transformed.z };
}
