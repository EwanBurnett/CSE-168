#ifndef __TRIANGLE_H
#define __TRIANGLE_H
/**
 * @file Triangle.h
 * @brief Triangle Primitive Class
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-29
*/
#include "Primitive.h"
namespace EDX {

    /**
     * @brief Defines a triangle, with CLOCKWISE winding order. 
    */
    class Triangle : public Primitive {
    public: 
        Triangle(Maths::Vector3f pointA, Maths::Vector3f pointB, Maths::Vector3f pointC); 

        bool Intersects(Ray ray, RayHit& hitResult) const override;
    private: 
        Maths::Vector3f m_PointA; 
        Maths::Vector3f m_PointB; 
        Maths::Vector3f m_PointC; 

        Maths::Vector3f m_Normal; 

    };
}

#endif