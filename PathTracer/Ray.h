#ifndef __RAY_H
#define __RAY_H
/**
 * @file Ray.h
 * @brief Ray Class
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-27
*/
#include "Maths/Vector3.h"

namespace EDX {
    using Vec3 = Maths::Vector3f; 

    class Ray {
    public: 
        Ray(); 
        Ray(const Vec3& origin, const Vec3& direction);

        Vec3 At(const float t) const; 
        Vec3 Origin() const; 
        Vec3 Direction() const; 

    private:
        Vec3 m_Origin; 
        Vec3 m_Direction; 
    };

    inline Ray::Ray() {
        m_Origin = {};
        m_Direction = {};
    }

    inline Ray::Ray(const Vec3& origin, const Vec3& direction) : m_Origin(origin), m_Direction(direction)
    {
    }

    inline Vec3 Ray::At(const float t) const {
        return m_Origin + (m_Direction * t); 
    } 

    inline Vec3 Ray::Origin() const {
        return m_Origin; 
    }

    inline Vec3 Ray::Direction() const {
        return m_Direction; 
    }
}

#endif