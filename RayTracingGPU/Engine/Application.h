#pragma once

#include "FrameBuffer.h"
#include "WindowConfig.h"

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace Scenes
{
class Scene;
}

namespace Render
{
class UniformBufferObject;
class UniformBuffer;
}

namespace Engine
{
class Application
{
public:
    virtual ~Application();

    Application(const Application&)            = delete;
    Application(Application&&)                 = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&)      = delete;

    const std::vector<VkExtensionProperties>& Extensions() const;
    const std::vector<VkLayerProperties>&     Layers() const;
    const std::vector<VkPhysicalDevice>&      PhysicalDevices() const;

    const class SwapChain& SwapChain() const { return *m_SwapChain; }
    class Window&          Window() { return *m_Window; }
    const class Window&    Window() const { return *m_Window; }

    bool HasSwapChain() const { return m_SwapChain.operator bool(); }

    void SetPhysicalDevice(VkPhysicalDevice physicalDevice);
    void Run();

protected:
    Application(const WindowConfig& windowConfig, VkPresentModeKHR presentMode, bool enableValidationLayers);

    const class Device&                       Device() const { return *m_Device; }
    class CommandPool&                        CommandPool() const { return *m_CommandPool; }
    const class DepthBuffer&                  DepthBuffer() const { return *m_DepthBuffer; }
    const std::vector<Render::UniformBuffer>& UniformBuffers() const { return m_UniformBuffers; }
    const class GraphicsPipeline&             GraphicsPipeline() const { return *m_GraphicsPipeline; }
    const FrameBuffer&                        SwapChainFrameBuffer(const size_t i) const { return m_SwapChainFramebuffers[i]; }

    virtual const Scenes::Scene&        GetScene() const                                = 0;
    virtual Render::UniformBufferObject GetUniformBufferObject(VkExtent2D extent) const = 0;

    virtual void SetPhysicalDevice(
        VkPhysicalDevice          physicalDevice,
        std::vector<const char*>& requiredExtensions,
        VkPhysicalDeviceFeatures& deviceFeatures,
        void*                     nextDeviceFeatures);

    virtual void OnDeviceSet();
    virtual void CreateSwapChain();
    virtual void DeleteSwapChain();
    virtual void DrawFrame();
    virtual void Render(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    virtual void OnKey(int key, int scancode, int action, int mods) { }
    virtual void OnCursorPosition(double xpos, double ypos) { }
    virtual void OnMouseButton(int button, int action, int mods) { }
    virtual void OnScroll(double xoffset, double yoffset) { }

    bool m_IsWireFrame{};

private:
    void UpdateUniformBuffer(uint32_t imageIndex) const;
    void RecreateSwapChain();

    const VkPresentModeKHR m_PresentMode;

    std::unique_ptr<class Window>              m_Window;
    std::unique_ptr<class Instance>            m_Instance;
    std::unique_ptr<class DebugUtilsMessenger> m_DebugUtilsMessenger;
    std::unique_ptr<class Surface>             m_Surface;
    std::unique_ptr<class Device>              m_Device;
    std::unique_ptr<class SwapChain>           m_SwapChain;
    std::vector<Render::UniformBuffer>         m_UniformBuffers;
    std::unique_ptr<class DepthBuffer>         m_DepthBuffer;
    std::unique_ptr<class GraphicsPipeline>    m_GraphicsPipeline;
    std::vector<FrameBuffer>                   m_SwapChainFramebuffers;
    std::unique_ptr<class CommandPool>         m_CommandPool;
    std::unique_ptr<class CommandBuffers>      m_CommandBuffers;
    std::vector<class Semaphore>               m_ImageAvailableSemaphores;
    std::vector<Semaphore>                     m_RenderFinishedSemaphores;
    std::vector<class Fence>                   m_InFlightFences;

    size_t m_CurrentFrame{};
};

}    // namespace Engine
