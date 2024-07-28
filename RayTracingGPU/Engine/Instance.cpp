#include "Instance.h"

#include "Enumerate.h"
#include "Version.h"
#include "Window.h"

#include <Utils/VulkanException.h>

#include <algorithm>
#include <sstream>

namespace Engine
{

Instance::Instance(const class Window& window, const std::vector<const char*>& validationLayers, const uint32_t vulkanVersion)
    : m_Window(window)
    , m_ValidationLayers(validationLayers)
{
    // Check the minimum version.
    CheckVulkanMinimumVersion(vulkanVersion);

    // Get the list of required extensions.
    auto extensions = window.GetRequiredInstanceExtensions();

    // Check the validation layers and add them to the list of required extensions.
    CheckVulkanValidationLayerSupport(validationLayers);

    if(!validationLayers.empty())
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    // Create the Vulkan instance.
    VkApplicationInfo appInfo  = {};
    appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName   = "RayTracingWeekends";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName        = "No Engine";
    appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion         = vulkanVersion;

    VkInstanceCreateInfo createInfo    = {};
    createInfo.sType                   = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo        = &appInfo;
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
    createInfo.ppEnabledExtensionNames = extensions.data();
    createInfo.enabledLayerCount       = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames     = validationLayers.data();

    VK_CHECK(vkCreateInstance(&createInfo, nullptr, &m_Instance));

    GetVulkanPhysicalDevices();
    GetVulkanLayers();
    GetVulkanExtensions();
}

Instance::~Instance()
{
    if(m_Instance != nullptr)
    {
        vkDestroyInstance(m_Instance, nullptr);
        m_Instance = nullptr;
    }
}

void Instance::GetVulkanExtensions()
{
    GetEnumerateVector(static_cast<const char*>(nullptr), vkEnumerateInstanceExtensionProperties, m_Extensions);
}

void Instance::GetVulkanLayers()
{
    GetEnumerateVector(vkEnumerateInstanceLayerProperties, m_Layers);
}

void Instance::GetVulkanPhysicalDevices()
{
    GetEnumerateVector(m_Instance, vkEnumeratePhysicalDevices, m_PhysicalDevices);

    if(m_PhysicalDevices.empty())
    {
        VK_THROW("found no Vulkan physical devices");
    }
}

void Instance::CheckVulkanMinimumVersion(const uint32_t minVersion)
{
    uint32_t version;
    VK_CHECK(vkEnumerateInstanceVersion(&version));

    if(minVersion > version)
    {
        std::ostringstream out;
        out << "minimum required version not found (required " << Version(minVersion);
        out << ", found " << Version(version) << ")";

        VK_THROW(out.str());
    }
}

void Instance::CheckVulkanValidationLayerSupport(const std::vector<const char*>& validationLayers)
{
    const auto availableLayers = GetEnumerateVector(vkEnumerateInstanceLayerProperties);

    for(const char* layer : validationLayers)
    {
        auto result = std::find_if(availableLayers.begin(), availableLayers.end(), [layer](const VkLayerProperties& layerProperties)
        {
            return strcmp(layer, layerProperties.layerName) == 0;
        });

        if(result == availableLayers.end())
        {
            VK_THROW("could not find the requested validation layer: '", std::string(layer), "'");
        }
    }
}

}    // namespace Engine
