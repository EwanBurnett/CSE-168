#ifndef __PLANE_H
#define __PLANE_H

#include "../Maths/Vector3.h"
#include "../Ray.h"
#include "../RayHit.h"

namespace EDX {

    class Plane {
    public: 
        Plane(Maths::Vector3f normal, float offset); 

        bool Intersects(Ray ray, RayHit& hitResult);

        void SetNormal(Maths::Vector3f normal);
        void SetOffset(float offset); 

    private: 
        Maths::Vector3f m_Normal; 
        float m_Offset; 

    };
}

#endif