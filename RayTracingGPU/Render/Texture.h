#pragma once

#include <Engine/Sampler.h>

#include <memory>
#include <string>

namespace Render
{
class Texture final
{
public:
    Texture(Texture&&) = default;
    ~Texture()         = default;

    Texture()                          = delete;
    Texture& operator=(const Texture&) = delete;
    Texture& operator=(Texture&&)      = delete;
    Texture(const Texture&)            = delete;

    static Texture LoadTexture(const std::string& filename, const Engine::SamplerConfig& samplerConfig);

    const unsigned char* Pixels() const { return m_Pixels.get(); }

    int Width() const { return m_Width; }
    int Height() const { return m_Height; }

private:
    Texture(int width, int height, int channels, unsigned char* pixels);

    Engine::SamplerConfig                           m_SamplerConfig;
    int                                             m_Width;
    int                                             m_Height;
    int                                             m_Channels;
    std::unique_ptr<unsigned char, void (*)(void*)> m_Pixels;
};

}    // namespace Render
