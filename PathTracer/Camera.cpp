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

    m_Forwards = Maths::Vector3f::Normalize(lookFrom - lookAt);
    m_Up = up;
    m_Right = Maths::Vector3f::Cross(m_Forwards, m_Up);

    Maths::Vector3f::Orthonormalize(m_Forwards, m_Up, m_Right);

    m_FoVRadians = FoVRadians;
}

EDX::Maths::Vector3f EDX::Camera::GetPosition() const
{
    return m_Position;
}

void EDX::Camera::SetPosition(Maths::Vector3f& position)
{
    m_Position = position;
}

EDX::Maths::Vector3f EDX::Camera::GetForwardsVector() const
{
    return m_Forwards;
}

EDX::Maths::Vector3f EDX::Camera::GetUpVector() const
{
    return m_Up;
}

EDX::Maths::Vector3f EDX::Camera::GetRightVector() const
{
    return m_Right;
}

float EDX::Camera::GetFoVRadians() const
{
    return m_FoVRadians;
}

void EDX::Camera::SetFoVRadians(const float FoVRadians)
{
    m_FoVRadians = FoVRadians;
}

void EDX::Camera::SetFoVDegrees(const float FoVDegrees)
{
    m_FoVRadians = Maths::DegToRad(FoVDegrees);
}

EDX::Maths::Matrix4x4<float> EDX::Camera::GetViewMatrix() const
{
    return Maths::Matrix4x4<float>::View(m_Position, m_Forwards, m_Right, m_Up);
}

void EDX::Camera::Orbit(Maths::Vector3f& focus, float phi, float theta, float radius)
{
    m_Position.x = radius * sinf(phi) * cosf(theta);
    m_Position.y = radius * sinf(phi) * cosf(theta);
    m_Position.z = radius * cosf(phi);

    m_Position += focus; 

    m_Up = Maths::Vector3f::Up();
    m_Forwards = Maths::Vector3f::Normalize(m_Position - focus);
    m_Right = Maths::Vector3f::Cross(m_Forwards, m_Up);

    Maths::Vector3f::Orthonormalize(m_Forwards, m_Up, m_Right);
}

void EDX::Camera::Look(float dx, float dy, float speed)
{
    //TODO: first-person camera logic
}

void EDX::Camera::Walk(Maths::Vector3f& direction, float speed)
{
    Maths::Vector3f dir = {};
    dir += direction * m_Forwards;
    dir += direction * m_Up;
    dir += direction * m_Right;
    dir = dir.Normalize();

    m_Position += dir * speed;
}
