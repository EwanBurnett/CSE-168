#ifndef __TRIANGLE_H
#define __TRIANGLE_H

#include "../Maths/Vector3.h"
#include "../Ray.h"
#include "../RayHit.h"

namespace EDX {

    class Triangle {
    public: 
        Triangle(Maths::Vector3f pointA, Maths::Vector3f pointB, Maths::Vector3f pointC); 

        bool Intersects(Ray ray, RayHit& hitResult);


    private: 
        Maths::Vector3f m_PointA; 
        Maths::Vector3f m_PointB; 
        Maths::Vector3f m_PointC; 

        Maths::Vector3f m_Normal; 

    };
}

#endif