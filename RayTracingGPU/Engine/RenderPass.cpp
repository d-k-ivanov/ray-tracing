#include "RenderPass.h"
#include "DepthBuffer.h"
#include "Device.h"
#include "SwapChain.h"

#include <Utils/VulkanException.h>

#include <array>

namespace Engine
{

RenderPass::RenderPass(
    const class SwapChain&   swapChain,
    const class DepthBuffer& depthBuffer,
    const VkAttachmentLoadOp colorBufferLoadOp,
    const VkAttachmentLoadOp depthBufferLoadOp)
    : m_SwapChain(swapChain)
    , m_DepthBuffer(depthBuffer)
{
    VkAttachmentDescription colorAttachment = {};
    colorAttachment.format                  = swapChain.Format();
    colorAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp                  = colorBufferLoadOp;
    colorAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout           = colorBufferLoadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    colorAttachment.finalLayout             = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentDescription depthAttachment = {};
    depthAttachment.format                  = depthBuffer.Format();
    depthAttachment.samples                 = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp                  = depthBufferLoadOp;
    depthAttachment.storeOp                 = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp           = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp          = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout           = depthBufferLoadOp == VK_ATTACHMENT_LOAD_OP_CLEAR ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    depthAttachment.finalLayout             = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorAttachmentRef = {};
    colorAttachmentRef.attachment            = 0;
    colorAttachmentRef.layout                = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthAttachmentRef = {};
    depthAttachmentRef.attachment            = 1;
    depthAttachmentRef.layout                = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpass    = {};
    subpass.pipelineBindPoint       = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount    = 1;
    subpass.pColorAttachments       = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass          = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass          = 0;
    dependency.srcStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask       = 0;
    dependency.dstStageMask        = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstAccessMask       = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    std::array attachments =
        {
            colorAttachment,
            depthAttachment};

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType                  = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount        = static_cast<uint32_t>(attachments.size());
    renderPassInfo.pAttachments           = attachments.data();
    renderPassInfo.subpassCount           = 1;
    renderPassInfo.pSubpasses             = &subpass;
    renderPassInfo.dependencyCount        = 1;
    renderPassInfo.pDependencies          = &dependency;

    VK_CHECK(vkCreateRenderPass(m_SwapChain.Device().Handle(), &renderPassInfo, nullptr, &m_RenderPass));
}

RenderPass::~RenderPass()
{
    if(m_RenderPass != nullptr)
    {
        vkDestroyRenderPass(m_SwapChain.Device().Handle(), m_RenderPass, nullptr);
        m_RenderPass = nullptr;
    }
}

}    // namespace Engine
