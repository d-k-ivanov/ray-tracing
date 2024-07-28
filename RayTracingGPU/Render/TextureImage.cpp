#include "TextureImage.h"

#include "Texture.h"

#include <Engine/Buffer.h>
#include <Engine/CommandPool.h>
#include <Engine/Image.h>
#include <Engine/ImageView.h>
#include <Engine/Sampler.h>

#include <cstring>

namespace Render
{

TextureImage::TextureImage(Engine::CommandPool& commandPool, const Texture& texture)
{
    // Create a host staging buffer and copy the image into it.
    const VkDeviceSize imageSize = texture.Width() * texture.Height() * 4;
    const auto&        device    = commandPool.Device();

    auto       stagingBuffer       = std::make_unique<Engine::Buffer>(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
    const auto stagingBufferMemory = stagingBuffer->AllocateMemory(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    const auto data = stagingBufferMemory.Map(0, imageSize);
    std::memcpy(data, texture.Pixels(), imageSize);
    stagingBufferMemory.Unmap();

    // Create the device side image, memory, view and sampler.
    m_Image.reset(new Engine::Image(device, VkExtent2D{static_cast<uint32_t>(texture.Width()), static_cast<uint32_t>(texture.Height())}, VK_FORMAT_R8G8B8A8_UNORM));
    m_ImageMemory.reset(new Engine::DeviceMemory(m_Image->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
    m_ImageView.reset(new Engine::ImageView(device, m_Image->Handle(), m_Image->Format(), VK_IMAGE_ASPECT_COLOR_BIT));
    m_Sampler.reset(new Engine::Sampler(device, Engine::SamplerConfig()));

    // Transfer the data to device side.
    m_Image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    m_Image->CopyFrom(commandPool, *stagingBuffer);
    m_Image->TransitionImageLayout(commandPool, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    // Delete the buffer before the memory
    stagingBuffer.reset();
}

TextureImage::~TextureImage()
{
    m_Sampler.reset();
    m_ImageView.reset();
    m_Image.reset();
    m_ImageMemory.reset();
}

}    // namespace Render
