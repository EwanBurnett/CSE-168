#include "Grid.h"

#include "../Utils/Logger.h"
#include "../Utils/Timer.h"
#include "../Maths.h"

#include "../RenderData.h"


EDX::Acceleration::Grid::Grid()
{
    m_Dimensions = { 1, 1, 1 };
}

EDX::Acceleration::Grid::Grid(Maths::Vector3<uint32_t> dim)
{
    //TODO: Ensure Grid Dimensions are >= 1
    m_Dimensions.x = Maths::Clamp(dim.x, 1u, UINT32_MAX);
    m_Dimensions.y = Maths::Clamp(dim.y, 1u, UINT32_MAX);
    m_Dimensions.z = Maths::Clamp(dim.z, 1u, UINT32_MAX);

    //m_Dimensions = dim; 
}

void EDX::Acceleration::Grid::BuildAccelerationStructure(EDX::RenderData& renderData) {
    EDX::Log::Status("Building Grid Acceleration Structure.\nDimensions: [%d x %d x %d]\n", m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);
    EDX::Timer timer;
    timer.Start();
    {
        EDX::Maths::Vector3f boundsMin = {};
        boundsMin.Set(0.0f);
        EDX::Maths::Vector3f boundsMax = {};
        boundsMax.Set(0.0f);

        auto compareBounds = [&](const EDX::Maths::Vector3f min, const EDX::Maths::Vector3f max) {
            (min.x < boundsMin.x) ? boundsMin.x = min.x : 0;
            (min.y < boundsMin.y) ? boundsMin.y = min.y : 0;
            (min.z < boundsMin.z) ? boundsMin.z = min.z : 0;

            (max.x > boundsMax.x) ? boundsMax.x = max.x : 0;
            (max.y > boundsMax.y) ? boundsMax.y = max.y : 0;
            (max.z > boundsMax.z) ? boundsMax.z = max.z : 0;
        };

        //Iterate over each primitive, and retrieve its bounds in world XYZ coords. 
        {
            //Spheres
            for (auto& sphere : renderData.scene.Spheres()) {
                EDX::Maths::Vector3f min = sphere.GetBoundsMin();
                EDX::Maths::Vector3f max = sphere.GetBoundsMax();

                compareBounds(min, max);
            }

            //Triangles
            for (auto& tri : renderData.scene.Triangles()) {
                EDX::Maths::Vector3f min = tri.GetBoundsMin();
                EDX::Maths::Vector3f max = tri.GetBoundsMax();

                compareBounds(min, max);
            }

        }

        //Generate u * v * w grid cells
        {
            EDX::Maths::Vector3i gridDimensions = m_Dimensions;
            auto cellSize = (boundsMax - boundsMin);
            cellSize.x /= (float)gridDimensions.x;
            cellSize.y /= (float)gridDimensions.y;
            cellSize.z /= (float)gridDimensions.z;

            for (int z = 0; z < gridDimensions.z; z++) {
                for (int y = 0; y < gridDimensions.y; y++) {
                    for (int x = 0; x < gridDimensions.x; x++) {
                        EDX::Maths::Vector3f dim = { (float)x, (float)y, (float)z };
                        EDX::Maths::Vector3f cellMin = boundsMin + (cellSize * dim);
                        EDX::Maths::Vector3f cellMax = cellMin + cellSize;


                        EDX::Box cell(cellMin, cellMax);
                        m_Cells.push_back({ cell, {} });
                    }
                }
            }
        }

        //Iterate over each primitive in the scene per-cell, and maintain a list of intersections with each grid cell. 
        {
            for (auto& cell : m_Cells) {
                for (auto& sphere : renderData.scene.Spheres()) {
                    if (cell.bounds.Intersects({ sphere.GetBoundsMin(), sphere.GetBoundsMax() })) {
                        cell.intersections.push_back(&sphere);
                    }
                }
                for (auto& tri : renderData.scene.Triangles()) {
                    if (cell.bounds.Intersects({ tri.GetBoundsMin(), tri.GetBoundsMax() })) {
                        cell.intersections.push_back(&tri);
                    }
                }


            }
        }

    }
    timer.Tick();
    float dtms = timer.DeltaTime();

    EDX::Log::Success("Finished building acceleration structures in %fs.\n", dtms);

}

const std::vector<EDX::Acceleration::Grid::Cell>& EDX::Acceleration::Grid::GetCells() const {
    return m_Cells;
}
