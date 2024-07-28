#include "CommandBuffers.h"
#include "CommandPool.h"
#include "Device.h"

#include <Utils/VulkanException.h>

namespace Engine
{

CommandBuffers::CommandBuffers(CommandPool& commandPool, const uint32_t size)
    : m_CommandPool(commandPool)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool                 = commandPool.Handle();
    allocInfo.level                       = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount          = size;

    m_CommandBuffers.resize(size);

    VK_CHECK(vkAllocateCommandBuffers(commandPool.Device().Handle(), &allocInfo, m_CommandBuffers.data()));
}

CommandBuffers::~CommandBuffers()
{
    if(!m_CommandBuffers.empty())
    {
        vkFreeCommandBuffers(m_CommandPool.Device().Handle(), m_CommandPool.Handle(), static_cast<uint32_t>(m_CommandBuffers.size()), m_CommandBuffers.data());
        m_CommandBuffers.clear();
    }
}

VkCommandBuffer CommandBuffers::Begin(const size_t i) const
{
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType                    = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags                    = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
    beginInfo.pInheritanceInfo         = nullptr;    // Optional

    VK_CHECK(vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo));

    return m_CommandBuffers[i];
}

void CommandBuffers::End(const size_t i)
{
    VK_CHECK(vkEndCommandBuffer(m_CommandBuffers[i]));
}

}    // namespace Engine
