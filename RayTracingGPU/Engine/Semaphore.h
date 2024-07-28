#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class Device;

class Semaphore final
{
public:
    explicit Semaphore(const Device& device);
    Semaphore(Semaphore&& other) noexcept;
    ~Semaphore();

    Semaphore(const Semaphore&)            = delete;
    Semaphore& operator=(const Semaphore&) = delete;
    Semaphore& operator=(Semaphore&&)      = delete;

    VkSemaphore   Handle() const { return m_Semaphore; }
    const Device& Device() const { return m_Device; }

private:
    VkSemaphore         m_Semaphore{};
    const class Device& m_Device;
};

}    // namespace Engine
