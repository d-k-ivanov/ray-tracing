#pragma once

#include <Utils/VulkanException.h>

#include <vulkan/vulkan.h>

namespace Engine
{
class DebugUtils final
{
public:
    explicit DebugUtils(VkInstance instance);
    ~DebugUtils() = default;

    DebugUtils(const DebugUtils&)            = delete;
    DebugUtils(DebugUtils&&)                 = delete;
    DebugUtils& operator=(const DebugUtils&) = delete;
    DebugUtils& operator=(DebugUtils&&)      = delete;

    void SetDevice(const VkDevice device)
    {
        m_Device = device;
    }

    void SetObjectName(const VkAccelerationStructureKHR& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_ACCELERATION_STRUCTURE_KHR); }
    void SetObjectName(const VkBuffer& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_BUFFER); }
    void SetObjectName(const VkCommandBuffer& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_COMMAND_BUFFER); }
    void SetObjectName(const VkDescriptorSet& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET); }
    void SetObjectName(const VkDescriptorSetLayout& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT); }
    void SetObjectName(const VkDeviceMemory& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_DEVICE_MEMORY); }
    void SetObjectName(const VkFramebuffer& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_FRAMEBUFFER); }
    void SetObjectName(const VkImage& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_IMAGE); }
    void SetObjectName(const VkImageView& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_IMAGE_VIEW); }
    void SetObjectName(const VkPipeline& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_PIPELINE); }
    void SetObjectName(const VkQueue& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_QUEUE); }
    void SetObjectName(const VkRenderPass& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_RENDER_PASS); }
    void SetObjectName(const VkSemaphore& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_SEMAPHORE); }
    void SetObjectName(const VkShaderModule& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_SHADER_MODULE); }
    void SetObjectName(const VkSwapchainKHR& object, const char* name) const { SetObjectName(object, name, VK_OBJECT_TYPE_SWAPCHAIN_KHR); }

private:
    template <typename T>
    void SetObjectName(const T& object, const char* name, const VkObjectType type) const
    {
#ifndef NDEBUG
        VkDebugUtilsObjectNameInfoEXT info = {};
        info.sType                         = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        info.pNext                         = nullptr;
        info.objectHandle                  = reinterpret_cast<const uint64_t&>(object);
        info.objectType                    = type;
        info.pObjectName                   = name;

        VK_CHECK(m_VkSetDebugUtilsObjectNameEXT(m_Device, &info));
#endif
    }

    const PFN_vkSetDebugUtilsObjectNameEXT m_VkSetDebugUtilsObjectNameEXT;

    VkDevice m_Device{};
};

}    // namespace Engine
