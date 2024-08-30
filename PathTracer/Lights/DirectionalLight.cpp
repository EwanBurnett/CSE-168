#include "DirectionalLight.h"

EDX::DirectionalLight::DirectionalLight()
{
    m_Direction = { 0.0f, 0.0f, 0.0f };
    m_Colour = { 0.0f, 0.0f, 0.0f, 0.0f };
}

EDX::DirectionalLight::DirectionalLight(Maths::Vector3f direction, Colour colour)
{
    m_Direction = direction;
    m_Colour = colour;
}

void EDX::DirectionalLight::SetDirection(const Maths::Vector3f direction)
{
    m_Direction = direction;
}

EDX::Maths::Vector3f EDX::DirectionalLight::GetDirection() const
{
    return m_Direction;
}

void EDX::DirectionalLight::SetColour(const Colour colour)
{
    m_Colour = colour;
}

EDX::Colour EDX::DirectionalLight::GetColour() const
{
    return m_Colour;
}
