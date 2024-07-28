#pragma once

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace Objects
{
class Model;
}

namespace Render
{
class Texture;
class TextureImage;
}

namespace Engine
{
class Buffer;
class CommandPool;
class DeviceMemory;
class Image;
}

namespace Scenes
{

class Scene final
{
public:
    Scene(Engine::CommandPool& commandPool, std::vector<Objects::Model>&& models, std::vector<Render::Texture>&& textures);
    ~Scene();

    Scene(const Scene&)            = delete;
    Scene(Scene&&)                 = delete;
    Scene& operator=(const Scene&) = delete;
    Scene& operator=(Scene&&)      = delete;

    const std::vector<Objects::Model>& Models() const { return m_Models; }

    bool HasProcedurals() const { return static_cast<bool>(m_ProceduralBuffer); }

    const Engine::Buffer&    VertexBuffer() const { return *m_VertexBuffer; }
    const Engine::Buffer&    IndexBuffer() const { return *m_IndexBuffer; }
    const Engine::Buffer&    MaterialBuffer() const { return *m_MaterialBuffer; }
    const Engine::Buffer&    OffsetsBuffer() const { return *m_OffsetBuffer; }
    const Engine::Buffer&    AabbBuffer() const { return *m_AABBBuffer; }
    const Engine::Buffer&    ProceduralBuffer() const { return *m_ProceduralBuffer; }
    std::vector<VkImageView> TextureImageViews() const { return m_TextureImageViewHandles; }
    std::vector<VkSampler>   TextureSamplers() const { return m_TextureSamplerHandles; }

private:
    const std::vector<Objects::Model>  m_Models;
    const std::vector<Render::Texture> m_Textures;

    std::unique_ptr<Engine::Buffer>       m_VertexBuffer;
    std::unique_ptr<Engine::DeviceMemory> m_VertexBufferMemory;

    std::unique_ptr<Engine::Buffer>       m_IndexBuffer;
    std::unique_ptr<Engine::DeviceMemory> m_IndexBufferMemory;

    std::unique_ptr<Engine::Buffer>       m_MaterialBuffer;
    std::unique_ptr<Engine::DeviceMemory> m_MaterialBufferMemory;

    std::unique_ptr<Engine::Buffer>       m_OffsetBuffer;
    std::unique_ptr<Engine::DeviceMemory> m_OffsetBufferMemory;

    std::unique_ptr<Engine::Buffer>       m_AABBBuffer;
    std::unique_ptr<Engine::DeviceMemory> m_AABBBufferMemory;

    std::unique_ptr<Engine::Buffer>       m_ProceduralBuffer;
    std::unique_ptr<Engine::DeviceMemory> m_ProceduralBufferMemory;

    std::vector<std::unique_ptr<Render::TextureImage>> m_TextureImages;
    std::vector<VkImageView>                           m_TextureImageViewHandles;
    std::vector<VkSampler>                             m_TextureSamplerHandles;
};

}    // namespace Scenes
