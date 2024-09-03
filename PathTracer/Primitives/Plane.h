#ifndef __PLANE_H
#define __PLANE_H
/**
 * @file Plane.h
 * @brief Plane Primitive Class
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-29
*/
#include "Primitive.h"

namespace EDX {

    class Plane : public Primitive{
    public: 
        Plane(Maths::Vector3f normal, Maths::Vector3f position); 

        bool Intersects(Ray ray, RayHit& hitResult) const override;

        void SetNormal(Maths::Vector3f normal);

    private: 
        Maths::Vector3f m_Normal; 
        Maths::Vector3f m_Position; 

    };
}

#endif