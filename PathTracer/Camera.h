#ifndef __CAMERA_H
#define __CAMERA_H
/**
 * @file Colour.h
 * @brief RGBA Colour Representation
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-27
*/
#include "Maths.h"

namespace EDX {
    class Ray; 

    class Camera {
    public: 
        Camera(); 
        Camera(Maths::Vector3f& position, double FoVRadians, Maths::Vector3f& forwards, Maths::Vector3f& up, Maths::Vector3f& right);
        Camera(Maths::Vector3f& lookFrom, Maths::Vector3f& lookAt, Maths::Vector3f& up, double FoVRadians); 

        Maths::Vector3f GetPosition() const; 
        void SetPosition(Maths::Vector3f& position);

        Maths::Vector3f GetForwardsVector() const; 
        Maths::Vector3f GetUpVector() const; 
        Maths::Vector3f GetRightVector() const; 

        /**
         * @brief Returns the camera's Vertical Field of View, in Radians.
        */
        float GetFoVRadians() const; 
        void SetFoVRadians(const float FoVRadians);
        void SetFoVDegrees(const float FoVDegrees);

        Maths::Matrix4x4<float> GetViewMatrix() const; 

        void Orbit(Maths::Vector3f& focus, float phi, float theta, float radius);
        void Look(float dx, float dy, float speed);
        void Walk(Maths::Vector3f& direction, float speed); 

        //Viewport - {left, right, top, bottom}
        Ray GenRay(const Maths::Vector4i viewport, const uint32_t x, const uint32_t y) const; 
    private:
        float m_FoVRadians;

        Maths::Vector3f m_Position; 

        Maths::Vector3f m_Forwards; 
        Maths::Vector3f m_Up; 
        Maths::Vector3f m_Right; 
    };
}

#endif