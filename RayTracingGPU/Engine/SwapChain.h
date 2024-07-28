#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace Engine
{
class Device;
class ImageView;
class Window;

class SwapChain final
{
public:
    SwapChain(const Device& device, VkPresentModeKHR presentMode);
    ~SwapChain();

    SwapChain(const SwapChain&)            = delete;
    SwapChain(SwapChain&&)                 = delete;
    SwapChain& operator=(const SwapChain&) = delete;
    SwapChain& operator=(SwapChain&&)      = delete;

    VkSwapchainKHR   Handle() const { return m_SwapChain; }
    VkPhysicalDevice PhysicalDevice() const { return m_PhysicalDevice; }
    const Device&    Device() const { return m_Device; }
    uint32_t         MinImageCount() const { return m_MinImageCount; }

    const std::vector<VkImage>&                    Images() const { return m_Images; }
    const std::vector<std::unique_ptr<ImageView>>& ImageViews() const { return m_ImageViews; }

    const VkExtent2D& Extent() const { return m_Extent; }
    VkFormat          Format() const { return m_Format; }
    VkPresentModeKHR  PresentMode() const { return m_PresentMode; }

private:
    struct SupportDetails
    {
        VkSurfaceCapabilitiesKHR        Capabilities{};
        std::vector<VkSurfaceFormatKHR> Formats;
        std::vector<VkPresentModeKHR>   PresentModes;
    };

    static SupportDetails     QuerySwapChainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface);
    static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats);
    static VkPresentModeKHR   ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& presentModes, VkPresentModeKHR presentMode);
    static VkExtent2D         ChooseSwapExtent(const Window& window, const VkSurfaceCapabilitiesKHR& capabilities);
    static uint32_t           ChooseImageCount(const VkSurfaceCapabilitiesKHR& capabilities);

    VkSwapchainKHR         m_SwapChain{};
    const VkPhysicalDevice m_PhysicalDevice;
    const class Device&    m_Device;

    uint32_t                                m_MinImageCount;
    VkPresentModeKHR                        m_PresentMode;
    VkFormat                                m_Format;
    VkExtent2D                              m_Extent{};
    std::vector<VkImage>                    m_Images;
    std::vector<std::unique_ptr<ImageView>> m_ImageViews;
};

}    // namespace Engine
