#include "Grid.h"

#include "../Utils/Logger.h"
#include "../Utils/ProgressBar.h"
#include "../Maths.h"

#include "../RenderData.h"


EDX::Acceleration::Grid::Grid()
{
    m_Dimensions = { 1, 1, 1 };
}

EDX::Acceleration::Grid::Grid(Maths::Vector3<uint32_t> dim)
{
    //Ensure Grid Dimensions are >= 1
    m_Dimensions.x = Maths::Clamp(dim.x, 1u, UINT32_MAX);
    m_Dimensions.y = Maths::Clamp(dim.y, 1u, UINT32_MAX);
    m_Dimensions.z = Maths::Clamp(dim.z, 1u, UINT32_MAX);
}

void EDX::Acceleration::Grid::Build(EDX::RenderData& renderData) {
    EDX::Log::Status("Building Grid Acceleration Structure.\nDimensions: [%d x %d x %d]\n", m_Dimensions.x, m_Dimensions.y, m_Dimensions.z);
    EDX::ProgressBar pb;
    {
        m_BoundsMin.Set(0.0f);
        m_BoundsMax.Set(0.0f);

        auto compareBounds = [&](const EDX::Maths::Vector3f min, const EDX::Maths::Vector3f max) {
            (min.x < m_BoundsMin.x) ? m_BoundsMin.x = min.x : 0;
            (min.y < m_BoundsMin.y) ? m_BoundsMin.y = min.y : 0;
            (min.z < m_BoundsMin.z) ? m_BoundsMin.z = min.z : 0;

            (max.x > m_BoundsMax.x) ? m_BoundsMax.x = max.x : 0;
            (max.y > m_BoundsMax.y) ? m_BoundsMax.y = max.y : 0;
            (max.z > m_BoundsMax.z) ? m_BoundsMax.z = max.z : 0;
        };

        //Iterate over each primitive, and retrieve its bounds in world XYZ coords. 
        {
            //Spheres
            for (const auto& sphere : renderData.scene.Spheres()) {
                EDX::Maths::Vector3f min = sphere.GetBoundsMin();
                EDX::Maths::Vector3f max = sphere.GetBoundsMax();

                compareBounds(min, max);
            }

            //Triangles
            for (const auto& tri : renderData.scene.Triangles()) {
                EDX::Maths::Vector3f min = tri.GetBoundsMin();
                EDX::Maths::Vector3f max = tri.GetBoundsMax();

                compareBounds(min, max);
            }

        }

        //Generate u * v * w grid cells
        {
            EDX::Maths::Vector3i gridDimensions = m_Dimensions;

            m_CellSize = (m_BoundsMax - m_BoundsMin);
            m_CellSize.x /= (float)gridDimensions.x;
            m_CellSize.y /= (float)gridDimensions.y;
            m_CellSize.z /= (float)gridDimensions.z;

            //m_Cells.resize(gridDimensions.x * gridDimensions.y * gridDimensions.z);

            //TODO: Multithreaded Acceleration Structure Generation
            std::atomic<uint64_t> cellsProcessed = 0;

            for (int z = 0; z < gridDimensions.z; z++) {
                for (int y = 0; y < gridDimensions.y; y++) {
                    for (int x = 0; x < gridDimensions.x; x++) {
                        EDX::Maths::Vector3f dim = { (float)x, (float)y, (float)z };
                        EDX::Maths::Vector3f cellMin = m_BoundsMin + (m_CellSize * dim);
                        EDX::Maths::Vector3f cellMax = cellMin + m_CellSize;

                        uint32_t idx = ConvertXYZToIndex(x, y, z);
                        //auto xyz = ConvertIndexToXYZ(cellXYZ); 
                        //m_Cells[idx] = { {cellMin, cellMax}, {} };

                        Cell cell = { { cellMin, cellMax }, {} };

                        //Iterate over each primitive in the scene per-cell, and maintain a list of intersections with each grid cell. 
                        {
                            for (auto& tri : renderData.scene.Triangles()) {
                                if (cell.bounds.Intersects({ tri.GetBoundsMin(), tri.GetBoundsMax() })) {
                                    cell.intersections.push_back(&tri);
                                }
                            }
                            for (auto& sphere : renderData.scene.Spheres()) {
                                if (cell.bounds.Intersects({ sphere.GetBoundsMin(), sphere.GetBoundsMax() })) {
                                    cell.intersections.push_back(&sphere);
                                }
                            }
                        }

                        if (cell.intersections.size() > 0) {
                            m_Cells.push_back(cell);
                        }

                        cellsProcessed++;
                        const uint64_t numCells = gridDimensions.x * gridDimensions.y * gridDimensions.z;
                        pb.Update((float)(cellsProcessed) / (float)(numCells));
                    }
                }
            }
        }


    }

    float dtms = pb.GetProgressTimer().DeltaTime();

    EDX::Log::Success("\nFinished building acceleration structures in %fs.\n", dtms);
    const uint64_t numCells = m_Dimensions.x * m_Dimensions.y * m_Dimensions.z;
    EDX::Log::Print("Passed Cell Count: %d / %d\n", m_Cells.size(), numCells);

}

bool EDX::Acceleration::Grid::Traverse(const EDX::Ray& ray, std::vector<RayHit>& results) const
{
    //DDA Testing
    {
        /*
        auto gridSize = m_BoundsMax - m_BoundsMin;


        //Retrieve the grid cell index of the ray's origin.

        int cellIdx = GetCellIndex(ray.Origin());
        auto cellXYZ = ConvertIndexToXYZ(cellIdx);

        //If the ray originates from outside the grid, take the index of the first voxel it intersects with.
        if (cellIdx < 0 || cellIdx >= m_Cells.size()) {
            //If the ray doesn't intersect with the grid, then ignore it.
            RayHit gridHit = {};
            {
                if (!EDX::Box::Intersects(ray, m_BoundsMin, m_BoundsMax, gridHit)) {
                    return false;
                }
                //else if (gridHit.t < Maths::Epsilon) {
                   // return false;
               // }
            }
            cellIdx = GetCellIndex(gridHit.point);
            if (cellIdx < 0 || cellIdx >= m_Cells.size()) {
                return false;
            }
            cellXYZ = ConvertIndexToXYZ(cellIdx);
        }


        //Compute the step direction
        Maths::Vector3i step = {};
        step.x = (ray.Direction().x >= 0.0f) ? 1 : -1;
        step.y = (ray.Direction().y >= 0.0f) ? 1 : -1;
        step.z = (ray.Direction().z >= 0.0f) ? 1 : -1;

        Maths::Vector3f delta_t = {};



        //auto delta_t = Maths::Vector3f(1.0f, 1.0f, 1.0f) / ray.Direction();
        const auto& cell = m_Cells[cellIdx];

        */
    }

    //TODO: DDA 3D acceleration; Although it's fast, This is still a O(n) search through each grid cell..
    for (auto& cell : m_Cells) {
        RayHit cellHit = {};
        if (cell.bounds.Intersects(ray, cellHit)) {
            //Test intersections within this cell. 
            for (uint64_t i = 0; i < cell.intersections.size(); i++) {
                Primitive* pPrimitive = cell.intersections[i];

                RayHit l_Result = {};
                if (pPrimitive->Intersects(ray, l_Result)) {
                    if (l_Result.t > 0.0f) {
                        l_Result.pMat = pPrimitive->GetMaterial();
                        results.push_back(l_Result);
                    }
                }
            }
        }
    }

    /*
    Maths::Vector3f tMin = (cell.bounds.GetBoundsMin() - ray.Origin()) / ray.Direction();
    Maths::Vector3f tMax = (cell.bounds.GetBoundsMax() - ray.Origin()) / ray.Direction();

    if (tMin.x > tMax.x) {
        std::swap(tMin.x, tMax.x);
    }
    if (tMin.y > tMax.y) {
        std::swap(tMin.y, tMax.y);
    }
    if (tMin.z > tMax.z) {
        std::swap(tMin.z, tMax.z);
    }

    //Step through the grid
    {
        std::vector<EDX::Primitive*>const* ppPrimitives = nullptr;

        do {
            if (tMax.x < tMax.y) {
                if (tMax.x < tMax.z) {
                    cellXYZ.x += step.x;
                    tMax.x += delta_t.x;
                }
                else {
                    cellXYZ += step.z;
                    tMax.z += delta_t.z;
                }
            }
            else {
                if (tMax.y < tMax.z) {
                    cellXYZ.y += step.y;
                    tMax.y += delta_t.y;
                }
                else {
                    cellXYZ.z += step.z;
                    tMax.z += delta_t.z;
                }
            }


            cellXYZ.x = Maths::Clamp(cellXYZ.x, 0, (int)m_Dimensions.x - 1);
            cellXYZ.y = Maths::Clamp(cellXYZ.y, 0, (int)m_Dimensions.y - 1);
            cellXYZ.z = Maths::Clamp(cellXYZ.z, 0, (int)m_Dimensions.z - 1);

            const auto idx = ConvertXYZToIndex(cellXYZ.x, cellXYZ.y, cellXYZ.z);
            ppPrimitives = &m_Cells[idx].intersections;


            if (ppPrimitives) {
                for (auto* pPrimitive : *ppPrimitives) {

                    RayHit l_Result = {};
                    if (pPrimitive->Intersects(ray, l_Result)) {
                        if (l_Result.t > 0.0f) {
                            l_Result.pMat = pPrimitive->GetMaterial();
                            results.push_back(l_Result);
                        }
                    }
                }
            }

        } while (tMax.x <= 1.0f && tMax.y <= 1.0f && tMax.z <= 1.0f);
    }
}



    */


    /*
    //Test whether the ray intersects with the grid.
    RayHit gridResult = {};
    const bool intersectsGrid = Box::Intersects(ray, m_BoundsMin, m_BoundsMax, gridResult);

    if (!intersectsGrid) {
        return false;
    }


    float t_min = 0.0f;
    float t_max = 0.0f;

}


for (const auto& cell : m_Cells) {
    //Ignore empty cells.
    if (cell.intersections.empty()) {
        continue;
    }

    //Attempt an intersection with this cell.
    RayHit c_Result = {};
    if (!cell.bounds.Intersects(ray, c_Result)) {
        continue;
    }

    for (uint64_t i = 0; i < cell.intersections.size(); i++) {
        Primitive* pPrimitive = cell.intersections[i];

        RayHit l_Result = {};
        if (pPrimitive->Intersects(ray, l_Result)) {
            if (l_Result.t > 0.0f) {
                l_Result.pMat = pPrimitive->GetMaterial();
                results.push_back(l_Result);
            }
        }
    }
}
    */

    return results.empty();
}

const std::vector<EDX::Acceleration::Grid::Cell>& EDX::Acceleration::Grid::GetCells() const {
    return m_Cells;
}

int EDX::Acceleration::Grid::ConvertXYZToIndex(int x, int y, int z) const
{
    return (((z * m_Dimensions.z) + y) * m_Dimensions.x) + x;
}

EDX::Maths::Vector3i EDX::Acceleration::Grid::ConvertIndexToXYZ(int idx) const
{
    return Maths::Vector3i{idx % (int)m_Dimensions.x, (idx / (int)m_Dimensions.x) % (int)m_Dimensions.y, (idx / (int)m_Dimensions.x) / (int)m_Dimensions.y};
}

int EDX::Acceleration::Grid::GetCellIndex(Maths::Vector3f point) const
{
    const Maths::Vector3i cellIdx = GetCellXYZ(point);
    return ConvertXYZToIndex(cellIdx.x, cellIdx.y, cellIdx.z);
}

EDX::Maths::Vector3i EDX::Acceleration::Grid::GetCellXYZ(Maths::Vector3f point) const
{
    auto rayCell = (point + ((m_BoundsMax))) / m_CellSize;

    Maths::Vector3i cellIdx = {};
    cellIdx.x = (int)std::floor(rayCell.x); //-1;
    cellIdx.y = (int)std::floor(rayCell.y);// -1;
    cellIdx.z = (int)std::floor(rayCell.z);// -1;

    return cellIdx;
}
