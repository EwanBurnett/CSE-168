#ifndef __ACCELSTRUCTURE_H
#define __ACCELSTRUCTURE_H

#include "../Ray.h"
#include "../RayHit.h"
#include <vector> 

namespace EDX {
    struct RenderData;

    namespace Acceleration {
        class AccelStructure {
        public:
            virtual void Build(EDX::RenderData& renderData) = 0;
            virtual bool Traverse(const EDX::Ray& ray, std::vector<RayHit>& results) const = 0;
        };
    }
}

#endif