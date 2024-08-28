#ifndef __RAYHIT_H
#define __RAYHIT_H

#include "Maths/Vector3.h"

namespace EDX {
    struct RayHit {
        float t;
        Maths::Vector3f point;
        Maths::Vector3f normal;
    };
}

#endif