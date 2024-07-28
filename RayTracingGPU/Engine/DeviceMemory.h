#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class Device;

class DeviceMemory final
{
public:
    DeviceMemory(const Device& device, size_t size, uint32_t memoryTypeBits, VkMemoryAllocateFlags allocateFLags, VkMemoryPropertyFlags propertyFlags);
    DeviceMemory(DeviceMemory&& other) noexcept;
    ~DeviceMemory();

    DeviceMemory(const DeviceMemory&)            = delete;
    DeviceMemory& operator=(const DeviceMemory&) = delete;
    DeviceMemory& operator=(DeviceMemory&&)      = delete;

    VkDeviceMemory Handle() const { return m_Memory; }
    const Device&  Device() const { return m_Device; }

    void* Map(size_t offset, size_t size) const;
    void  Unmap() const;

private:
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags propertyFlags) const;

    VkDeviceMemory      m_Memory{};
    const class Device& m_Device;
};

}    // namespace Engine
