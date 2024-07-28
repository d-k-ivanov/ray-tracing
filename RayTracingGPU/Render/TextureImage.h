#pragma once

#include <memory>

namespace Engine
{
class CommandPool;
class DeviceMemory;
class Image;
class ImageView;
class Sampler;
}

namespace Render
{
class Texture;

class TextureImage final
{
public:
    TextureImage(Engine::CommandPool& commandPool, const Texture& texture);
    ~TextureImage();

    TextureImage(const TextureImage&)            = delete;
    TextureImage(TextureImage&&)                 = delete;
    TextureImage& operator=(const TextureImage&) = delete;
    TextureImage& operator=(TextureImage&&)      = delete;

    const Engine::ImageView& ImageView() const { return *m_ImageView; }
    const Engine::Sampler&   Sampler() const { return *m_Sampler; }

private:
    std::unique_ptr<Engine::Image>        m_Image;
    std::unique_ptr<Engine::DeviceMemory> m_ImageMemory;
    std::unique_ptr<Engine::ImageView>    m_ImageView;
    std::unique_ptr<Engine::Sampler>      m_Sampler;
};

}    // namespace Render
