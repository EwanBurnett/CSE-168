#ifndef __SPHERE_H
#define __SPHERE_H
/**
 * @file Sphere.h
 * @brief Sphere Primitive Class
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-29
*/
#include "Primitive.h" 

namespace EDX {
    class Sphere : public Primitive {
    public:
        Sphere(Maths::Vector3f position, float radius);

        bool Intersects(Ray ray, RayHit& hitResult) const override;
        static bool Intersects(Ray ray, const Maths::Vector3f position, const float radius, const Maths::Matrix4x4<float>& world, RayHit& hitResult);

        Maths::Vector3f GetPosition() const;
        void SetPosition(Maths::Vector3f position);

        float GetRadius() const;
        void SetRadius(float radius);

        Maths::Vector3f GetBoundsMin() const override;
        Maths::Vector3f GetBoundsMax() const override;

    private:
        Maths::Vector3f m_Position;
        float m_Radius;
    };
}

#endif

