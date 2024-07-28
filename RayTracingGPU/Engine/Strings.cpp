#include "Strings.h"

namespace Engine
{

const char* Strings::DeviceType(const VkPhysicalDeviceType deviceType)
{
    switch(deviceType)
    {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "Other";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "Integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "Discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "Virtual GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "CPU";
        default:
            return "UnknownDeviceType";
    }
}

const char* Strings::VendorId(const uint32_t vendorId)
{
    switch(vendorId)
    {
        case 0x1002:
            return "AMD";
        case 0x1010:
            return "ImgTec";
        case 0x10DE:
            return "NVIDIA";
        case 0x13B5:
            return "ARM";
        case 0x5143:
            return "Qualcomm";
        case 0x8086:
            return "INTEL";
        default:
            return "UnknownVendor";
    }
}
const char* Strings::Result(const VkResult result)
{
    switch(result)
    {
#define STR(r)   \
    case VK_##r: \
        return #r
        STR(SUCCESS);
        STR(NOT_READY);
        STR(TIMEOUT);
        STR(EVENT_SET);
        STR(EVENT_RESET);
        STR(INCOMPLETE);
        STR(ERROR_OUT_OF_HOST_MEMORY);
        STR(ERROR_OUT_OF_DEVICE_MEMORY);
        STR(ERROR_INITIALIZATION_FAILED);
        STR(ERROR_DEVICE_LOST);
        STR(ERROR_MEMORY_MAP_FAILED);
        STR(ERROR_LAYER_NOT_PRESENT);
        STR(ERROR_EXTENSION_NOT_PRESENT);
        STR(ERROR_FEATURE_NOT_PRESENT);
        STR(ERROR_INCOMPATIBLE_DRIVER);
        STR(ERROR_TOO_MANY_OBJECTS);
        STR(ERROR_FORMAT_NOT_SUPPORTED);
        STR(ERROR_FRAGMENTED_POOL);
        STR(ERROR_UNKNOWN);
        STR(ERROR_OUT_OF_POOL_MEMORY);
        STR(ERROR_INVALID_EXTERNAL_HANDLE);
        STR(ERROR_FRAGMENTATION);
        STR(ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS);
        STR(ERROR_SURFACE_LOST_KHR);
        STR(ERROR_NATIVE_WINDOW_IN_USE_KHR);
        STR(SUBOPTIMAL_KHR);
        STR(ERROR_OUT_OF_DATE_KHR);
        STR(ERROR_INCOMPATIBLE_DISPLAY_KHR);
        STR(ERROR_VALIDATION_FAILED_EXT);
        STR(ERROR_INVALID_SHADER_NV);
        STR(ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT);
        STR(ERROR_NOT_PERMITTED_EXT);
        STR(ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT);
        STR(THREAD_IDLE_KHR);
        STR(THREAD_DONE_KHR);
        STR(OPERATION_DEFERRED_KHR);
        STR(OPERATION_NOT_DEFERRED_KHR);
        STR(PIPELINE_COMPILE_REQUIRED_EXT);
        STR(ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR);
        STR(ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR);
        STR(ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR);
        STR(ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR);
        STR(ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR);
        STR(ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR);
        STR(ERROR_INVALID_VIDEO_STD_PARAMETERS_KHR);
        STR(ERROR_COMPRESSION_EXHAUSTED_EXT);
        STR(INCOMPATIBLE_SHADER_BINARY_EXT);
        STR(RESULT_MAX_ENUM);
#undef STR
        default:
            return "UNKNOWN_ERROR";
    }
}
const char* Strings::PresentMode(const VkPresentModeKHR mode)
{
    switch(mode)
    {
        case VK_PRESENT_MODE_IMMEDIATE_KHR:
            return "Immediate (0) - swap images at any time";
        case VK_PRESENT_MODE_MAILBOX_KHR:
            return "Mailbox (1) - update the image when the next V-Sync o ccurs";
        case VK_PRESENT_MODE_FIFO_KHR:
            return "FIFO (2) - update images one by one when the next V-Sync o ccurs";
        case VK_PRESENT_MODE_FIFO_RELAXED_KHR:
            return "Relaxed FIFO (3) - FIFO when V-Sync is off";
        default:
            return "UnknownPresentMode";
    }
}

}    // namespace Engine
