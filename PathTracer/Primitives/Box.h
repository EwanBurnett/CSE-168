#ifndef __BOX_H
#define __BOX_H
/**
 * @file Box.h
 * @brief Box Primitive Class
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-09-02
*/
#include "../Maths/Vector3.h"
#include "../Ray.h"
#include "../RayHit.h"
#include "../Materials/BlinnPhong.h"
namespace EDX {
    class Sphere; 
    class Triangle; 

    /**
     * @brief Represents a Box, using its minimum and maximum coordinates. 
    */
    class Box {
    public: 
        Box(); 
        Box(Maths::Vector3f boundsMin, Maths::Vector3f boundsMax);

        bool Intersects(Ray ray, RayHit& hitResult) const; 
        bool Intersects(EDX::Box b) const; 
        static bool Intersects(Ray ray, Maths::Vector3f boundsMin, Maths::Vector3f boundsMax, RayHit& hitResult) ; 
        
        void SetMaterial(BlinnPhong material); 
        BlinnPhong GetMaterial() const; 

        Maths::Vector3f GetBoundsMin() const; 
        Maths::Vector3f GetBoundsMax() const; 


    private:
        Maths::Vector3f m_BoundsMin; 
        Maths::Vector3f m_BoundsMax; 
    };
}

#endif