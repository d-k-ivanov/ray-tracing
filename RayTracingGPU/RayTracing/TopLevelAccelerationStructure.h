#pragma once

#include "AccelerationStructure.h"

#include <Utils/Glm.h>

#include <vulkan/vulkan.h>

namespace RayTracing
{
class BottomLevelAccelerationStructure;

class TopLevelAccelerationStructure final : public AccelerationStructure
{
public:
    TopLevelAccelerationStructure(const class DeviceProcedures& deviceProcedures,
                                  const RayTracingProperties&   rayTracingProperties,
                                  VkDeviceAddress               instanceAddress,
                                  uint32_t                      instancesCount);
    TopLevelAccelerationStructure(TopLevelAccelerationStructure&& other) noexcept;
    ~TopLevelAccelerationStructure() override;

    TopLevelAccelerationStructure(const TopLevelAccelerationStructure&)            = delete;
    TopLevelAccelerationStructure& operator=(const TopLevelAccelerationStructure&) = delete;
    TopLevelAccelerationStructure& operator=(TopLevelAccelerationStructure&&)      = delete;

    void Generate(VkCommandBuffer commandBuffer,
                  Engine::Buffer& scratchBuffer,
                  VkDeviceSize    scratchOffset,
                  Engine::Buffer& resultBuffer,
                  VkDeviceSize    resultOffset);

    static VkAccelerationStructureInstanceKHR CreateInstance(const BottomLevelAccelerationStructure& bottomLevelAs,
                                                             const glm::mat4&                        transform,
                                                             uint32_t                                instanceId,
                                                             uint32_t                                hitGroupId);

private:
    uint32_t m_InstancesCount;

    VkAccelerationStructureGeometryInstancesDataKHR m_InstancesVk{};
    VkAccelerationStructureGeometryKHR              m_TopAsGeometry{};
};

}    // namespace RayTracing
