#include "Primitive.h"

void EDX::Primitive::SetMaterial(BlinnPhong material)
{
    m_Material = material;
}

EDX::BlinnPhong EDX::Primitive::GetMaterial() const
{
    return m_Material;
}

void EDX::Primitive::SetWorldMatrix(Maths::Matrix4x4<float> world)
{
    m_World = world;
}

EDX::Maths::Matrix4x4<float> EDX::Primitive::GetWorldMatrix() const
{
    return m_World;
}
