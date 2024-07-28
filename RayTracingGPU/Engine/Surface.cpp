#include "Surface.h"
#include "Instance.h"
#include "Window.h"

#include <Utils/VulkanException.h>

namespace Engine
{

Surface::Surface(const class Instance& instance)
    : m_Instance(instance)
{
    VK_CHECK(glfwCreateWindowSurface(instance.Handle(), instance.Window().Handle(), nullptr, &m_Surface));
}

Surface::~Surface()
{
    if(m_Surface != nullptr)
    {
        vkDestroySurfaceKHR(m_Instance.Handle(), m_Surface, nullptr);
        m_Surface = nullptr;
    }
}

}    // namespace Engine
