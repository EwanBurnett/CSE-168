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

        void SetPosition(Maths::Vector3f position);
        void SetRadius(float radius); 

    private:
        Maths::Vector3f m_Position; 
        float m_Radius; 
    };
}

#endif

