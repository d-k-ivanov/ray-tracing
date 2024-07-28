#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Engine
{
class CommandPool;

class CommandBuffers final
{
public:
    CommandBuffers(CommandPool& commandPool, uint32_t size);
    ~CommandBuffers();

    CommandBuffers(const CommandBuffers&)            = delete;
    CommandBuffers(CommandBuffers&&)                 = delete;
    CommandBuffers& operator=(const CommandBuffers&) = delete;
    CommandBuffers& operator=(CommandBuffers&&)      = delete;

    uint32_t         Size() const { return static_cast<uint32_t>(m_CommandBuffers.size()); }
    VkCommandBuffer& operator[](const size_t i) { return m_CommandBuffers[i]; }

    VkCommandBuffer Begin(size_t i) const;
    void            End(size_t);

private:
    const CommandPool& m_CommandPool;

    std::vector<VkCommandBuffer> m_CommandBuffers;
};

}    // namespace Engine
