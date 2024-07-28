#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class Device;
}    // namespace Engine

namespace RayTracing
{
class RayTracingProperties final
{
public:
    explicit RayTracingProperties(const Engine::Device& device);

    const Engine::Device& Device() const { return m_Device; }

    uint32_t MaxDescriptorSetAccelerationStructures() const { return m_AccelProps.maxDescriptorSetAccelerationStructures; }
    uint64_t MaxGeometryCount() const { return m_AccelProps.maxGeometryCount; }
    uint64_t MaxInstanceCount() const { return m_AccelProps.maxInstanceCount; }
    uint64_t MaxPrimitiveCount() const { return m_AccelProps.maxPrimitiveCount; }
    uint32_t MaxRayRecursionDepth() const { return m_PipelineProps.maxRayRecursionDepth; }
    uint32_t MaxShaderGroupStride() const { return m_PipelineProps.maxShaderGroupStride; }
    uint32_t MinAccelerationStructureScratchOffsetAlignment() const { return m_AccelProps.minAccelerationStructureScratchOffsetAlignment; }
    uint32_t ShaderGroupBaseAlignment() const { return m_PipelineProps.shaderGroupBaseAlignment; }
    uint32_t ShaderGroupHandleCaptureReplaySize() const { return m_PipelineProps.shaderGroupHandleCaptureReplaySize; }
    uint32_t ShaderGroupHandleSize() const { return m_PipelineProps.shaderGroupHandleSize; }

private:
    const Engine::Device& m_Device;

    VkPhysicalDeviceAccelerationStructurePropertiesKHR m_AccelProps{};
    VkPhysicalDeviceRayTracingPipelinePropertiesKHR    m_PipelineProps{};
};

}    // namespace RayTracing
