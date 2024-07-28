#include "TopLevelAccelerationStructure.h"

#include "BottomLevelAccelerationStructure.h"
#include "DeviceProcedures.h"

#include <Engine/Buffer.h>
#include <Engine/Device.h>

#include <cstring>

namespace RayTracing
{

TopLevelAccelerationStructure::TopLevelAccelerationStructure(
    const class DeviceProcedures& deviceProcedures,
    const RayTracingProperties&   rayTracingProperties,
    const VkDeviceAddress         instanceAddress,
    const uint32_t                instancesCount)
    : AccelerationStructure(deviceProcedures, rayTracingProperties)
    , m_InstancesCount(instancesCount)
{
    // Create VkAccelerationStructureGeometryInstancesDataKHR. This wraps a device pointer to the above uploaded instances.
    m_InstancesVk.sType              = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_INSTANCES_DATA_KHR;
    m_InstancesVk.arrayOfPointers    = VK_FALSE;
    m_InstancesVk.data.deviceAddress = instanceAddress;

    // Put the above into a VkAccelerationStructureGeometryKHR. We need to put the
    // instances struct in a union and label it as instance data.
    m_TopAsGeometry.sType              = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_GEOMETRY_KHR;
    m_TopAsGeometry.geometryType       = VK_GEOMETRY_TYPE_INSTANCES_KHR;
    m_TopAsGeometry.geometry.instances = m_InstancesVk;

    m_BuildGeometryInfo.sType                    = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_GEOMETRY_INFO_KHR;
    m_BuildGeometryInfo.flags                    = m_Flags;
    m_BuildGeometryInfo.geometryCount            = 1;
    m_BuildGeometryInfo.pGeometries              = &m_TopAsGeometry;
    m_BuildGeometryInfo.mode                     = VK_BUILD_ACCELERATION_STRUCTURE_MODE_BUILD_KHR;
    m_BuildGeometryInfo.type                     = VK_ACCELERATION_STRUCTURE_TYPE_TOP_LEVEL_KHR;
    m_BuildGeometryInfo.srcAccelerationStructure = nullptr;

    m_BuildSizesInfo = GetBuildSizes(&instancesCount);
}

TopLevelAccelerationStructure::TopLevelAccelerationStructure(TopLevelAccelerationStructure&& other) noexcept
    : AccelerationStructure(std::move(other))
    , m_InstancesCount(other.m_InstancesCount)
{
}

TopLevelAccelerationStructure::~TopLevelAccelerationStructure()
{
}

void TopLevelAccelerationStructure::Generate(
    const VkCommandBuffer commandBuffer,
    Engine::Buffer&       scratchBuffer,
    const VkDeviceSize    scratchOffset,
    Engine::Buffer&       resultBuffer,
    const VkDeviceSize    resultOffset)
{
    // Create the acceleration structure.
    CreateAccelerationStructure(resultBuffer, resultOffset);

    // Build the actual bottom-level acceleration structure
    VkAccelerationStructureBuildRangeInfoKHR buildOffsetInfo = {};
    buildOffsetInfo.primitiveCount                           = m_InstancesCount;

    const VkAccelerationStructureBuildRangeInfoKHR* pBuildOffsetInfo = &buildOffsetInfo;

    m_BuildGeometryInfo.dstAccelerationStructure  = Handle();
    m_BuildGeometryInfo.scratchData.deviceAddress = scratchBuffer.GetDeviceAddress() + scratchOffset;

    m_DeviceProcedures.vkCmdBuildAccelerationStructuresKHR(commandBuffer, 1, &m_BuildGeometryInfo, &pBuildOffsetInfo);
}

VkAccelerationStructureInstanceKHR TopLevelAccelerationStructure::CreateInstance(
    const BottomLevelAccelerationStructure& bottomLevelAs,
    const glm::mat4&                        transform,
    const uint32_t                          instanceId,
    const uint32_t                          hitGroupId)
{
    const auto& device          = bottomLevelAs.Device();
    const auto& deviceProcedure = bottomLevelAs.DeviceProcedures();

    VkAccelerationStructureDeviceAddressInfoKHR addressInfo = {};
    addressInfo.sType                                       = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_DEVICE_ADDRESS_INFO_KHR;
    addressInfo.accelerationStructure                       = bottomLevelAs.Handle();

    const VkDeviceAddress address = deviceProcedure.vkGetAccelerationStructureDeviceAddressKHR(device.Handle(), &addressInfo);

    VkAccelerationStructureInstanceKHR instance     = {};
    instance.instanceCustomIndex                    = instanceId;
    instance.mask                                   = 0xFF;                                                         // The visibility mask is always set of 0xFF, but if some instances would need to be ignored in some cases, this flag should be passed by the application.
    instance.instanceShaderBindingTableRecordOffset = hitGroupId;                                                   // Set the hit group index, that will be used to find the shader code to execute when hitting the geometry.
    instance.flags                                  = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;    // Disable culling - more fine control could be provided by the application
    instance.accelerationStructureReference         = address;

    // The instance.transform value only contains 12 values, corresponding to a 4x3 matrix,
    // hence saving the last row that is anyway always (0,0,0,1).
    // Since the matrix is row-major, we simply copy the first 12 values of the original 4x4 matrix
    std::memcpy(&instance.transform, &transform, sizeof(instance.transform));

    return instance;
}

}    // namespace RayTracing
