#pragma once

#include "DeviceMemory.h"

#include <vulkan/vulkan.h>

namespace Engine
{
class Buffer;
class CommandPool;
class Device;

class Image final
{
public:
    Image(const Device& device, VkExtent2D extent, VkFormat format);
    Image(const Device& device, VkExtent2D extent, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage);
    Image(Image&& other) noexcept;
    ~Image();

    Image(const Image&)            = delete;
    Image& operator=(const Image&) = delete;
    Image& operator=(Image&&)      = delete;

    VkImage       Handle() const { return m_Image; }
    const Device& Device() const { return m_Device; }
    VkExtent2D    Extent() const { return m_Extent; }
    VkFormat      Format() const { return m_Format; }

    DeviceMemory         AllocateMemory(VkMemoryPropertyFlags properties) const;
    VkMemoryRequirements GetMemoryRequirements() const;

    void TransitionImageLayout(CommandPool& commandPool, VkImageLayout newLayout);
    void CopyFrom(CommandPool& commandPool, const Buffer& buffer) const;

private:
    VkImage             m_Image{};
    const class Device& m_Device;
    const VkExtent2D    m_Extent;
    const VkFormat      m_Format;
    VkImageLayout       m_ImageLayout;
};

}    // namespace Engine
