#include "DepthBuffer.h"

#include "CommandPool.h"
#include "Device.h"
#include "DeviceMemory.h"
#include "Image.h"
#include "ImageView.h"

#include <Utils/VulkanException.h>

namespace Engine
{

namespace
{
VkFormat FindSupportedFormat(const Device& device, const std::vector<VkFormat>& candidates, const VkImageTiling tiling, const VkFormatFeatureFlags features)
{
    for(const auto format : candidates)
    {
        VkFormatProperties props;
        vkGetPhysicalDeviceFormatProperties(device.PhysicalDevice(), format, &props);

        if(tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
        {
            return format;
        }

        if(tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    VK_THROW("failed to find supported format");
}

VkFormat FindDepthFormat(const Device& device)
{
    return FindSupportedFormat(
        device,
        {VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT},
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
}
}

DepthBuffer::DepthBuffer(CommandPool& commandPool, const VkExtent2D extent)
    : m_Format(FindDepthFormat(commandPool.Device()))
{
    const auto& device = commandPool.Device();

    m_Image.reset(new Image(device, extent, m_Format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT));
    m_ImageMemory.reset(new DeviceMemory(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
    m_ImageView.reset(new class ImageView(device, m_Image->Handle(), m_Format, VK_IMAGE_ASPECT_DEPTH_BIT));

    m_Image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);

    const auto& debugUtils = device.DebugUtils();

    debugUtils.SetObjectName(m_Image->Handle(), "Depth Buffer Image");
    debugUtils.SetObjectName(m_ImageMemory->Handle(), "Depth Buffer Image Memory");
    debugUtils.SetObjectName(m_ImageView->Handle(), "Depth Buffer ImageView");
}

DepthBuffer::~DepthBuffer()
{
    m_ImageView.reset();
    m_Image.reset();
    m_ImageMemory.reset();    // release memory after bound image has been destroyed
}

}    // namespace Engine
