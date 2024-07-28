#include "Image.h"
#include "Application.h"

#include <imgui.h>
#include <imgui_impl_vulkan.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "Utils/VulkanException.h"

namespace Engine
{

namespace Impl
{

    uint32_t GetVulkanMemoryType(const VkMemoryPropertyFlags properties, const uint32_t typeBits)
    {
        VkPhysicalDeviceMemoryProperties vkProps;
        vkGetPhysicalDeviceMemoryProperties(Application::GetPhysicalDevice(), &vkProps);
        for(uint32_t i = 0; i < vkProps.memoryTypeCount; i++)
        {
            if((vkProps.memoryTypes[i].propertyFlags & properties) == properties && typeBits & (1 << i))
                return i;
        }

        return 0xffffffff;
    }

    uint32_t BytesPerPixel(const ImageFormat format)
    {
        switch(format)
        {
            case ImageFormat::RGBA:
                return 4;
            case ImageFormat::RGBA32F:
                return 16;
            case ImageFormat::None:
                return 0;
        }
        return 0;
    }

    VkFormat FormatToVulkanFormat(const ImageFormat format)
    {
        switch(format)
        {
            case ImageFormat::RGBA:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case ImageFormat::RGBA32F:
                return VK_FORMAT_R32G32B32A32_SFLOAT;
            case ImageFormat::None:
                return static_cast<VkFormat>(0);
        }
        return static_cast<VkFormat>(0);
    }

    // Return the value clamped to the range [low, high).
    uint32_t Clamp(const uint32_t x, const uint32_t low, const uint32_t high)
    {
        if(x < low)
            return low;
        if(x < high)
            return x;
        return high - 1;
    }

}    // namespace Impl

Image::Image(const std::string_view path, const bool storeImageData)
    : m_Filepath(path)
{
    int      width, height, channels;
    uint8_t* data;

    if(stbi_is_hdr(m_Filepath.c_str()))
    {
        data     = reinterpret_cast<uint8_t*>(stbi_loadf(m_Filepath.c_str(), &width, &height, &channels, 4));
        m_Format = ImageFormat::RGBA32F;
    }
    else
    {
        data     = stbi_load(m_Filepath.c_str(), &width, &height, &channels, 4);
        m_Format = ImageFormat::RGBA;
    }

    m_Width  = width;
    m_Height = height;

    AllocateMemory(static_cast<int64_t>(m_Width) * m_Height * Impl::BytesPerPixel(m_Format));
    SetData(data);
    if(storeImageData)
    {
        m_Data = data;
    }
    else
    {
        stbi_image_free(data);
    }
}

Image::Image(const uint32_t width, const uint32_t height, const ImageFormat format, const void* data)
    : m_Width(width)
    , m_Height(height)
    , m_Format(format)
{
    AllocateMemory(static_cast<int64_t>(m_Width) * m_Height * Impl::BytesPerPixel(m_Format));
    if(data)
        SetData(data);
}

Image::~Image()
{
    Release();
}

void Image::SetData(const void* data)
{
    VkDevice device     = Application::GetDevice();
    size_t   uploadSize = static_cast<size_t>(m_Width) * m_Height * Impl::BytesPerPixel(m_Format);

    if(!m_StagingBuffer)
    {
        // Create the Upload Buffer
        {
            VkBufferCreateInfo bufferInfo = {};
            bufferInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
            bufferInfo.size               = uploadSize;
            bufferInfo.usage              = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
            bufferInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;
            VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &m_StagingBuffer));
            VkMemoryRequirements req;
            vkGetBufferMemoryRequirements(device, m_StagingBuffer, &req);
            m_AlignedSize                  = req.size;
            VkMemoryAllocateInfo allocInfo = {};
            allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
            allocInfo.allocationSize       = req.size;
            allocInfo.memoryTypeIndex      = Impl::GetVulkanMemoryType(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT, req.memoryTypeBits);
            VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &m_StagingBufferMemory));
            VK_CHECK(vkBindBufferMemory(device, m_StagingBuffer, m_StagingBufferMemory, 0));
        }
    }

    // Upload to Buffer
    {
        char* map = nullptr;
        VK_CHECK(vkMapMemory(device, m_StagingBufferMemory, 0, m_AlignedSize, 0, reinterpret_cast<void**>(&map)));
        memcpy(map, data, uploadSize);
        VkMappedMemoryRange range[1] = {};
        range[0].sType               = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
        range[0].memory              = m_StagingBufferMemory;
        range[0].size                = m_AlignedSize;
        VK_CHECK(vkFlushMappedMemoryRanges(device, 1, range));
        vkUnmapMemory(device, m_StagingBufferMemory);
    }

    // Copy to Image
    {
        VkCommandBuffer commandBuffer = Application::GetCommandBuffer(true);

        VkImageMemoryBarrier copyBarrier        = {};
        copyBarrier.sType                       = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        copyBarrier.dstAccessMask               = VK_ACCESS_TRANSFER_WRITE_BIT;
        copyBarrier.oldLayout                   = VK_IMAGE_LAYOUT_UNDEFINED;
        copyBarrier.newLayout                   = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        copyBarrier.srcQueueFamilyIndex         = VK_QUEUE_FAMILY_IGNORED;
        copyBarrier.dstQueueFamilyIndex         = VK_QUEUE_FAMILY_IGNORED;
        copyBarrier.image                       = m_Image;
        copyBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        copyBarrier.subresourceRange.levelCount = 1;
        copyBarrier.subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_HOST_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0, 0, nullptr, 0, nullptr, 1, &copyBarrier);

        VkBufferImageCopy region           = {};
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.layerCount = 1;
        region.imageExtent.width           = m_Width;
        region.imageExtent.height          = m_Height;
        region.imageExtent.depth           = 1;
        vkCmdCopyBufferToImage(commandBuffer, m_StagingBuffer, m_Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

        VkImageMemoryBarrier useBarrier        = {};
        useBarrier.sType                       = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        useBarrier.srcAccessMask               = VK_ACCESS_TRANSFER_WRITE_BIT;
        useBarrier.dstAccessMask               = VK_ACCESS_SHADER_READ_BIT;
        useBarrier.oldLayout                   = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        useBarrier.newLayout                   = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        useBarrier.srcQueueFamilyIndex         = VK_QUEUE_FAMILY_IGNORED;
        useBarrier.dstQueueFamilyIndex         = VK_QUEUE_FAMILY_IGNORED;
        useBarrier.image                       = m_Image;
        useBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        useBarrier.subresourceRange.levelCount = 1;
        useBarrier.subresourceRange.layerCount = 1;
        vkCmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0, 0, nullptr, 0, nullptr, 1, &useBarrier);

        Application::FlushCommandBuffer(commandBuffer);
    }
}

void Image::Resize(const uint32_t width, const uint32_t height)
{
    if(m_Image && m_Width == width && m_Height == height)
        return;

    m_Width  = width;
    m_Height = height;

    Release();
    AllocateMemory(static_cast<uint64_t>(m_Width) * m_Height * Impl::BytesPerPixel(m_Format));
}

VkDescriptorSet Image::GetDescriptorSet() const
{
    return m_DescriptorSet;
}

uint32_t Image::GetWidth() const
{
    return m_Width;
}

uint32_t Image::GetHeight() const
{
    return m_Height;
}

uint8_t* Image::GetPixels() const
{
    return m_Data;
}

// Return the address of the three bytes of the pixel at x,y (or magenta if no data).
const uint8_t* Image::PixelData(uint32_t x, uint32_t y) const
{
    static uint8_t magenta[] = {255, 0, 255};
    if(m_Data == nullptr)
        return magenta;

    x = Impl::Clamp(x, 0, m_Width);
    y = Impl::Clamp(y, 0, m_Height);

    const auto verticalOffset   = y * m_Width * Impl::BytesPerPixel(m_Format);
    const auto horisontalOffset = x * Impl::BytesPerPixel(m_Format);

    return m_Data + verticalOffset + horisontalOffset;
}

void Image::AllocateMemory([[maybe_unused]] uint64_t size)
{
    VkDevice device       = Application::GetDevice();
    VkFormat vulkanFormat = Impl::FormatToVulkanFormat(m_Format);

    // Create the Image
    {
        VkImageCreateInfo info = {};
        info.sType             = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        info.imageType         = VK_IMAGE_TYPE_2D;
        info.format            = vulkanFormat;
        info.extent.width      = m_Width;
        info.extent.height     = m_Height;
        info.extent.depth      = 1;
        info.mipLevels         = 1;
        info.arrayLayers       = 1;
        info.samples           = VK_SAMPLE_COUNT_1_BIT;
        info.tiling            = VK_IMAGE_TILING_OPTIMAL;
        info.usage             = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
        info.sharingMode       = VK_SHARING_MODE_EXCLUSIVE;
        info.initialLayout     = VK_IMAGE_LAYOUT_UNDEFINED;
        VK_CHECK(vkCreateImage(device, &info, nullptr, &m_Image));
        VkMemoryRequirements req;
        vkGetImageMemoryRequirements(device, m_Image, &req);
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize       = req.size;
        allocInfo.memoryTypeIndex      = Impl::GetVulkanMemoryType(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, req.memoryTypeBits);
        VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &m_Memory));
        VK_CHECK(vkBindImageMemory(device, m_Image, m_Memory, 0));
    }

    // Create the Image View:
    {
        VkImageViewCreateInfo info       = {};
        info.sType                       = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        info.image                       = m_Image;
        info.viewType                    = VK_IMAGE_VIEW_TYPE_2D;
        info.format                      = vulkanFormat;
        info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        info.subresourceRange.levelCount = 1;
        info.subresourceRange.layerCount = 1;
        VK_CHECK(vkCreateImageView(device, &info, nullptr, &m_ImageView));
    }

    // Create Sampler:
    {
        VkSamplerCreateInfo info = {};
        info.sType               = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        info.magFilter           = VK_FILTER_LINEAR;
        info.minFilter           = VK_FILTER_LINEAR;
        info.mipmapMode          = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        info.addressModeU        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeV        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.addressModeW        = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        info.minLod              = -1000;
        info.maxLod              = 1000;
        info.maxAnisotropy       = 1.0f;
        VK_CHECK(vkCreateSampler(device, &info, nullptr, &m_Sampler));
    }

    // Create the Descriptor Set:
    m_DescriptorSet = ImGui_ImplVulkan_AddTexture(m_Sampler, m_ImageView, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

void Image::Release()
{
    Application::SubmitResourceFree(
        [sampler             = m_Sampler,
         imageView           = m_ImageView,
         image               = m_Image,
         memory              = m_Memory,
         stagingBuffer       = m_StagingBuffer,
         stagingBufferMemory = m_StagingBufferMemory]()
        {
            const VkDevice device = Application::GetDevice();
            vkDestroySampler(device, sampler, nullptr);
            vkDestroyImageView(device, imageView, nullptr);
            vkDestroyImage(device, image, nullptr);
            vkFreeMemory(device, memory, nullptr);
            vkDestroyBuffer(device, stagingBuffer, nullptr);
            vkFreeMemory(device, stagingBufferMemory, nullptr);
        });

    m_Sampler             = nullptr;
    m_ImageView           = nullptr;
    m_Image               = nullptr;
    m_Memory              = nullptr;
    m_StagingBuffer       = nullptr;
    m_StagingBufferMemory = nullptr;
}

}    // namespace Vulkan
