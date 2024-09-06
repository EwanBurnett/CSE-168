#include "Grid.h"

#include "../Utils/Logger.h"
#include "../Utils/Timer.h"
#include "../Maths.h"

#include "../RenderData.h"


EDX::Acceleration::Grid::Grid()
{
}

void EDX::Acceleration::Grid::BuildAccelerationStructure(EDX::RenderData& renderData) {
    EDX::Log::Status("Building Acceleration Structure...\n");
    EDX::Timer timer;
    timer.Start();
    {
        EDX::Maths::Vector3f boundsMin = {};
        boundsMin.Set(EDX::Maths::Infinity);
        EDX::Maths::Vector3f boundsMax = {};
        boundsMax.Set(-EDX::Maths::Infinity);

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
            EDX::Maths::Vector3i gridDimensions = { 5, 5, 5 };
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
                for (auto& tri : renderData.scene.Triangles()) {
                    if (cell.bounds.Intersects(tri)) {

                        cell.intersections.push_back(&tri);
                    }
                }

                for (auto& sphere : renderData.scene.Spheres()) {
                    if (cell.bounds.Intersects(sphere)) {
                        cell.intersections.push_back(&sphere);
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
