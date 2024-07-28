#include "CommandPool.h"
#include "Device.h"

#include <Utils/VulkanException.h>

namespace Engine
{

CommandPool::CommandPool(const class Device& device, const uint32_t queueFamilyIndex, const bool allowReset)
    : m_Device(device)
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType                   = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex        = queueFamilyIndex;
    poolInfo.flags                   = allowReset ? VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT : 0;

    VK_CHECK(vkCreateCommandPool(device.Handle(), &poolInfo, nullptr, &m_CommandPool));
}

CommandPool::~CommandPool()
{
    if(m_CommandPool != nullptr)
    {
        vkDestroyCommandPool(m_Device.Handle(), m_CommandPool, nullptr);
        m_CommandPool = nullptr;
    }
}

}    // namespace Engine
