#pragma once

#include <vulkan/vulkan.h>

#include <functional>
#include <memory>

namespace Engine
{
class CommandPool;
class DepthBuffer;
class DescriptorPool;
class FrameBuffer;
class GraphicsPipeline;
class RenderPass;
class SwapChain;
}

namespace UI
{

struct UserSettings;

struct Statistics final
{
    VkExtent2D FramebufferSize;
    float      FrameRate;
    float      RayRate;
    uint32_t   TotalSamples;
};

class UserInterface final
{
public:
    UserInterface(Engine::CommandPool& commandPool, const Engine::SwapChain& swapChain, const Engine::DepthBuffer& depthBuffer, UserSettings& userSettings);

    ~UserInterface();

    UserInterface(const UserInterface&)            = delete;
    UserInterface(UserInterface&&)                 = delete;
    UserInterface& operator=(const UserInterface&) = delete;
    UserInterface& operator=(UserInterface&&)      = delete;

    void Render(VkCommandBuffer commandBuffer, const Engine::FrameBuffer& frameBuffer, const Statistics& statistics) const;

    bool WantsToCaptureKeyboard() const;
    bool WantsToCaptureMouse() const;

    UserSettings& Settings() const { return m_UserSettings; }

    void SetMenubarCallback(const std::function<void()>& menubarCallback) { m_MenubarCallback = menubarCallback; }

private:
    void DrawSettings(const Statistics& statistics) const;
    void DrawViewport(const Engine::GraphicsPipeline& graphicsPipeline) const;
    void DrawStats(const Statistics& statistics) const;

    std::unique_ptr<Engine::DescriptorPool> m_DescriptorPool;
    std::unique_ptr<Engine::RenderPass>     m_RenderPass;
    UserSettings&                           m_UserSettings;
    std::function<void()>                   m_MenubarCallback = nullptr;
};

}    // namespace UI
