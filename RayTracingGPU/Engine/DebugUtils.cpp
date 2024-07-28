#include "DebugUtils.h"

#include <Utils/VulkanException.h>

namespace Engine
{

DebugUtils::DebugUtils(const VkInstance instance)
    : m_VkSetDebugUtilsObjectNameEXT(reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(vkGetInstanceProcAddr(instance, "vkSetDebugUtilsObjectNameEXT")))
{
#ifndef NDEBUG
    if(m_VkSetDebugUtilsObjectNameEXT == nullptr)
    {
        VK_THROW("failed to get address of 'vkSetDebugUtilsObjectNameEXT'");
    }
#endif
}

}    // namespace Engine
