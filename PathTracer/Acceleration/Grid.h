#ifndef __GRID_H
#define __GRID_H

#include "../Primitives/Primitive.h"
#include "../Primitives/Box.h"
#include <vector>


namespace EDX {

    struct RenderData;
    namespace Acceleration {
        class Grid {
        public:
            Grid();

            struct Cell {
                EDX::Box bounds;
                std::vector<EDX::Primitive*> intersections;
            };


            void BuildAccelerationStructure(EDX::RenderData& renderData);
            const std::vector<EDX::Acceleration::Grid::Cell>& GetCells() const;

        private:
            std::vector<EDX::Acceleration::Grid::Cell> m_Cells;
        };
    }
}

#endif