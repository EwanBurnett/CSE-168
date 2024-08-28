#ifndef __SPHERE_H
#define __SPHERE_H

#include "../Maths/Vector3.h"
#include "../Ray.h"
#include "../RayHit.h"

namespace EDX {
    class Sphere {
    public: 
        Sphere(Maths::Vector3f position, float radius); 

        bool Intersects(Ray ray, RayHit& hitResult);

        void SetPosition(Maths::Vector3f position);
        void SetRadius(float radius); 

    private:
        Maths::Vector3f m_Position; 
        float m_Radius; 
    };
}

#endif

