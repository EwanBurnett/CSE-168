#ifndef __RAYHIT_H
#define __RAYHIT_H
/**
 * @file RayHit.h
 * @brief Ray Hit Result Representation
 * @author Ewan Burnett (EwanBurnettSK@Outlook.com)
 * @date 2024-08-28
*/
#include "Maths/Vector3.h"
#include "Materials/BlinnPhong.h"
#include <memory>

namespace EDX {
    struct RayHit {
        float t = Maths::Infinity;
        Maths::Vector3f point;
        Maths::Vector3f normal;
        
        BlinnPhong* pMat = nullptr;
    };
}

#endif