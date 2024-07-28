#pragma once

#include <vulkan/vulkan.h>

#include <vector>

namespace Engine
{
class Window;

class Instance final
{
public:
    Instance(const Window& window, const std::vector<const char*>& validationLayers, uint32_t vulkanVersion);
    ~Instance();

    Instance(const Instance&)            = delete;
    Instance(Instance&&)                 = delete;
    Instance& operator=(const Instance&) = delete;
    Instance& operator=(Instance&&)      = delete;

    VkInstance    Handle() const { return m_Instance; }
    const Window& Window() const { return m_Window; }

    const std::vector<VkExtensionProperties>& Extensions() const { return m_Extensions; }
    const std::vector<VkLayerProperties>&     Layers() const { return m_Layers; }
    const std::vector<VkPhysicalDevice>&      PhysicalDevices() const { return m_PhysicalDevices; }
    const std::vector<const char*>&           ValidationLayers() const { return m_ValidationLayers; }

private:
    void GetVulkanExtensions();
    void GetVulkanLayers();
    void GetVulkanPhysicalDevices();

    static void CheckVulkanMinimumVersion(uint32_t minVersion);
    static void CheckVulkanValidationLayerSupport(const std::vector<const char*>& validationLayers);

    VkInstance                     m_Instance{};
    const class Window&            m_Window;
    const std::vector<const char*> m_ValidationLayers;

    std::vector<VkExtensionProperties> m_Extensions;
    std::vector<VkLayerProperties>     m_Layers;
    std::vector<VkPhysicalDevice>      m_PhysicalDevices;
};

}    // namespace Engine
