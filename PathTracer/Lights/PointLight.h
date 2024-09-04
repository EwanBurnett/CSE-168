#ifndef __POINTLIGHT_H
#define __POINTLIGHT_H
/**
 * @file PointLight.h
 * @brief Point Light Class
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-09-04
*/
#include "../Maths/Vector3.h"
#include "../Colour.h"

namespace EDX {
    class PointLight {
    public:
        PointLight(); 
        PointLight(Maths::Vector3f position,Maths::Vector3f attenuation, Colour colour); 

        void SetPosition(const Maths::Vector3f position); 
        Maths::Vector3f GetPosition() const; 


        void SetAttenuation(const Maths::Vector3f attenuation); 
        Maths::Vector3f GetAttenuation() const; 

        void SetColour(const Colour colour); 
        Colour GetColour() const; 

    private:
        Maths::Vector3f m_Position;  
        Maths::Vector3f m_Attenuation;  //Constant, Linear, Exponential
        Colour m_Colour; 
    };
}
#endif