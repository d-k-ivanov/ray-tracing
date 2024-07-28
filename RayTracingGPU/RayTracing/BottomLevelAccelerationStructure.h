#pragma once

#include "AccelerationStructure.h"
#include "BottomLevelGeometry.h"

#include <vulkan/vulkan.h>

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

class BottomLevelAccelerationStructure final : public AccelerationStructure
{
public:
    BottomLevelAccelerationStructure(const class DeviceProcedures& deviceProcedures, const RayTracingProperties& rayTracingProperties, BottomLevelGeometry geometries);
    BottomLevelAccelerationStructure(BottomLevelAccelerationStructure&& other) noexcept;
    ~BottomLevelAccelerationStructure() override = default;

    BottomLevelAccelerationStructure(const BottomLevelAccelerationStructure&)            = delete;
    BottomLevelAccelerationStructure& operator=(const BottomLevelAccelerationStructure&) = delete;
    BottomLevelAccelerationStructure& operator=(BottomLevelAccelerationStructure&&)      = delete;

    void Generate(VkCommandBuffer commandBuffer, const Engine::Buffer& scratchBuffer, VkDeviceSize scratchOffset, const Engine::Buffer& resultBuffer, VkDeviceSize resultOffset);

private:
    BottomLevelGeometry m_Geometries;
};

}    // namespace RayTracing
