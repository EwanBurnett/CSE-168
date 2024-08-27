#ifndef __CAMERA_H
#define __CAMERA_H
/**
 * @file Colour.h
 * @brief RGBA Colour Representation
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-27
*/
#include "Maths/Vector3.h"
#include "Maths/Matrix.h"

namespace EDX {
    class Camera {
    public: 
        Camera(); 
        Camera(Maths::Vector3f& position, double FoVRadians, Maths::Vector3f& forwards, Maths::Vector3f& up, Maths::Vector3f& right);
        Camera(Maths::Vector3f& lookFrom, Maths::Vector3f& lookAt, Maths::Vector3f& up, double FoVRadians); 

        Maths::Vector3f GetPosition(); 
        void SetPosition(Maths::Vector3f& position);

        Maths::Vector3f GetForwardsVector(); 
        Maths::Vector3f GetUpVector(); 
        Maths::Vector3f GetRightVector(); 

        Maths::Matrix4x4<float> GetViewMatrix(); 

        void Orbit(Maths::Vector3f& focus, float theta, float phi, float radius);
        void Look(float dx, float dy, float speed);
        void Walk(Maths::Vector3f& direction, float speed); 

    private:
        float m_FoVRadians;

        Maths::Vector3f m_Position; 

        Maths::Vector3f m_Forwards; 
        Maths::Vector3f m_Up; 
        Maths::Vector3f m_Right; 
    };
}

#endif