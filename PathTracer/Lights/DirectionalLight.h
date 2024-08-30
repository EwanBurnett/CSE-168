#ifndef __DIRECTIONALLIGHT_H
#define __DIRECTIONALLIGHT_H
/**
 * @file DirectionalLight.h
 * @brief Directional Light Class
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-30
*/
#include "../Maths/Vector3.h"
#include "../Colour.h"

namespace EDX {
    class DirectionalLight {
    public:
        DirectionalLight(); 
        DirectionalLight(Maths::Vector3f direction, Colour colour); 

        void SetDirection(const Maths::Vector3f direction); 
        Maths::Vector3f GetDirection() const; 

        void SetColour(const Colour colour); 
        Colour GetColour() const; 

    private:
        Maths::Vector3f m_Direction; 
        Colour m_Colour; 
    };
}
#endif