#include "Fence.h"
#include "Device.h"

#include <Utils/VulkanException.h>

namespace Engine
{

Fence::Fence(const class Device& device, const bool signaled)
    : m_Device(device)
{
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType             = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags             = signaled ? VK_FENCE_CREATE_SIGNALED_BIT : 0;

    VK_CHECK(vkCreateFence(device.Handle(), &fenceInfo, nullptr, &m_Fence));
}

Fence::Fence(Fence&& other) noexcept
    : m_Device(other.m_Device)
    , m_Fence(other.m_Fence)
{
    other.m_Fence = nullptr;
}

Fence::~Fence()
{
    if(m_Fence != nullptr)
    {
        vkDestroyFence(m_Device.Handle(), m_Fence, nullptr);
        m_Fence = nullptr;
    }
}

void Fence::Reset() const
{
    VK_CHECK(vkResetFences(m_Device.Handle(), 1, &m_Fence));
}

void Fence::Wait(const uint64_t timeout) const
{
    VK_CHECK(vkWaitForFences(m_Device.Handle(), 1, &m_Fence, VK_TRUE, timeout));
}

}    // namespace Engine
