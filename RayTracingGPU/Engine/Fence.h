#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class Device;

class Fence final
{
public:
    explicit Fence(const Device& device, bool signaled);
    Fence(Fence&& other) noexcept;
    ~Fence();

    Fence(const Fence&)            = delete;
    Fence& operator=(const Fence&) = delete;
    Fence& operator=(Fence&&)      = delete;

    const Device&  Device() const { return m_Device; }
    const VkFence& Handle() const { return m_Fence; }

    void Reset() const;
    void Wait(uint64_t timeout) const;

private:
    const class Device& m_Device;

    VkFence m_Fence{};
};

}    // namespace Engine
