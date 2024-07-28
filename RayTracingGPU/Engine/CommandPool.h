#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class Device;

class CommandPool final
{
public:
    CommandPool(const Device& device, uint32_t queueFamilyIndex, bool allowReset);
    ~CommandPool();

    CommandPool(const CommandPool&)            = delete;
    CommandPool(CommandPool&&)                 = delete;
    CommandPool& operator=(const CommandPool&) = delete;
    CommandPool& operator=(CommandPool&&)      = delete;

    VkCommandPool Handle() const { return m_CommandPool; }
    const Device& Device() const { return m_Device; }

private:
    VkCommandPool       m_CommandPool{};
    const class Device& m_Device;
};

}    // namespace Engine
