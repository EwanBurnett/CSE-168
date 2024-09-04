#include "PointLight.h"

EDX::PointLight::PointLight()
{
}

EDX::PointLight::PointLight(Maths::Vector3f position, Maths::Vector3f attenuation, Colour colour)
{
    m_Position = position; 
    m_Attenuation = attenuation;
    m_Colour = colour;
}

void EDX::PointLight::SetPosition(const Maths::Vector3f position)
{
    m_Position = position; 
}

EDX::Maths::Vector3f EDX::PointLight::GetPosition() const
{
    return m_Position; 
}

void EDX::PointLight::SetAttenuation(const Maths::Vector3f attenuation)
{
    m_Attenuation = attenuation;
}

EDX::Maths::Vector3f EDX::PointLight::GetAttenuation() const
{
    return m_Attenuation;
}

void EDX::PointLight::SetColour(const Colour colour)
{
    m_Colour = colour;
}

EDX::Colour EDX::PointLight::GetColour() const
{
    return m_Colour;
}
