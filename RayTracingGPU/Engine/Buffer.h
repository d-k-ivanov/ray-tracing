#pragma once

#include "DeviceMemory.h"

#include <vulkan/vulkan.h>

namespace Engine
{
class CommandPool;
class Device;

class Buffer final
{
public:
    Buffer(const Device& device, size_t size, VkBufferUsageFlags usage);
    ~Buffer();

    Buffer(const Buffer&)            = delete;
    Buffer(Buffer&&)                 = delete;
    Buffer& operator=(const Buffer&) = delete;
    Buffer& operator=(Buffer&&)      = delete;

    const Device& Device() const { return m_Device; }

    VkBuffer             Handle() const { return m_Buffer; }
    DeviceMemory         AllocateMemory(VkMemoryPropertyFlags propertyFlags) const;
    DeviceMemory         AllocateMemory(VkMemoryAllocateFlags allocateFlags, VkMemoryPropertyFlags propertyFlags) const;
    VkMemoryRequirements GetMemoryRequirements() const;
    VkDeviceAddress      GetDeviceAddress() const;

    void CopyFrom(CommandPool& commandPool, const Buffer& src, VkDeviceSize size) const;

private:
    VkBuffer            m_Buffer{};
    const class Device& m_Device;
};

}    // namespace Engine
