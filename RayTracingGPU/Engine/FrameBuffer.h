#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class ImageView;
class RenderPass;

class FrameBuffer final
{
public:
    explicit FrameBuffer(const ImageView& imageView, const RenderPass& renderPass);
    FrameBuffer(FrameBuffer&& other) noexcept;
    ~FrameBuffer();

    FrameBuffer(const FrameBuffer&)            = delete;
    FrameBuffer& operator=(const FrameBuffer&) = delete;
    FrameBuffer& operator=(FrameBuffer&&)      = delete;

    VkFramebuffer     Handle() const { return m_Framebuffer; }
    const ImageView&  ImageView() const { return m_ImageView; }
    const RenderPass& RenderPass() const { return m_RenderPass; }

private:
    VkFramebuffer           m_Framebuffer{};
    const class ImageView&  m_ImageView;
    const class RenderPass& m_RenderPass;
};

}    // namespace Engine
