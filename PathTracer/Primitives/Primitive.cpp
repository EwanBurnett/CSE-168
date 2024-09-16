#include "Primitive.h"

void EDX::Primitive::SetMaterial(BlinnPhong material)
{
    m_Material = material;
}

EDX::BlinnPhong* EDX::Primitive::GetMaterial() const
{
    return const_cast<EDX::BlinnPhong*>(&m_Material);
}

void EDX::Primitive::SetWorldMatrix(Maths::Matrix4x4<float> world)
{
    m_World = world;
}

EDX::Maths::Matrix4x4<float> EDX::Primitive::GetWorldMatrix() const
{
    return m_World;
}

const EDX::Primitive::EPrimitiveType EDX::Primitive::GetType() const
{
    return m_Type;
}
