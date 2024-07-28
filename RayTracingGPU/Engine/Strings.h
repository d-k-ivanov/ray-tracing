#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{

class Strings final
{
public:
    Strings()                          = delete;
    Strings(const Strings&)            = delete;
    Strings(Strings&&)                 = delete;
    Strings& operator=(const Strings&) = delete;
    Strings& operator=(Strings&&)      = delete;
    ~Strings()                         = delete;

    static const char* DeviceType(VkPhysicalDeviceType deviceType);
    static const char* VendorId(uint32_t vendorId);
    static const char* Result(VkResult result);
    static const char* PresentMode(VkPresentModeKHR mode);
};

}    // namespace Engine
