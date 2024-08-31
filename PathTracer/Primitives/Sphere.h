#ifndef __SPHERE_H
#define __SPHERE_H
/**
 * @file Sphere.h
 * @brief Sphere Primitive Class
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-29
*/
#include "../Maths/Vector3.h"
#include "../Ray.h"
#include "../RayHit.h"
#include "../Materials/BlinnPhong.h"

namespace EDX {
    class Sphere {
    public: 
        Sphere(Maths::Vector3f position, float radius); 

        bool Intersects(Ray ray, RayHit& hitResult) const;

        void SetPosition(Maths::Vector3f position);
        void SetRadius(float radius); 

        void SetMaterial(BlinnPhong material); 
        BlinnPhong GetMaterial() const; 

    private:
        Maths::Vector3f m_Position; 
        float m_Radius; 

        BlinnPhong m_Material; 
    };
}

#endif

