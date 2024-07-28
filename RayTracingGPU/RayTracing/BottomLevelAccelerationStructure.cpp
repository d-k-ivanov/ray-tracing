#include "BottomLevelAccelerationStructure.h"

#include "DeviceProcedures.h"

#include <Engine/Buffer.h>
#include <Scenes/Scene.h>

namespace RayTracing
{

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(const class DeviceProcedures& deviceProcedures,
                                                                   const RayTracingProperties&   rayTracingProperties,
                                                                   BottomLevelGeometry           geometries)
    : AccelerationStructure(deviceProcedures, rayTracingProperties)
    , m_Geometries(std::move(geometries))
{
    m_BuildGeometryInfo.sType                    = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    m_BuildGeometryInfo.flags                    = m_Flags;
    m_BuildGeometryInfo.geometryCount            = static_cast<uint32_t>(m_Geometries.Geometry().size());
    m_BuildGeometryInfo.pGeometries              = m_Geometries.Geometry().data();
    m_BuildGeometryInfo.mode                     = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    m_BuildGeometryInfo.type                     = VK_ACCELERATION_STRUCTURE_TYPE_BOTTOM_LEVEL_KHR;
    m_BuildGeometryInfo.srcAccelerationStructure = nullptr;

    std::vector<uint32_t> maxPrimCount(m_Geometries.BuildOffsetInfo().size());

    for(size_t i = 0; i != maxPrimCount.size(); ++i)
    {
        maxPrimCount[i] = m_Geometries.BuildOffsetInfo()[i].primitiveCount;
    }

    m_BuildSizesInfo = GetBuildSizes(maxPrimCount.data());
}

BottomLevelAccelerationStructure::BottomLevelAccelerationStructure(BottomLevelAccelerationStructure&& other) noexcept
    : AccelerationStructure(std::move(other))
    , m_Geometries(std::move(other.m_Geometries))
{
}

void BottomLevelAccelerationStructure::Generate(const VkCommandBuffer commandBuffer,
                                                const Engine::Buffer& scratchBuffer,
                                                const VkDeviceSize    scratchOffset,
                                                const Engine::Buffer& resultBuffer,
                                                const VkDeviceSize    resultOffset)
{
    // Create the acceleration structure.
    CreateAccelerationStructure(resultBuffer, resultOffset);

    // Build the actual bottom-level acceleration structure
    const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = m_Geometries.BuildOffsetInfo().data();

    m_BuildGeometryInfo.dstAccelerationStructure  = Handle();
    m_BuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.GetDeviceAddress() + scratchOffset;

    m_DeviceProcedures.vkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &m_BuildGeometryInfo, &pBuildOffsetInfo);
}

}    // namespace RayTracing
