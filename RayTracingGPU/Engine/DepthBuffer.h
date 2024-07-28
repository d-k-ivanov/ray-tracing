#pragma once

#include <vulkan/vulkan.h>

#include <memory>

namespace Engine
{
class CommandPool;
class Device;
class DeviceMemory;
class Image;
class ImageView;

class DepthBuffer final
{
public:
    DepthBuffer(CommandPool& commandPool, VkExtent2D extent);
    ~DepthBuffer();

    DepthBuffer(const DepthBuffer&)            = delete;
    DepthBuffer(DepthBuffer&&)                 = delete;
    DepthBuffer& operator=(const DepthBuffer&) = delete;
    DepthBuffer& operator=(DepthBuffer&&)      = delete;

    VkFormat         Format() const { return m_Format; }
    const ImageView& ImageView() const { return *m_ImageView; }

    static bool HasStencilComponent(const VkFormat format)
    {
        return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
    }

private:
    const VkFormat                   m_Format;
    std::unique_ptr<Image>           m_Image;
    std::unique_ptr<DeviceMemory>    m_ImageMemory;
    std::unique_ptr<class ImageView> m_ImageView;
};

}    // namespace Engine
