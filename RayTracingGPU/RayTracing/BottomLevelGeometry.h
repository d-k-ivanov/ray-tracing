#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Objects
{
class Procedural;
}

namespace Scenes
{
class Scene;
}

namespace RayTracing
{

class BottomLevelGeometry final
{
public:
    size_t size() const { return m_Geometry.size(); }

    const std::vector<VkAccelerationStructureGeometryKHR>&       Geometry() const { return m_Geometry; }
    const std::vector<VkAccelerationStructureBuildRangeInfoKHR>& BuildOffsetInfo() const { return m_BuildOffsetInfo; }

    void AddGeometryTriangles(
        const Scenes::Scene& scene,
        uint32_t             vertexOffset,
        uint32_t             vertexCount,
        uint32_t             indexOffset,
        uint32_t             indexCount,
        bool                 isOpaque);

    void AddGeometryAabb(
        const Scenes::Scene& scene,
        uint32_t             aabbOffset,
        uint32_t             aabbCount,
        bool                 isOpaque);

private:
    // The geometry to build, addresses of vertices and indices.
    std::vector<VkAccelerationStructureGeometryKHR> m_Geometry;

    // the number of elements to build and offsets
    std::vector<VkAccelerationStructureBuildRangeInfoKHR> m_BuildOffsetInfo;
};

}    // namespace RayTracing
