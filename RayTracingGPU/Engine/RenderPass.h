#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class DepthBuffer;
class SwapChain;

class RenderPass final
{
public:
    RenderPass(const SwapChain& swapChain, const DepthBuffer& depthBuffer, VkAttachmentLoadOp colorBufferLoadOp, VkAttachmentLoadOp depthBufferLoadOp);
    ~RenderPass();

    RenderPass(const RenderPass&)            = delete;
    RenderPass(RenderPass&&)                 = delete;
    RenderPass& operator=(const RenderPass&) = delete;
    RenderPass& operator=(RenderPass&&)      = delete;

    VkRenderPass       Handle() const { return m_RenderPass; }
    const SwapChain&   SwapChain() const { return m_SwapChain; }
    const DepthBuffer& DepthBuffer() const { return m_DepthBuffer; }

private:
    VkRenderPass             m_RenderPass{};
    const class SwapChain&   m_SwapChain;
    const class DepthBuffer& m_DepthBuffer;
};

}    // namespace Engine
