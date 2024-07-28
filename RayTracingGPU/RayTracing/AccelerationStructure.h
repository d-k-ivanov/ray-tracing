#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class Buffer;
class Device;
class DeviceMemory;
}

namespace RayTracing
{
class DeviceProcedures;

class AccelerationStructure
{
public:
    AccelerationStructure(AccelerationStructure&& other) noexcept;
    virtual ~AccelerationStructure();

    AccelerationStructure(const AccelerationStructure&)            = delete;
    AccelerationStructure& operator=(const AccelerationStructure&) = delete;
    AccelerationStructure& operator=(AccelerationStructure&&)      = delete;

    VkAccelerationStructureKHR Handle() const { return m_AccelerationStructure; }
    const Engine::Device&      Device() const { return m_Device; }
    const DeviceProcedures&    DeviceProcedures() const { return m_DeviceProcedures; }

    VkAccelerationStructureBuildSizesInfoKHR BuildSizes() const { return m_BuildSizesInfo; }

    static void MemoryBarrier(VkCommandBuffer commandBuffer);

protected:
    explicit AccelerationStructure(const class DeviceProcedures& deviceProcedures, const class RayTracingProperties& rayTracingProperties);

    VkAccelerationStructureBuildSizesInfoKHR GetBuildSizes(const uint32_t* pMaxPrimitiveCounts) const;

    void CreateAccelerationStructure(const Engine::Buffer& resultBuffer, VkDeviceSize resultOffset);

    const class DeviceProcedures&              m_DeviceProcedures;
    const VkBuildAccelerationStructureFlagsKHR m_Flags;

    VkAccelerationStructureBuildGeometryInfoKHR m_BuildGeometryInfo{};
    VkAccelerationStructureBuildSizesInfoKHR    m_BuildSizesInfo{};

private:
    VkAccelerationStructureKHR  m_AccelerationStructure{};
    const Engine::Device&       m_Device;
    const RayTracingProperties& m_RayTracingProperties;
};

}    // namespace RayTracing
