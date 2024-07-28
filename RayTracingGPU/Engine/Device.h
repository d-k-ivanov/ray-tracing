#pragma once

#include "DebugUtils.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Engine
{
class Surface;

class Device final
{
public:
    Device(VkPhysicalDevice                physicalDevice,
           const Surface&                  surface,
           const std::vector<const char*>& requiredExtensionsconst,
           const VkPhysicalDeviceFeatures& deviceFeatures,
           const void*                     nextDeviceFeatures);

    ~Device();

    Device(const Device&)            = delete;
    Device(Device&&)                 = delete;
    Device& operator=(const Device&) = delete;
    Device& operator=(Device&&)      = delete;

    VkDevice         Handle() const { return m_Device; }
    VkPhysicalDevice PhysicalDevice() const { return m_PhysicalDevice; }
    const Surface&   Surface() const { return m_Surface; }

    const DebugUtils& DebugUtils() const { return m_DebugUtils; }

    uint32_t GraphicsFamilyIndex() const { return m_GraphicsFamilyIndex; }
    uint32_t ComputeFamilyIndex() const { return m_ComputeFamilyIndex; }
    uint32_t PresentFamilyIndex() const { return m_PresentFamilyIndex; }
    // uint32_t TransferFamilyIndex() const { return m_TransferFamilyIndex; }

    VkQueue GraphicsQueue() const { return m_GraphicsQueue; }
    VkQueue ComputeQueue() const { return m_ComputeQueue; }
    VkQueue PresentQueue() const { return m_PresentQueue; }
    // VkQueue TransferQueue() const { return m_TransferQueue; }

    void WaitIdle() const;

private:
    void CheckRequiredExtensions(VkPhysicalDevice physicalDevice, const std::vector<const char*>& requiredExtensions) const;

    VkDevice               m_Device{};
    const VkPhysicalDevice m_PhysicalDevice;
    const class Surface&   m_Surface;

    class DebugUtils m_DebugUtils;

    uint32_t m_GraphicsFamilyIndex{};
    uint32_t m_ComputeFamilyIndex{};
    uint32_t m_PresentFamilyIndex{};
    // uint32_t m_TransferFamilyIndex{};

    VkQueue m_GraphicsQueue{};
    VkQueue m_ComputeQueue{};
    VkQueue m_PresentQueue{};
    // VkQueue m_TransferQueue{};
};

}    // namespace Engine
