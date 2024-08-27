#include "Camera.h"

EDX::Camera::Camera()
{
    m_Position = { 0.0f, 0.0f, 0.0f };

    m_Forwards = Maths::Vector3f::Forwards(); 
    m_Up = Maths::Vector3f::Up(); 
    m_Right = Maths::Vector3f::Right(); 

    m_FoVRadians = Maths::DegToRad(90.0); 
}

EDX::Camera::Camera(Maths::Vector3f& position, double FoVRadians, Maths::Vector3f& forwards, Maths::Vector3f& up, Maths::Vector3f& right)
{
    m_Position = position; 

    m_Forwards = forwards; 
    m_Up = up; 
    m_Right = right; 

    m_FoVRadians = FoVRadians; 
}

EDX::Camera::Camera(Maths::Vector3f& lookFrom, Maths::Vector3f& lookAt, Maths::Vector3f& up, double FoVRadians)
{
    m_Position = lookFrom; 

    m_Forwards = Maths::Vector3f::Normalize(lookAt - lookFrom); 
    m_Up = up; 
    m_Right = Maths::Vector3f::Cross(m_Forwards, m_Up); 
    Maths::Vector3f::Orthonormalize(m_Forwards, m_Up, m_Right); 

    m_FoVRadians = FoVRadians; 
}
