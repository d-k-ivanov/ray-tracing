#include "Buffer.h"

#include "SingleTimeCommands.h"

#include <Utils/VulkanException.h>

namespace Engine
{

Buffer::Buffer(const class Device& device, const size_t size, const VkBufferUsageFlags usage)
    : m_Device(device)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType              = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size               = size;
    bufferInfo.usage              = usage;
    bufferInfo.sharingMode        = VK_SHARING_MODE_EXCLUSIVE;

    VK_CHECK(vkCreateBuffer(device.Handle(), &bufferInfo, nullptr, &m_Buffer));
}

Buffer::~Buffer()
{
    if(m_Buffer != nullptr)
    {
        vkDestroyBuffer(m_Device.Handle(), m_Buffer, nullptr);
        m_Buffer = nullptr;
    }
}

DeviceMemory Buffer::AllocateMemory(const VkMemoryPropertyFlags propertyFlags) const
{
    return AllocateMemory(0, propertyFlags);
}

DeviceMemory Buffer::AllocateMemory(const VkMemoryAllocateFlags allocateFlags, const VkMemoryPropertyFlags propertyFlags) const
{
    const auto   requirements = GetMemoryRequirements();
    DeviceMemory memory(m_Device, requirements.size, requirements.memoryTypeBits, allocateFlags, propertyFlags);

    VK_CHECK(vkBindBufferMemory(m_Device.Handle(), m_Buffer, memory.Handle(), 0));

    return memory;
}

VkMemoryRequirements Buffer::GetMemoryRequirements() const
{
    VkMemoryRequirements requirements;
    vkGetBufferMemoryRequirements(m_Device.Handle(), m_Buffer, &requirements);
    return requirements;
}

VkDeviceAddress Buffer::GetDeviceAddress() const
{
    VkBufferDeviceAddressInfo info = {};
    info.sType                     = VK_STRUCTURE_TYPE_BUFFER_DEVICE_ADDRESS_INFO;
    info.pNext                     = nullptr;
    info.buffer                    = Handle();

    return vkGetBufferDeviceAddress(m_Device.Handle(), &info);
}

void Buffer::CopyFrom(CommandPool& commandPool, const Buffer& src, const VkDeviceSize size) const
{
    SingleTimeCommands::Submit(commandPool, [&](const VkCommandBuffer commandBuffer)
    {
        VkBufferCopy copyRegion = {};
        copyRegion.srcOffset    = 0;    // Optional
        copyRegion.dstOffset    = 0;    // Optional
        copyRegion.size         = size;

        vkCmdCopyBuffer(commandBuffer, src.Handle(), Handle(), 1, &copyRegion);
    });
}

}    // namespace Engine
