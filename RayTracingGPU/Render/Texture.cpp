#include "Texture.h"

#include <Utils/StbImage.h>
#include <Utils/VulkanException.h>

#include <chrono>
#include <filesystem>

namespace Render
{

Texture Texture::LoadTexture(const std::string& filename, const Engine::SamplerConfig& samplerConfig)
{
    LOG_INFO("Loading texture '{}'", filename);

    const auto timer = std::chrono::high_resolution_clock::now();

    // Load the texture in normal host memory.
    int        width, height, channels;
    const auto pixels = stbi_load(filename.c_str(), &width, &height, &channels, STBI_rgb_alpha);

    if(!pixels)
    {
        VK_THROW("failed to load texture image '", filename, "'");
    }

    const auto elapsed = std::chrono::duration<float>(std::chrono::high_resolution_clock::now() - timer).count();

    auto textureName = std::filesystem::path(filename).filename().string();
    LOG_INFO("Texture: '{}' ({} x {} x {})", textureName, width, height, channels);
    LOG_INFO("Texture: '{}' loaded in {:.2f} seconds", textureName, elapsed);

    return {width, height, channels, pixels};
}

Texture::Texture(const int width, const int height, const int channels, unsigned char* const pixels)
    : m_Width(width)
    , m_Height(height)
    , m_Channels(channels)
    , m_Pixels(pixels, stbi_image_free)
{
}

}    // namespace Render
