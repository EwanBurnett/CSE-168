#ifndef __PRIMITIVE_H
#define __PRIMITIVE_H
/**
 * @file Triangle.h
 * @brief Triangle Primitive Class
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-09-02
*/
#include "../Maths/Vector3.h"
#include "../Maths/Matrix.h"
#include "../Ray.h"
#include "../RayHit.h"
#include "../Materials/BlinnPhong.h"
namespace EDX {
    class Primitive {
    public:
        Primitive() = default;
        virtual ~Primitive() = default;

        virtual bool Intersects(Ray ray, RayHit& hitResult) const = 0;

        void SetMaterial(BlinnPhong material);
        BlinnPhong GetMaterial() const;

        void SetWorldMatrix(Maths::Matrix4x4<float> world);
        Maths::Matrix4x4<float> GetWorldMatrix() const;
    protected:
        BlinnPhong m_Material;
        Maths::Matrix4x4<float> m_World;
    };
}
#endif