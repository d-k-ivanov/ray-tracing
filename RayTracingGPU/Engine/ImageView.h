#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class Device;

class ImageView final
{
public:
    explicit ImageView(const Device& device, VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
    ~ImageView();

    ImageView(const ImageView&)            = delete;
    ImageView(ImageView&&)                 = delete;
    ImageView& operator=(const ImageView&) = delete;
    ImageView& operator=(ImageView&&)      = delete;

    VkImageView   Handle() const { return m_ImageView; }
    const Device& Device() const { return m_Device; }

private:
    VkImageView         m_ImageView{};
    const class Device& m_Device;
    const VkImage       m_Image;
    const VkFormat      m_Format;
};

}    // namespace Engine
