#include "BottomLevelGeometry.h"
#include "DeviceProcedures.h"

#include <Engine/Buffer.h>
#include <Objects/Vertex.h>
#include <Scenes/Scene.h>

namespace RayTracing
{

void BottomLevelGeometry::AddGeometryTriangles(const Scenes::Scene& scene,
                                               const uint32_t vertexOffset, const uint32_t vertexCount,
                                               const uint32_t indexOffset, const uint32_t indexCount,
                                               const bool isOpaque)
{
    VkAccelerationStructureGeometryKHR geometry          = {};
    geometry.sType                                       = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geometry.pNext                                       = nullptr;
    geometry.geometryType                                = VK_GEOMETRY_TYPE_TRIANGLES_KHR;
    geometry.geometry.triangles.sType                    = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_TRIANGLES_DATA_KHR;
    geometry.geometry.triangles.pNext                    = nullptr;
    geometry.geometry.triangles.vertexData.deviceAddress = scene.VertexBuffer().GetDeviceAddress();
    geometry.geometry.triangles.vertexStride             = sizeof(Objects::Vertex);
    geometry.geometry.triangles.maxVertex                = vertexCount;
    geometry.geometry.triangles.vertexFormat             = VK_FORMAT_R32G32B32_SFLOAT;
    geometry.geometry.triangles.indexData.deviceAddress  = scene.IndexBuffer().GetDeviceAddress();
    geometry.geometry.triangles.indexType                = VK_INDEX_TYPE_UINT32;
    geometry.geometry.triangles.transformData            = {};
    geometry.flags                                       = isOpaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0;

    VkAccelerationStructureBuildRangeInfoKHR buildOffsetInfo = {};
    buildOffsetInfo.firstVertex                              = vertexOffset / sizeof(Objects::Vertex);
    buildOffsetInfo.primitiveOffset                          = indexOffset;
    buildOffsetInfo.primitiveCount                           = indexCount / 3;
    buildOffsetInfo.transformOffset                          = 0;

    m_Geometry.emplace_back(geometry);
    m_BuildOffsetInfo.emplace_back(buildOffsetInfo);
}

void BottomLevelGeometry::AddGeometryAabb(
    const Scenes::Scene& scene,
    const uint32_t       aabbOffset,
    const uint32_t       aabbCount,
    const bool           isOpaque)
{
    VkAccelerationStructureGeometryKHR geometry = {};
    geometry.sType                              = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    geometry.pNext                              = nullptr;
    geometry.geometryType                       = VK_GEOMETRY_TYPE_AABBS_KHR;
    geometry.geometry.aabbs.sType               = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_AABBS_DATA_KHR;
    geometry.geometry.aabbs.pNext               = nullptr;
    geometry.geometry.aabbs.data.deviceAddress  = scene.AabbBuffer().GetDeviceAddress();
    geometry.geometry.aabbs.stride              = sizeof(VkAabbPositionsKHR);
    geometry.flags                              = isOpaque ? VK_GEOMETRY_OPAQUE_BIT_KHR : 0;

    VkAccelerationStructureBuildRangeInfoKHR buildOffsetInfo = {};
    buildOffsetInfo.firstVertex                              = 0;
    buildOffsetInfo.primitiveOffset                          = aabbOffset;
    buildOffsetInfo.primitiveCount                           = aabbCount;
    buildOffsetInfo.transformOffset                          = 0;

    m_Geometry.emplace_back(geometry);
    m_BuildOffsetInfo.emplace_back(buildOffsetInfo);
}

}    // namespace RayTracing
