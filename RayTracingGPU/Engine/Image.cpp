#include "Image.h"

#include "Buffer.h"
#include "DepthBuffer.h"
#include "Device.h"
#include "SingleTimeCommands.h"

#include <Utils/VulkanException.h>

namespace Engine
{

Image::Image(const class Device& device, const VkExtent2D extent, const VkFormat format)
    : Image(device, extent, format, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT)
{
}

Image::Image(
    const class Device&     device,
    const VkExtent2D        extent,
    const VkFormat          format,
    const VkImageTiling     tiling,
    const VkImageUsageFlags usage)
    : m_Device(device)
    , m_Extent(extent)
    , m_Format(format)
    , m_ImageLayout(VK_IMAGE_LAYOUT_UNDEFINED)
{
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType         = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width      = extent.width;
    imageInfo.extent.height     = extent.height;
    imageInfo.extent.depth      = 1;
    imageInfo.mipLevels         = 1;
    imageInfo.arrayLayers       = 1;
    imageInfo.format            = format;
    imageInfo.tiling            = tiling;
    imageInfo.initialLayout     = m_ImageLayout;
    imageInfo.usage             = usage;
    imageInfo.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples           = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.flags             = 0;    // Optional

    VK_CHECK(vkCreateImage(device.Handle(), &imageInfo, nullptr, &m_Image));
}

Image::Image(Image&& other) noexcept
    : m_Image(other.m_Image)
    , m_Device(other.m_Device)
    , m_Extent(other.m_Extent)
    , m_Format(other.m_Format)
    , m_ImageLayout(other.m_ImageLayout)
{
    other.m_Image = nullptr;
}

Image::~Image()
{
    if(m_Image != nullptr)
    {
        vkDestroyImage(m_Device.Handle(), m_Image, nullptr);
        m_Image = nullptr;
    }
}

DeviceMemory Image::AllocateMemory(const VkMemoryPropertyFlags properties) const
{
    const auto   requirements = GetMemoryRequirements();
    DeviceMemory memory(m_Device, requirements.size, requirements.memoryTypeBits, 0, properties);

    VK_CHECK(vkBindImageMemory(m_Device.Handle(), m_Image, memory.Handle(), 0));

    return memory;
}

VkMemoryRequirements Image::GetMemoryRequirements() const
{
    VkMemoryRequirements requirements;
    vkGetImageMemoryRequirements(m_Device.Handle(), m_Image, &requirements);
    return requirements;
}

void Image::TransitionImageLayout(CommandPool& commandPool, const VkImageLayout newLayout)
{
    SingleTimeCommands::Submit(commandPool, [&](const VkCommandBuffer commandBuffer)
    {
        VkImageMemoryBarrier barrier            = {};
        barrier.sType                           = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout                       = m_ImageLayout;
        barrier.newLayout                       = newLayout;
        barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
        barrier.image                           = m_Image;
        barrier.subresourceRange.baseMipLevel   = 0;
        barrier.subresourceRange.levelCount     = 1;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount     = 1;

        if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

            if(DepthBuffer::HasStencilComponent(m_Format))
            {
                barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
            }
        }
        else
        {
            barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        }

        VkPipelineStageFlags sourceStage;
        VkPipelineStageFlags destinationStage;

        if(m_ImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        }
        else if(m_ImageLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TRANSFER_BIT;
            destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else if(m_ImageLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

            sourceStage      = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
        }
        else
        {
            VK_THROW("unsupported layout transition");
        }

        vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
    });

    m_ImageLayout = newLayout;
}

void Image::CopyFrom(CommandPool& commandPool, const Buffer& buffer) const
{
    SingleTimeCommands::Submit(commandPool, [&](const VkCommandBuffer commandBuffer)
    {
        VkBufferImageCopy region               = {};
        region.bufferOffset                    = 0;
        region.bufferRowLength                 = 0;
        region.bufferImageHeight               = 0;
        region.imageSubresource.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel       = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount     = 1;
        region.imageOffset                     = {0, 0, 0};
        region.imageExtent                     = {m_Extent.width, m_Extent.height, 1};

        vkCmdCopyBufferToImage(commandBuffer, buffer.Handle(), m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    });
}

}    // namespace Engine
