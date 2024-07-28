#include "ImageView.h"
#include "Device.h"

#include <Utils/VulkanException.h>

namespace Engine
{

ImageView::ImageView(const class Device& device, const VkImage image, const VkFormat format, const VkImageAspectFlags aspectFlags)
    : m_Device(device)
    , m_Image(image)
    , m_Format(format)
{
    VkImageViewCreateInfo createInfo           = {};
    createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    createInfo.image                           = image;
    createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
    createInfo.format                          = format;
    createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
    createInfo.subresourceRange.aspectMask     = aspectFlags;
    createInfo.subresourceRange.baseMipLevel   = 0;
    createInfo.subresourceRange.levelCount     = 1;
    createInfo.subresourceRange.baseArrayLayer = 0;
    createInfo.subresourceRange.layerCount     = 1;

    VK_CHECK(vkCreateImageView(m_Device.Handle(), &createInfo, nullptr, &m_ImageView));
}

ImageView::~ImageView()
{
    if(m_ImageView != nullptr)
    {
        vkDestroyImageView(m_Device.Handle(), m_ImageView, nullptr);
        m_ImageView = nullptr;
    }
}

}    // namespace Engine
