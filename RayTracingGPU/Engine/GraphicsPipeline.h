#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace Scenes
{
class Scene;
}

namespace Render
{
class UniformBuffer;
}

namespace Engine
{
class DepthBuffer;
class PipelineLayout;
class RenderPass;
class SwapChain;

class GraphicsPipeline final
{
public:
    GraphicsPipeline(const SwapChain&                          swapChain,
                     const DepthBuffer&                        depthBuffer,
                     const std::vector<Render::UniformBuffer>& uniformBuffers,
                     const Scenes::Scene&                      scene,
                     bool                                      isWireFrame);
    ~GraphicsPipeline();

    GraphicsPipeline(const GraphicsPipeline&)            = delete;
    GraphicsPipeline(GraphicsPipeline&&)                 = delete;
    GraphicsPipeline& operator=(const GraphicsPipeline&) = delete;
    GraphicsPipeline& operator=(GraphicsPipeline&&)      = delete;

    VkPipeline      Handle() const { return m_Pipeline; }
    VkDescriptorSet DescriptorSet(uint32_t index) const;
    bool            IsWireFrame() const { return m_IsWireFrame; }

    const PipelineLayout& PipelineLayout() const { return *m_PipelineLayout; }
    const RenderPass&     RenderPass() const { return *m_RenderPass; }

private:
    VkPipeline       m_Pipeline{};
    const SwapChain& m_SwapChain;
    const bool       m_IsWireFrame;

    std::unique_ptr<class DescriptorSetManager> m_DescriptorSetManager;
    std::unique_ptr<class PipelineLayout>       m_PipelineLayout;
    std::unique_ptr<class RenderPass>           m_RenderPass;
};

}    // namespace Engine
