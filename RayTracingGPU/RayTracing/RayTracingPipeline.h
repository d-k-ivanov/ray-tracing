#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace Scenes
{
class Scene;
}    // namespace Scenes

namespace Render
{
class UniformBuffer;
}    // namespace Render

namespace Engine
{
class DescriptorSetManager;
class ImageView;
class PipelineLayout;
class SwapChain;
}    // namespace Engine

namespace RayTracing
{
class DeviceProcedures;
class TopLevelAccelerationStructure;

class RayTracingPipeline final
{
public:
    RayTracingPipeline(const DeviceProcedures&                   deviceProcedures,
                       const Engine::SwapChain&                  swapChain,
                       const TopLevelAccelerationStructure&      accelerationStructure,
                       const Engine::ImageView&                  accumulationImageView,
                       const Engine::ImageView&                  outputImageView,
                       const std::vector<Render::UniformBuffer>& uniformBuffers,
                       const Scenes::Scene&                      scene);
    ~RayTracingPipeline();

    RayTracingPipeline(const RayTracingPipeline&)            = delete;
    RayTracingPipeline(RayTracingPipeline&&)                 = delete;
    RayTracingPipeline& operator=(const RayTracingPipeline&) = delete;
    RayTracingPipeline& operator=(RayTracingPipeline&&)      = delete;

    VkPipeline Handle() const { return m_Pipeline; }
    uint32_t   RayGenShaderIndex() const { return m_RayGenIndex; }
    uint32_t   MissShaderIndex() const { return m_MissIndex; }
    uint32_t   TriangleHitGroupIndex() const { return m_TriangleHitGroupIndex; }
    uint32_t   ProceduralHitGroupIndex() const { return m_ProceduralHitGroupIndex; }

    VkDescriptorSet               DescriptorSet(uint32_t index) const;
    const Engine::PipelineLayout& PipelineLayout() const { return *m_PipelineLayout; }

private:
    VkPipeline               m_Pipeline{};
    const Engine::SwapChain& m_SwapChain;

    std::unique_ptr<Engine::DescriptorSetManager> m_DescriptorSetManager;
    std::unique_ptr<Engine::PipelineLayout>       m_PipelineLayout;

    uint32_t m_RayGenIndex;
    uint32_t m_MissIndex;
    uint32_t m_TriangleHitGroupIndex;
    uint32_t m_ProceduralHitGroupIndex;
};

}    // namespace RayTracing
