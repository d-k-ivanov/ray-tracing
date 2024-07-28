#include "AccelerationStructure.h"

#include "DeviceProcedures.h"
#include "RayTracingProperties.h"

#include <Engine/Buffer.h>
#include <Engine/Device.h>
#include <Utils/VulkanException.h>

#undef MemoryBarrier

namespace RayTracing
{

namespace
{
uint64_t RoundUp(const uint64_t size, const uint64_t granularity)
{
    const auto divUp = (size + granularity - 1) / granularity;
    return divUp * granularity;
}
}

AccelerationStructure::AccelerationStructure(const class DeviceProcedures& deviceProcedures, const RayTracingProperties& rayTracingProperties)
    : m_DeviceProcedures(deviceProcedures)
    , m_Flags(VK_BUILD_ACCELERATION_STRUCTURE_PREFER_FAST_TRACE_BIT_KHR)
    , m_Device(deviceProcedures.Device())
    , m_RayTracingProperties(rayTracingProperties)
{
}

AccelerationStructure::AccelerationStructure(AccelerationStructure&& other) noexcept
    : m_DeviceProcedures(other.m_DeviceProcedures)
    , m_Flags(other.m_Flags)
    , m_BuildGeometryInfo(other.m_BuildGeometryInfo)
    , m_BuildSizesInfo(other.m_BuildSizesInfo)
    , m_AccelerationStructure(other.m_AccelerationStructure)
    , m_Device(other.m_Device)
    , m_RayTracingProperties(other.m_RayTracingProperties)
{
    other.m_AccelerationStructure = nullptr;
}

AccelerationStructure::~AccelerationStructure()
{
    if(m_AccelerationStructure != nullptr)
    {
        m_DeviceProcedures.vkDestroyAccelerationStructureKHR(m_Device.Handle(), m_AccelerationStructure, nullptr);
        m_AccelerationStructure = nullptr;
    }
}

VkAccelerationStructureBuildSizesInfoKHR AccelerationStructure::GetBuildSizes(const uint32_t* pMaxPrimitiveCounts) const
{
    // Query both the size of the finished acceleration structure and the amount of scratch memory needed.
    VkAccelerationStructureBuildSizesInfoKHR sizeInfo = {};
    sizeInfo.sType                                    = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_BUILD_SIZES_INFO_KHR;

    m_DeviceProcedures.vkGetAccelerationStructureBuildSizesKHR(
        m_Device.Handle(),
        VK_ACCELERATION_STRUCTURE_BUILD_TYPE_DEVICE_KHR,
        &m_BuildGeometryInfo,
        pMaxPrimitiveCounts,
        &sizeInfo);

    // AccelerationStructure offset needs to be 256 bytes aligned (official Vulkan specs, don't ask me why).
    constexpr uint64_t accelerationStructureAlignment = 256;
    const uint64_t     scratchAlignment               = m_RayTracingProperties.MinAccelerationStructureScratchOffsetAlignment();

    sizeInfo.accelerationStructureSize = RoundUp(sizeInfo.accelerationStructureSize, accelerationStructureAlignment);
    sizeInfo.buildScratchSize          = RoundUp(sizeInfo.buildScratchSize, scratchAlignment);

    return sizeInfo;
}

void AccelerationStructure::CreateAccelerationStructure(const Engine::Buffer& resultBuffer, const VkDeviceSize resultOffset)
{
    VkAccelerationStructureCreateInfoKHR createInfo = {};

    createInfo.sType  = VK_STRUCTURE_TYPE_ACCELERATION_STRUCTURE_CREATE_INFO_KHR;
    createInfo.pNext  = nullptr;
    createInfo.type   = m_BuildGeometryInfo.type;
    createInfo.size   = BuildSizes().accelerationStructureSize;
    createInfo.buffer = resultBuffer.Handle();
    createInfo.offset = resultOffset;

    VK_CHECK(m_DeviceProcedures.vkCreateAccelerationStructureKHR(m_Device.Handle(), &createInfo, nullptr, &m_AccelerationStructure));
}

void AccelerationStructure::MemoryBarrier(const VkCommandBuffer commandBuffer)
{
    // Wait for the builder to complete by setting a barrier on the resulting buffer. This is
    // particularly important as the construction of the top-level hierarchy may be called right
    // afterwards, before executing the command list.
    VkMemoryBarrier memoryBarrier;
    memoryBarrier.sType         = VK_STRUCTURE_TYPE_MEMORY_BARRIER;
    memoryBarrier.pNext         = nullptr;
    memoryBarrier.srcAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;
    memoryBarrier.dstAccessMask = VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        VK_PIPELINE_STAGE_ACCELERATION_STRUCTURE_BUILD_BIT_KHR,
        0, 1, &memoryBarrier, 0, nullptr, 0, nullptr);
}

}    // namespace RayTracing
