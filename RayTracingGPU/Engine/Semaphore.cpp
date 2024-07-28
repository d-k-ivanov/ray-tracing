#include "Semaphore.h"
#include "Device.h"

#include <Utils/VulkanException.h>

namespace Engine
{

Semaphore::Semaphore(const class Device& device)
    : m_Device(device)
{
    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType                 = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VK_CHECK(vkCreateSemaphore(device.Handle(), &semaphoreInfo, nullptr, &m_Semaphore));
}

Semaphore::Semaphore(Semaphore&& other) noexcept
    : m_Semaphore(other.m_Semaphore)
    , m_Device(other.m_Device)
{
    other.m_Semaphore = nullptr;
}

Semaphore::~Semaphore()
{
    if(m_Semaphore != nullptr)
    {
        vkDestroySemaphore(m_Device.Handle(), m_Semaphore, nullptr);
        m_Semaphore = nullptr;
    }
}

}    // namespace Engine
