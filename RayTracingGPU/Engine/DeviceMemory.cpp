#include "DeviceMemory.h"

#include "Device.h"

#include <Utils/VulkanException.h>

namespace Engine
{

DeviceMemory::DeviceMemory(
    const class Device&         device,
    const size_t                size,
    const uint32_t              memoryTypeBits,
    const VkMemoryAllocateFlags allocateFLags,
    const VkMemoryPropertyFlags propertyFlags)
    : m_Device(device)
{
    VkMemoryAllocateFlagsInfo flagsInfo = {};
    flagsInfo.sType                     = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_FLAGS_INFO;
    flagsInfo.pNext                     = nullptr;
    flagsInfo.flags                     = allocateFLags;

    VkMemoryAllocateInfo allocInfo = {};
    allocInfo.sType                = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.pNext                = &flagsInfo;
    allocInfo.allocationSize       = size;
    allocInfo.memoryTypeIndex      = FindMemoryType(memoryTypeBits, propertyFlags);

    VK_CHECK(vkAllocateMemory(device.Handle(), &allocInfo, nullptr, &m_Memory));
}

DeviceMemory::DeviceMemory(DeviceMemory&& other) noexcept
    : m_Memory(other.m_Memory)
    , m_Device(other.m_Device)
{
    other.m_Memory = nullptr;
}

DeviceMemory::~DeviceMemory()
{
    if(m_Memory != nullptr)
    {
        vkFreeMemory(m_Device.Handle(), m_Memory, nullptr);
        m_Memory = nullptr;
    }
}

void* DeviceMemory::Map(const size_t offset, const size_t size) const
{
    void* data;
    VK_CHECK(vkMapMemory(m_Device.Handle(), m_Memory, offset, size, 0, &data));

    return data;
}

void DeviceMemory::Unmap() const
{
    vkUnmapMemory(m_Device.Handle(), m_Memory);
}

uint32_t DeviceMemory::FindMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags propertyFlags) const
{
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(m_Device.PhysicalDevice(), &memProperties);

    for(uint32_t i = 0; i != memProperties.memoryTypeCount; ++i)
    {
        if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & propertyFlags) == propertyFlags)
        {
            return i;
        }
    }

    VK_THROW("failed to find suitable memory type");
}

}    // namespace Engine
