#include "Scene.h"

#include <Engine/BufferUtil.h>
#include <Engine/ImageView.h>
#include <Engine/Sampler.h>
#include <Engine/SingleTimeCommands.h>
#include <Objects/Model.h>
#include <Objects/Sphere.h>
#include <Render/Texture.h>
#include <Render/TextureImage.h>

namespace Scenes
{

Scene::Scene(Engine::CommandPool& commandPool, std::vector<Objects::Model>&& models, std::vector<Render::Texture>&& textures)
    : m_Models(std::move(models))
    , m_Textures(std::move(textures))
{
    // Concatenate all the models
    std::vector<Objects::Vertex>    vertices;
    std::vector<uint32_t>           indices;
    std::vector<Render::Material>   materials;
    std::vector<glm::vec4>          procedurals;
    std::vector<VkAabbPositionsKHR> aabbs;
    std::vector<glm::uvec2>         offsets;

    for(const auto& model : m_Models)
    {
        // Remember the index, vertex offsets.
        const auto indexOffset    = static_cast<uint32_t>(indices.size());
        const auto vertexOffset   = static_cast<uint32_t>(vertices.size());
        const auto materialOffset = static_cast<uint32_t>(materials.size());

        offsets.emplace_back(indexOffset, vertexOffset);

        // Copy model data one after the other.
        vertices.insert(vertices.end(), model.Vertices().begin(), model.Vertices().end());
        indices.insert(indices.end(), model.Indices().begin(), model.Indices().end());
        materials.insert(materials.end(), model.Materials().begin(), model.Materials().end());

        // Adjust the material id.
        for(size_t i = vertexOffset; i != vertices.size(); ++i)
        {
            vertices[i].MaterialIndex += materialOffset;
        }

        // Add optional procedurals.
        const auto* const sphere = dynamic_cast<const Objects::Sphere*>(model.Procedural());
        if(sphere != nullptr)
        {
            const auto aabb = sphere->BoundingBox();
            aabbs.push_back({aabb.first.x, aabb.first.y, aabb.first.z, aabb.second.x, aabb.second.y, aabb.second.z});
            procedurals.emplace_back(sphere->Center, sphere->Radius);
        }
        else
        {
            aabbs.emplace_back();
            procedurals.emplace_back();
        }
    }

    constexpr auto flags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;

    Engine::BufferUtil::CreateDeviceBuffer(commandPool, "Vertices", VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | flags, vertices, m_VertexBuffer, m_VertexBufferMemory);
    Engine::BufferUtil::CreateDeviceBuffer(commandPool, "Indices", VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | flags, indices, m_IndexBuffer, m_IndexBufferMemory);
    Engine::BufferUtil::CreateDeviceBuffer(commandPool, "Materials", flags, materials, m_MaterialBuffer, m_MaterialBufferMemory);
    Engine::BufferUtil::CreateDeviceBuffer(commandPool, "Offsets", flags, offsets, m_OffsetBuffer, m_OffsetBufferMemory);

    Engine::BufferUtil::CreateDeviceBuffer(commandPool, "AABBs", VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | flags, aabbs, m_AABBBuffer, m_AABBBufferMemory);
    Engine::BufferUtil::CreateDeviceBuffer(commandPool, "Procedurals", flags, procedurals, m_ProceduralBuffer, m_ProceduralBufferMemory);

    // Upload all textures
    m_TextureImages.reserve(m_Textures.size());
    m_TextureImageViewHandles.resize(m_Textures.size());
    m_TextureSamplerHandles.resize(m_Textures.size());

    for(size_t i = 0; i != m_Textures.size(); ++i)
    {
        m_TextureImages.emplace_back(new Render::TextureImage(commandPool, m_Textures[i]));
        m_TextureImageViewHandles[i] = m_TextureImages[i]->ImageView().Handle();
        m_TextureSamplerHandles[i]   = m_TextureImages[i]->Sampler().Handle();
    }
}

Scene::~Scene()
{
    m_TextureSamplerHandles.clear();
    m_TextureImageViewHandles.clear();
    m_TextureImages.clear();
    m_ProceduralBuffer.reset();
    m_ProceduralBufferMemory.reset();    // release memory after bound buffer has been destroyed
    m_AABBBuffer.reset();
    m_AABBBufferMemory.reset();    // release memory after bound buffer has been destroyed
    m_OffsetBuffer.reset();
    m_OffsetBufferMemory.reset();    // release memory after bound buffer has been destroyed
    m_MaterialBuffer.reset();
    m_MaterialBufferMemory.reset();    // release memory after bound buffer has been destroyed
    m_IndexBuffer.reset();
    m_IndexBufferMemory.reset();    // release memory after bound buffer has been destroyed
    m_VertexBuffer.reset();
    m_VertexBufferMemory.reset();    // release memory after bound buffer has been destroyed
}

}    // namespace Scenes
