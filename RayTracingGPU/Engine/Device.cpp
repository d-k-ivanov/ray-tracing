#include "Device.h"

#include "Enumerate.h"
#include "Instance.h"
#include "Surface.h"

#include <Utils/VulkanException.h>

#include <algorithm>
#include <set>

namespace Engine
{

namespace
{
std::vector<VkQueueFamilyProperties>::const_iterator FindQueue(
    const std::vector<VkQueueFamilyProperties>& queueFamilies,
    const std::string&                          name,
    const VkQueueFlags                          requiredBits,
    const VkQueueFlags                          excludedBits)
{
    const auto family = std::find_if(queueFamilies.begin(), queueFamilies.end(), [requiredBits, excludedBits](const VkQueueFamilyProperties& queueFamily)
    {
        return queueFamily.queueCount > 0 && queueFamily.queueFlags & requiredBits && !(queueFamily.queueFlags & excludedBits);
    });

    if(family == queueFamilies.end())
    {
        VK_THROW("found no matching ", name, " queue");
    }

    return family;
}
}

Device::Device(
    const VkPhysicalDevice          physicalDevice,
    const class Surface&            surface,
    const std::vector<const char*>& requiredExtensions,
    const VkPhysicalDeviceFeatures& deviceFeatures,
    const void*                     nextDeviceFeatures)
    : m_PhysicalDevice(physicalDevice)
    , m_Surface(surface)
    , m_DebugUtils(surface.Instance().Handle())
{
    CheckRequiredExtensions(physicalDevice, requiredExtensions);

    const auto queueFamilies = GetEnumerateVector(physicalDevice, vkGetPhysicalDeviceQueueFamilyProperties);

    // Find the graphics queue.
    const auto graphicsFamily = FindQueue(queueFamilies, "graphics", VK_QUEUE_GRAPHICS_BIT, 0);
    const auto computeFamily  = FindQueue(queueFamilies, "compute", VK_QUEUE_COMPUTE_BIT, VK_QUEUE_GRAPHICS_BIT);

    // Commented out the dedicated transfer queue, as it's never used (relic from Vulkan tutorial)
    // and causes problems with RADV (see https://github.com/NVIDIA/Q2RTX/issues/147).
    // const auto transferFamily = FindQueue(queueFamilies, "transfer", VK_QUEUE_TRANSFER_BIT, VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

    // Find the presentation queue (usually the same as graphics queue).
    const auto presentFamily = std::find_if(queueFamilies.begin(), queueFamilies.end(), [&](const VkQueueFamilyProperties& queueFamily)
    {
        VkBool32       presentSupport = false;
        const uint32_t i              = static_cast<uint32_t>(&*queueFamilies.cbegin() - &queueFamily);
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface.Handle(), &presentSupport);
        return queueFamily.queueCount > 0 && presentSupport;
    });

    if(presentFamily == queueFamilies.end())
    {
        VK_THROW("found no presentation queue");
    }

    m_GraphicsFamilyIndex = static_cast<uint32_t>(graphicsFamily - queueFamilies.begin());
    m_ComputeFamilyIndex  = static_cast<uint32_t>(computeFamily - queueFamilies.begin());
    m_PresentFamilyIndex  = static_cast<uint32_t>(presentFamily - queueFamilies.begin());
    // m_TransferFamilyIndex = static_cast<uint32_t>(transferFamily - queueFamilies.begin());

    // Queues can be the same
    const std::set uniqueQueueFamilies =
        {
            m_GraphicsFamilyIndex,
            m_ComputeFamilyIndex,
            m_PresentFamilyIndex,
            // m_TransferFamilyIndex
        };

    // Create queues
    constexpr float                      queuePriority = 1.0f;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    for(const uint32_t queueFamilyIndex : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex        = queueFamilyIndex;
        queueCreateInfo.queueCount              = 1;
        queueCreateInfo.pQueuePriorities        = &queuePriority;

        queueCreateInfos.push_back(queueCreateInfo);
    }

    // Create device
    VkDeviceCreateInfo createInfo      = {};
    createInfo.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.pNext                   = nextDeviceFeatures;
    createInfo.queueCreateInfoCount    = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos       = queueCreateInfos.data();
    createInfo.pEnabledFeatures        = &deviceFeatures;
    createInfo.enabledLayerCount       = static_cast<uint32_t>(m_Surface.Instance().ValidationLayers().size());
    createInfo.ppEnabledLayerNames     = m_Surface.Instance().ValidationLayers().data();
    createInfo.enabledExtensionCount   = static_cast<uint32_t>(requiredExtensions.size());
    createInfo.ppEnabledExtensionNames = requiredExtensions.data();

    VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &m_Device));

    m_DebugUtils.SetDevice(m_Device);

    vkGetDeviceQueue(m_Device, m_GraphicsFamilyIndex, 0, &m_GraphicsQueue);
    vkGetDeviceQueue(m_Device, m_ComputeFamilyIndex, 0, &m_ComputeQueue);
    vkGetDeviceQueue(m_Device, m_PresentFamilyIndex, 0, &m_PresentQueue);
    // vkGetDeviceQueue(device_, m_TransferFamilyIndex, 0, &m_TransferQueue);
}

Device::~Device()
{
    if(m_Device != nullptr)
    {
        vkDestroyDevice(m_Device, nullptr);
        m_Device = nullptr;
    }
}

void Device::WaitIdle() const
{
    VK_CHECK(vkDeviceWaitIdle(m_Device));
}

void Device::CheckRequiredExtensions(const VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) const
{
    const auto            availableExtensions = GetEnumerateVector(physicalDevice, static_cast<const char*>(nullptr), vkEnumerateDeviceExtensionProperties);
    std::set<std::string> required(requiredExtensions.begin(), requiredExtensions.end());

    for(const auto& extension : availableExtensions)
    {
        required.erase(extension.extensionName);
    }

    if(!required.empty())
    {
        bool        first = true;
        std::string extensions;

        for(const auto& extension : required)
        {
            if(!first)
            {
                extensions += ", ";
            }

            extensions += extension;
            first = false;
        }

        VK_THROW("missing required extensions: ", extensions);
    }
}

}    // namespace Engine
