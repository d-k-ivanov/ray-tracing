#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class Instance;

class DebugUtilsMessenger final
{
public:
    DebugUtilsMessenger(const Instance& instance, VkDebugUtilsMessageSeverityFlagBitsEXT threshold);
    ~DebugUtilsMessenger();

    DebugUtilsMessenger(const DebugUtilsMessenger&)            = delete;
    DebugUtilsMessenger(DebugUtilsMessenger&&)                 = delete;
    DebugUtilsMessenger& operator=(const DebugUtilsMessenger&) = delete;
    DebugUtilsMessenger& operator=(DebugUtilsMessenger&&)      = delete;

    VkDebugUtilsMessengerEXT               Handle() const { return m_Messenger; }
    VkDebugUtilsMessageSeverityFlagBitsEXT Threshold() const { return m_Threshold; }

private:
    VkDebugUtilsMessengerEXT                     m_Messenger{};
    const Instance&                              m_Instance;
    const VkDebugUtilsMessageSeverityFlagBitsEXT m_Threshold;
};

}    // namespace Engine
