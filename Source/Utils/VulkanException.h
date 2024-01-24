#pragma once

#include <Utils/Log.h>
#include <Utils/StringTools.h>

#include <vulkan/vulkan.h>

#include <exception>
#include <ostream>
#include <string>

#if defined(_MSC_VER) && _MSC_VER <= 1900
#define __func__ __FUNCTION__
#endif

#define VK_CHECK(function)                                                  \
    do                                                                      \
    {                                                                       \
        const VkResult result = (function);                                 \
        if(VK_SUCCESS != result)                                            \
        {                                                                   \
            LOG_ERROR(ConcatenateStrings(#function, " returned ", result)); \
            throw VulkanException(                                          \
                {__func__, __FILE__, static_cast<uint32_t>(__LINE__)},      \
                ConcatenateStrings(#function, " returned ", result));       \
        }                                                                   \
    } while(false)

#define VK_CHECK_CONDITION(condition, ...)                             \
    do                                                                 \
    {                                                                  \
        if(!(condition))                                               \
        {                                                              \
            LOG_ERROR(ConcatenateStrings(__VA_ARGS__));                \
            throw VulkanException(                                     \
                {__func__, __FILE__, static_cast<uint32_t>(__LINE__)}, \
                #condition,                                            \
                ConcatenateStrings(__VA_ARGS__));                      \
        }                                                              \
    } while(false)

#define VK_THROW(...)                                              \
    do                                                             \
    {                                                              \
        LOG_ERROR(ConcatenateStrings(__VA_ARGS__));                \
        throw VulkanException(                                     \
            {__func__, __FILE__, static_cast<uint32_t>(__LINE__)}, \
            ConcatenateStrings(__VA_ARGS__));                      \
    } while(false)

std::ostream& operator<<(std::ostream& out, VkResult result);

struct CodeLocation
{
    const char* function;
    const char* file;
    uint32_t    line;
};

std::ostream& operator<<(std::ostream& out, const CodeLocation& loc);

class VulkanException final : public std::exception
{
public:
    VulkanException(const CodeLocation& errorLocation, std::string message);
    VulkanException(const CodeLocation& errorLocation, const char* condition, std::string message);

private:
    std::string  m_Msg;
    CodeLocation m_CodeLocation;
    std::string  m_ErrorString;

public:
    const std::string& msg() const
    {
        return m_Msg;
    }

    const char* what() const noexcept override
    {
        return m_ErrorString.c_str();
    }
};
