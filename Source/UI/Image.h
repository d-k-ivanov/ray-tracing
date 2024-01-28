#pragma once

#include <vulkan/vulkan.h>

#include <string>

enum class ImageFormat
{
    None = 0,
    RGBA,
    RGBA32F
};

class Image
{
public:
    explicit Image(std::string_view path, bool storeImageData = false);
    Image(uint32_t width, uint32_t height, ImageFormat format, const void* data = nullptr);
    ~Image();

    void SetData(const void* data);

    VkDescriptorSet GetDescriptorSet() const { return m_DescriptorSet; }

    void Resize(uint32_t width, uint32_t height);

    uint32_t GetWidth() const { return m_Width; }
    uint32_t GetHeight() const { return m_Height; }

    const uint8_t* PixelData(uint32_t x, uint32_t y) const;

private:
    void AllocateMemory(uint64_t size);
    void Release();

private:
    uint32_t m_Width  = 0;
    uint32_t m_Height = 0;

    uint8_t*       m_Data      = nullptr;
    VkImage        m_Image     = nullptr;
    VkImageView    m_ImageView = nullptr;
    VkDeviceMemory m_Memory    = nullptr;
    VkSampler      m_Sampler   = nullptr;

    ImageFormat m_Format = ImageFormat::None;

    VkBuffer       m_StagingBuffer       = nullptr;
    VkDeviceMemory m_StagingBufferMemory = nullptr;

    size_t m_AlignedSize = 0;

    VkDescriptorSet m_DescriptorSet = nullptr;

    std::string m_Filepath;
};
