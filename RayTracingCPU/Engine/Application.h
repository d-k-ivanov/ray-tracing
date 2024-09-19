#pragma once

#include <UI/Layer.h>

#include <vulkan/vulkan.h>

#include <functional>
#include <memory>
#include <string>
#include <vector>

struct GLFWwindow;

namespace Engine
{

struct ApplicationSpecs
{
    std::string Name   = "Application";
    int         Width  = 1360;
    int         Height = 768;
};

class Application
{
public:
    explicit Application(ApplicationSpecs specification = ApplicationSpecs());
    ~Application();

    static Application& Get();

    void Run();
    void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }

    template <typename T>
    void PushLayer()
    {
        static_assert(std::is_base_of_v<UI::Layer, T>, "Pushed type is not subclass of Layer!");
        m_LayerStack.emplace_back(std::make_shared<T>())->OnAttach();
    }

    void PushLayer(const std::shared_ptr<UI::Layer>& layer);

    void                    Close();
    double                  GetTime() const;
    GLFWwindow*             GetWindowHandle() const;
    static VkInstance       GetInstance();
    static VkPhysicalDevice GetPhysicalDevice();
    static VkDevice         GetDevice();
    static VkCommandBuffer  GetCommandBuffer(bool begin);
    static void             FlushCommandBuffer(VkCommandBuffer commandBuffer);
    static void             SubmitResourceFree(std::function<void()>&& func);

private:
    void Init();
    void Shutdown();

private:
    ApplicationSpecs m_Specification;
    GLFWwindow*      m_WindowHandle = nullptr;
    bool             m_Running      = false;

    double m_TimeStep      = 0.0f;
    double m_FrameTime     = 0.0f;
    double m_LastFrameTime = 0.0f;

    std::vector<std::shared_ptr<UI::Layer>> m_LayerStack;
    std::function<void()>                   m_MenubarCallback;
};

// Implemented by CLIENT
Application* CreateApplication(int argc, char** argv);

}    // namespace Vulkan
