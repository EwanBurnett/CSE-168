#ifndef __GRID_H
#define __GRID_H

#include "../Primitives/Primitive.h"
#include "../Primitives/Box.h"

#include "AccelStructure.h"

namespace EDX {

    struct RenderData;
    namespace Acceleration {

        class Grid : public AccelStructure {
        public:
            Grid();
            Grid(Maths::Vector3<uint32_t> dim);

            struct Cell {
                EDX::Box bounds;
                std::vector<EDX::Primitive*> intersections;
            };


            void Build(EDX::RenderData& renderData) override;

            bool Traverse(const EDX::Ray& ray, std::vector<RayHit>& results) const override;

            const std::vector<EDX::Acceleration::Grid::Cell>& GetCells() const;

        private:
            int ConvertXYZToIndex(int x, int y, int z) const; 
            Maths::Vector3i ConvertIndexToXYZ(int idx) const; 
            int GetCellIndex(Maths::Vector3f point) const; 
            Maths::Vector3i GetCellXYZ(Maths::Vector3f point) const; 

        private:
            std::vector<EDX::Acceleration::Grid::Cell> m_Cells;
            Maths::Vector3<uint32_t> m_Dimensions;

            Maths::Vector3f m_BoundsMin; 
            Maths::Vector3f m_BoundsMax;

            Maths::Vector3f m_CellSize;
        };
    }
}

#endif