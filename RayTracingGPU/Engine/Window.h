#pragma once

#include "WindowConfig.h"

#include <vulkan/vulkan.h>

#define NOMINMAX
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#undef APIENTRY

#include <functional>
#include <vector>

namespace Engine
{

class Window final
{
public:
    explicit Window(const WindowConfig& config);
    ~Window();

    Window(const Window&)            = delete;
    Window(Window&&)                 = delete;
    Window& operator=(const Window&) = delete;
    Window& operator=(Window&&)      = delete;

    // Window instance properties.
    const WindowConfig& Config() const { return m_Config; }
    GLFWwindow*         Handle() const { return m_Window; }
    float               ContentScale() const;
    VkExtent2D          FramebufferSize() const;
    VkExtent2D          WindowSize() const;

    // GLFW instance properties (i.e. not bound to a window handler).
    const char*              GetKeyName(int key, int scancode) const;
    std::vector<const char*> GetRequiredInstanceExtensions() const;
    double                   GetTime() const;

    // Callbacks
    std::function<void()>                                            DrawFrame;
    std::function<void(int key, int scancode, int action, int mods)> OnKey;
    std::function<void(double xpos, double ypos)>                    OnCursorPosition;
    std::function<void(int button, int action, int mods)>            OnMouseButton;
    std::function<void(double xoffset, double yoffset)>              OnScroll;

    // Methods
    void Close() const;
    bool IsMinimized() const;
    void Run() const;
    void WaitForEvents() const;

private:
    const WindowConfig m_Config;
    GLFWwindow*        m_Window{};
};

}    // namespace Engine
