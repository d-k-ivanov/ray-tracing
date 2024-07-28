#include "VulkanException.h"

#include <sstream>

#include "Log.h"

namespace Utils
{

#define VK_RESULT_CASE(code) \
    case code:               \
        out << #code;        \
        break;

std::ostream& operator<<(std::ostream& out, const VkResult result)
{
    switch(result)
    {
        VK_RESULT_CASE(VK_SUCCESS)
        VK_RESULT_CASE(VK_NOT_READY)
        VK_RESULT_CASE(VK_TIMEOUT)
        VK_RESULT_CASE(VK_EVENT_SET)
        VK_RESULT_CASE(VK_EVENT_RESET)
        VK_RESULT_CASE(VK_INCOMPLETE)
        VK_RESULT_CASE(VK_OPERATION_DEFERRED_KHR)
        VK_RESULT_CASE(VK_OPERATION_NOT_DEFERRED_KHR)
        VK_RESULT_CASE(VK_PIPELINE_COMPILE_REQUIRED)
        VK_RESULT_CASE(VK_RESULT_MAX_ENUM)
        VK_RESULT_CASE(VK_SUBOPTIMAL_KHR)
        VK_RESULT_CASE(VK_THREAD_DONE_KHR)
        VK_RESULT_CASE(VK_THREAD_IDLE_KHR)
        VK_RESULT_CASE(VK_ERROR_COMPRESSION_EXHAUSTED_EXT)
        VK_RESULT_CASE(VK_ERROR_DEVICE_LOST)
        VK_RESULT_CASE(VK_ERROR_EXTENSION_NOT_PRESENT)
        VK_RESULT_CASE(VK_ERROR_FEATURE_NOT_PRESENT)
        VK_RESULT_CASE(VK_ERROR_FORMAT_NOT_SUPPORTED)
        VK_RESULT_CASE(VK_ERROR_FRAGMENTATION)
        VK_RESULT_CASE(VK_ERROR_FRAGMENTED_POOL)
        VK_RESULT_CASE(VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT)
        VK_RESULT_CASE(VK_ERROR_IMAGE_USAGE_NOT_SUPPORTED_KHR)
        VK_RESULT_CASE(VK_ERROR_INCOMPATIBLE_DISPLAY_KHR)
        VK_RESULT_CASE(VK_ERROR_INCOMPATIBLE_DRIVER)
        VK_RESULT_CASE(VK_ERROR_INCOMPATIBLE_SHADER_BINARY_EXT)
        VK_RESULT_CASE(VK_ERROR_INITIALIZATION_FAILED)
        VK_RESULT_CASE(VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT)
        VK_RESULT_CASE(VK_ERROR_INVALID_EXTERNAL_HANDLE)
        VK_RESULT_CASE(VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS)
        VK_RESULT_CASE(VK_ERROR_INVALID_SHADER_NV)
        VK_RESULT_CASE(VK_ERROR_LAYER_NOT_PRESENT)
        VK_RESULT_CASE(VK_ERROR_MEMORY_MAP_FAILED)
        VK_RESULT_CASE(VK_ERROR_NATIVE_WINDOW_IN_USE_KHR)
        VK_RESULT_CASE(VK_ERROR_NOT_PERMITTED_KHR)
        VK_RESULT_CASE(VK_ERROR_OUT_OF_DATE_KHR)
        VK_RESULT_CASE(VK_ERROR_OUT_OF_DEVICE_MEMORY)
        VK_RESULT_CASE(VK_ERROR_OUT_OF_HOST_MEMORY)
        VK_RESULT_CASE(VK_ERROR_OUT_OF_POOL_MEMORY)
        VK_RESULT_CASE(VK_ERROR_SURFACE_LOST_KHR)
        VK_RESULT_CASE(VK_ERROR_TOO_MANY_OBJECTS)
        VK_RESULT_CASE(VK_ERROR_UNKNOWN)
        VK_RESULT_CASE(VK_ERROR_VALIDATION_FAILED_EXT)
        VK_RESULT_CASE(VK_ERROR_VIDEO_PICTURE_LAYOUT_NOT_SUPPORTED_KHR)
        VK_RESULT_CASE(VK_ERROR_VIDEO_PROFILE_CODEC_NOT_SUPPORTED_KHR)
        VK_RESULT_CASE(VK_ERROR_VIDEO_PROFILE_FORMAT_NOT_SUPPORTED_KHR)
        VK_RESULT_CASE(VK_ERROR_VIDEO_PROFILE_OPERATION_NOT_SUPPORTED_KHR)
        VK_RESULT_CASE(VK_ERROR_VIDEO_STD_VERSION_NOT_SUPPORTED_KHR)
        default:
            out << "VK_ERROR_UNKNOWN (VkResult " << result << ")";
            break;
    }
    return out;
}
#undef VK_RESULT_CASE

std::ostream& operator<<(std::ostream& out, const CodeLocation& loc)
{
    out << loc.FunctionName << " at " << loc.FileName << ":" << loc.LineNumber;
    return out;
}

VulkanException::VulkanException(const CodeLocation& errorLocation, std::string message)
    : m_Msg(std::move(message))
    , m_CodeLocation(errorLocation)
{
    std::ostringstream oss;
    oss << "Exception raised from " << m_CodeLocation << ": ";
    oss << m_Msg;
    m_ErrorString = oss.str();
    LOG_ERROR(m_ErrorString);
}

VulkanException::VulkanException(const CodeLocation& errorLocation, const char* condition, std::string message)
    : m_Msg(std::move(message))
    , m_CodeLocation(errorLocation)
{
    std::ostringstream oss;
    oss << "Exception raised from " << m_CodeLocation << ": ";
    oss << "(" << condition << ") is false! ";
    oss << m_Msg;
    m_ErrorString = oss.str();
    LOG_ERROR(m_ErrorString);
}

const std::string& VulkanException::msg() const
{
    return m_Msg;
}

const char* VulkanException::what() const noexcept
{
    return m_ErrorString.c_str();
}

}    // namespace Utils
