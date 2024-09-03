#include "Plane.h"

EDX::Plane::Plane(Maths::Vector3f normal, Maths::Vector3f position)
{
    m_Normal = normal.Normalize();
    m_Position = position;

}

bool EDX::Plane::Intersects(Ray ray, RayHit& hitResult) const
{
    bool isInvertable = false;
    const Maths::Matrix4x4<float> inverseTransform = EDX::Maths::Matrix4x4<float>::Inverse(m_World, isInvertable);
    if (!isInvertable) {
        return false;
    }

    //Apply the Inverse of this primitive's transformation to the ray. 
    {
        Maths::Vector4f inv_ray_origin = { ray.Origin().x, ray.Origin().y, ray.Origin().z, 1.0f };
        Maths::Vector4f inv_ray_dir = { ray.Direction().x, ray.Direction().y, ray.Direction().z, 0.0f };


        inv_ray_origin = inv_ray_origin * inverseTransform;
        inv_ray_dir = inv_ray_dir * inverseTransform;
        Maths::Vector3f d = { inv_ray_dir.x, inv_ray_dir.y, inv_ray_dir.z };
        d = d.Normalize();

        ray = Ray({ inv_ray_origin.x, inv_ray_origin.y, inv_ray_origin.z }, d);
    }



    float n_dot_r = Maths::Vector3f::Dot(ray.Direction(), m_Normal);
    if (n_dot_r > Maths::Epsilon) {   //Ray is Parallel to / Pointing away from the Plane. 
        return false;
    }

    float d = Maths::Vector3f::Dot((m_Position - ray.Origin()), m_Normal);
    float t = d / n_dot_r;

    if (t < 0.0f) { //Ray intersects the plane Behind the origin. 
        return false;
    }

    //hitResult.point = ray.At(t);
    hitResult.t = t;
    //hitResult.normal = m_Normal;
    hitResult.pMat = const_cast<BlinnPhong*>(&m_Material);

    const Maths::Vector3f p = ray.At(t);
    //Compute transformed intersection point
    {
        Maths::Vector4f hit_point = { p.x, p.y, p.z, 1.0f };
        hit_point = hit_point * m_World;
        hitResult.point = { hit_point.x, hit_point.y, hit_point.z };
    }

    //Compute transformed intersection normal by applying the inverse-transpose of the world matrix. 
    {
        bool isInvertible = false;
        const Maths::Matrix4x4<float> invTranspose = Maths::Matrix4x4<float>::Transpose(inverseTransform);

        Maths::Vector3f n = m_Normal;
        Maths::Vector4f normal = { n.x, n.y, n.z, 0.0f };
        normal = normal * invTranspose;
        hitResult.normal = Maths::Vector3f::Normalize({ normal.x, normal.y, normal.z });
    }



    return true;
}

void EDX::Plane::SetNormal(Maths::Vector3f normal)
{
    m_Normal = normal;
}

