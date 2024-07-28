#include "Window.h"

#include <Utils/Filesystem.h>
#include <Utils/StbImage.h>
#include <Utils/VulkanException.h>

#include <vector>

namespace Engine
{

namespace
{
void GlfwErrorCallback(const int error, const char* const description)
{
    LOG_ERROR("GLFW: {} (code: {})", description, error);
}

void GlfwKeyCallback(GLFWwindow* window, const int key, const int scancode, const int action, const int mods)
{
    const auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(this_->OnKey)
    {
        this_->OnKey(key, scancode, action, mods);
    }
}

void GlfwCursorPositionCallback(GLFWwindow* window, const double xpos, const double ypos)
{
    const auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(this_->OnCursorPosition)
    {
        this_->OnCursorPosition(xpos, ypos);
    }
}

void GlfwMouseButtonCallback(GLFWwindow* window, const int button, const int action, const int mods)
{
    const auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(this_->OnMouseButton)
    {
        this_->OnMouseButton(button, action, mods);
    }
}

void GlfwScrollCallback(GLFWwindow* window, const double xoffset, const double yoffset)
{
    const auto* const this_ = static_cast<Window*>(glfwGetWindowUserPointer(window));
    if(this_->OnScroll)
    {
        this_->OnScroll(xoffset, yoffset);
    }
}
}

Window::Window(const WindowConfig& config)
    : m_Config(config)
{
    glfwSetErrorCallback(GlfwErrorCallback);

    if(!glfwInit())
    {
        VK_THROW("glfwInit() failed");
    }

    if(!glfwVulkanSupported())
    {
        VK_THROW("glfwVulkanSupported() failed");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, config.Resizable ? GLFW_TRUE : GLFW_FALSE);

    auto* const monitor = config.Fullscreen ? glfwGetPrimaryMonitor() : nullptr;

    m_Window = glfwCreateWindow(static_cast<int>(config.Width), static_cast<int>(config.Height), config.Title.c_str(), monitor, nullptr);
    if(m_Window == nullptr)
    {
        VK_THROW("failed to create window");
    }

    GLFWimage  icon;
    const auto iconPath = Utils::ThisExecutableLocation() + "/Resources/ray-tracing.jpg";
    icon.pixels         = stbi_load(iconPath.c_str(), &icon.width, &icon.height, nullptr, 4);
    if(icon.pixels == nullptr)
    {
        VK_THROW("failed to load icon");
    }

    glfwSetWindowIcon(m_Window, 1, &icon);
    stbi_image_free(icon.pixels);

    if(config.CursorDisabled)
    {
        glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    glfwSetWindowUserPointer(m_Window, this);
    glfwSetKeyCallback(m_Window, GlfwKeyCallback);
    glfwSetCursorPosCallback(m_Window, GlfwCursorPositionCallback);
    glfwSetMouseButtonCallback(m_Window, GlfwMouseButtonCallback);
    glfwSetScrollCallback(m_Window, GlfwScrollCallback);
}

Window::~Window()
{
    if(m_Window != nullptr)
    {
        glfwDestroyWindow(m_Window);
        m_Window = nullptr;
    }

    glfwTerminate();
    glfwSetErrorCallback(nullptr);
}

float Window::ContentScale() const
{
    float xscale;
    float yscale;
    glfwGetWindowContentScale(m_Window, &xscale, &yscale);

    return xscale;
}

VkExtent2D Window::FramebufferSize() const
{
    int width, height;
    glfwGetFramebufferSize(m_Window, &width, &height);
    return VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

VkExtent2D Window::WindowSize() const
{
    int width, height;
    glfwGetWindowSize(m_Window, &width, &height);
    return VkExtent2D{static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
}

const char* Window::GetKeyName(const int key, const int scancode) const
{
    return glfwGetKeyName(key, scancode);
}

std::vector<const char*> Window::GetRequiredInstanceExtensions() const
{
    uint32_t     glfwExtensionCount = 0;
    const char** glfwExtensions     = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
    return {glfwExtensions, glfwExtensions + glfwExtensionCount};
}

double Window::GetTime() const
{
    return glfwGetTime();
}

void Window::Close() const
{
    glfwSetWindowShouldClose(m_Window, 1);
}

bool Window::IsMinimized() const
{
    const auto size = FramebufferSize();
    return size.height == 0 && size.width == 0;
}

void Window::Run() const
{
    glfwSetTime(0.0);

    while(!glfwWindowShouldClose(m_Window))
    {
        glfwPollEvents();

        if(DrawFrame)
        {
            DrawFrame();
        }
    }
}

void Window::WaitForEvents() const
{
    glfwWaitEvents();
}

}    // namespace Engine
