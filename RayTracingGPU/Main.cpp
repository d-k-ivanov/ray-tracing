
#include "RayTracer.h"

#include <Engine/Enumerate.h>
#include <Engine/Strings.h>
#include <Engine/SwapChain.h>
#include <Engine/Version.h>
#include <Utils/VulkanException.h>

#include <algorithm>
#include <cstdlib>

namespace
{
UI::UserSettings CreateUserSettings()
{
    UI::UserSettings userSettings{};

    // userSettings.VisibleDevices = {1};
    // userSettings.VisibleDevices = {10336};    // NVIDIA GeForce GTX 4070
    // userSettings.VisibleDevices = {42888};    // Intel(R) RaptorLake-S

    userSettings.Benchmark           = false;
    userSettings.BenchmarkNextScenes = false;
    userSettings.BenchmarkMaxTime    = 60;

    userSettings.SceneIndex = 1;

    userSettings.IsRayTraced     = true;
    userSettings.AccumulateRays  = true;
    userSettings.NumberOfSamples = 10;
    userSettings.NumberOfBounces = 50;
    // userSettings.MaxNumberOfSamples = 64 * 1024;
    userSettings.MaxNumberOfSamples = UINT32_MAX - 1;

    userSettings.ShowSettings = true;
    userSettings.ShowViewport = true;
    userSettings.ShowStats    = true;

    userSettings.ShowHeatmap  = false;
    userSettings.HeatmapScale = 1.5f;

    return userSettings;
}

void PrintVulkanInstanceInformation(const Engine::Application& application);
void PrintVulkanLayersInformation(const Engine::Application& application);
void PrintVulkanDevices(const Engine::Application& application, const std::vector<uint32_t>& visible_devices);
void PrintVulkanSwapChainInformation(const Engine::Application& application);
void SetVulkanDevice(Engine::Application& application, const std::vector<uint32_t>& visibleDevices);
}

int main(const int argc, char* argv[], char* env[])
{
    // To turn off messages about unused variables.
    ((void)argc);
    ((void)argv);
    ((void)env);

    Utils::Log::Init();
    LOG_INFO("Starting...");
    try
    {
        const UI::UserSettings     userSettings = CreateUserSettings();
        const Engine::WindowConfig windowConfig{"Vulkan Window", 600, 600, false, false, true};
        // VK_PRESENT_MODE_IMMEDIATE_KHR    = 0
        // VK_PRESENT_MODE_MAILBOX_KHR      = 1
        // VK_PRESENT_MODE_FIFO_KHR         = 2 (default)
        // VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
        constexpr VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

        RayTracer application(userSettings, windowConfig, presentMode);

        LOG_INFO("Vulkan SDK Header Version: {}", VK_HEADER_VERSION);

        PrintVulkanInstanceInformation(application);
        PrintVulkanLayersInformation(application);
        PrintVulkanDevices(application, userSettings.VisibleDevices);

        SetVulkanDevice(application, userSettings.VisibleDevices);

        PrintVulkanSwapChainInformation(application);

        application.Run();

        return EXIT_SUCCESS;
    }

    catch(const std::exception& exception)
    {
        LOG_CRITICAL(exception.what());
    }

    catch(...)
    {
        LOG_CRITICAL("Unhandled Exception");
    }

    return EXIT_FAILURE;
}

namespace
{

void PrintVulkanInstanceInformation(const Engine::Application& application)
{
    LOG_INFO("Vulkan Instance Extensions:");

    for(const auto& [extensionName, specVersion] : application.Extensions())
    {
        LOG_INFO(" - {} ({})", extensionName, Engine::Version(specVersion).ToString());
    }
}

void PrintVulkanLayersInformation(const Engine::Application& application)
{
    LOG_INFO("Vulkan Instance Layers:");

    for(const auto& [layerName, specVersion, implementationVersion, description] : application.Layers())
    {
        LOG_INFO(" - {} ({}) : {}", layerName, Engine::Version(specVersion).ToString(), description);
    }
}

void PrintVulkanDevices(const Engine::Application& application, const std::vector<uint32_t>& visible_devices)
{
    LOG_INFO("Vulkan Devices:");

    for(const auto& device : application.PhysicalDevices())
    {
        VkPhysicalDeviceDriverProperties driverProp{};
        driverProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DRIVER_PROPERTIES;

        VkPhysicalDeviceProperties2 deviceProp{};
        deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        deviceProp.pNext = &driverProp;

        vkGetPhysicalDeviceProperties2(device, &deviceProp);

        VkPhysicalDeviceFeatures features;
        vkGetPhysicalDeviceFeatures(device, &features);

        const auto& prop = deviceProp.properties;

        // Check whether device has been explicitly filtered out.
        if(!visible_devices.empty() && std::find(visible_devices.begin(), visible_devices.end(), prop.deviceID) == visible_devices.end())
        {
            continue;
        }

        const Engine::Version vulkanVersion(prop.apiVersion);
        const Engine::Version driverVersion(prop.driverVersion, prop.vendorID);

        LOG_INFO(" - [{}] {} '{}' ({}: vulkan {}, driver {} {} - {})",
                 prop.deviceID,
                 Engine::Strings::VendorId(prop.vendorID),
                 prop.deviceName,
                 Engine::Strings::DeviceType(prop.deviceType),
                 vulkanVersion.ToString(),
                 driverProp.driverName,
                 driverProp.driverInfo,
                 driverVersion.ToString());
    }
}

void PrintVulkanSwapChainInformation(const Engine::Application& application)
{
    const auto& swapChain = application.SwapChain();

    LOG_INFO("Swap Chain:");
    LOG_INFO(" - Image count: {}", swapChain.Images().size());
    LOG_INFO(" - Present mode: {}", Engine::Strings::PresentMode(swapChain.PresentMode()));
}

void SetVulkanDevice(Engine::Application& application, const std::vector<uint32_t>& visibleDevices)
{
    const auto& physicalDevices = application.PhysicalDevices();
    const auto  result          = std::find_if(physicalDevices.begin(), physicalDevices.end(), [&](const VkPhysicalDevice& device)
              {
        VkPhysicalDeviceProperties2 prop{};
        prop.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
        vkGetPhysicalDeviceProperties2(device, &prop);

        // Check whether device has been explicitly filtered out.
        if(!visibleDevices.empty()
           && std::find(visibleDevices.begin(), visibleDevices.end(), prop.properties.deviceID) == visibleDevices.end())
        {
            return false;
        }

        // We want a device with geometry shader support.
        VkPhysicalDeviceFeatures deviceFeatures;
        vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

        if(!deviceFeatures.geometryShader)
        {
            return false;
        }

        // We want a device that supports the ray tracing extension.
        const auto extensions    = Engine::GetEnumerateVector(device, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties);
        const auto hasRayTracing = std::any_of(extensions.begin(), extensions.end(), [](const VkExtensionProperties& extension)
                  {
            return strcmp(extension.extensionName, VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME) == 0;
        });

        if(!hasRayTracing)
        {
            return false;
        }

        // We want a device with a graphics queue.
        const auto queueFamilies    = Engine::GetEnumerateVector(device, vkGetPhysicalDeviceQueueFamilyProperties);
        const auto hasGraphicsQueue = std::any_of(queueFamilies.begin(), queueFamilies.end(), [](const VkQueueFamilyProperties& queueFamily)
                  {
            return queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT;
        });

        return hasGraphicsQueue;
    });

    if(result == physicalDevices.end())
    {
        VK_THROW("cannot find a suitable device");
    }

    VkPhysicalDeviceProperties2 deviceProp{};
    deviceProp.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
    vkGetPhysicalDeviceProperties2(*result, &deviceProp);

    LOG_INFO("Using {} [{}]", deviceProp.properties.deviceName, deviceProp.properties.deviceID);
    application.SetPhysicalDevice(*result);
}

}    // namespace
