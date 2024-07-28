#include "FrameBuffer.h"
#include "DepthBuffer.h"
#include "Device.h"
#include "ImageView.h"
#include "RenderPass.h"
#include "SwapChain.h"

#include <Utils/VulkanException.h>

#include <array>

namespace Engine
{

FrameBuffer::FrameBuffer(const class ImageView& imageView, const class RenderPass& renderPass)
    : m_ImageView(imageView)
    , m_RenderPass(renderPass)
{
    const std::array attachments =
        {
            imageView.Handle(),
            renderPass.DepthBuffer().ImageView().Handle()};

    VkFramebufferCreateInfo framebufferInfo = {};
    framebufferInfo.sType                   = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.renderPass              = renderPass.Handle();
    framebufferInfo.attachmentCount         = static_cast<uint32_t>(attachments.size());
    framebufferInfo.pAttachments            = attachments.data();
    framebufferInfo.width                   = renderPass.SwapChain().Extent().width;
    framebufferInfo.height                  = renderPass.SwapChain().Extent().height;
    framebufferInfo.layers                  = 1;

    VK_CHECK(vkCreateFramebuffer(m_ImageView.Device().Handle(), &framebufferInfo, nullptr, &m_Framebuffer));
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
    : m_Framebuffer(other.m_Framebuffer)
    , m_ImageView(other.m_ImageView)
    , m_RenderPass(other.m_RenderPass)
{
    other.m_Framebuffer = nullptr;
}

FrameBuffer::~FrameBuffer()
{
    if(m_Framebuffer != nullptr)
    {
        vkDestroyFramebuffer(m_ImageView.Device().Handle(), m_Framebuffer, nullptr);
        m_Framebuffer = nullptr;
    }
}

}    // namespace Engine
