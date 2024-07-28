#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class Instance;
class Window;

class Surface final
{
public:
    explicit Surface(const Instance& instance);
    ~Surface();

    Surface(const Surface&)            = delete;
    Surface(Surface&&)                 = delete;
    Surface& operator=(const Surface&) = delete;
    Surface& operator=(Surface&&)      = delete;

    VkSurfaceKHR    Handle() const { return m_Surface; }
    const Instance& Instance() const { return m_Instance; }

private:
    VkSurfaceKHR          m_Surface{};
    const class Instance& m_Instance;
};

}    // namespace Engine
