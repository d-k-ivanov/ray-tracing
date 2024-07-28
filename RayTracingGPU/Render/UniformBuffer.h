#pragma once

#include <Utils/Glm.h>

#include <memory>

namespace Engine
{
class Buffer;
class Device;
class DeviceMemory;
}

namespace Render
{
class UniformBufferObject
{
public:
    glm::mat4 ModelView;
    glm::mat4 Projection;
    glm::mat4 ModelViewInverse;
    glm::mat4 ProjectionInverse;
    float     Aperture;
    float     FocusDistance;
    float     HeatmapScale;
    uint32_t  TotalNumberOfSamples;
    uint32_t  NumberOfSamples;
    uint32_t  NumberOfBounces;
    uint32_t  RandomSeed;
    uint32_t  HasSky;         // bool
    uint32_t  ShowHeatmap;    // bool
};

class UniformBuffer
{
public:
    explicit UniformBuffer(const Engine::Device& device);
    UniformBuffer(UniformBuffer&& other) noexcept;
    ~UniformBuffer();

    UniformBuffer(const UniformBuffer&)            = delete;
    UniformBuffer& operator=(const UniformBuffer&) = delete;
    UniformBuffer& operator=(UniformBuffer&&)      = delete;

    const Engine::Buffer& Buffer() const { return *m_Buffer; }

    void SetValue(const UniformBufferObject& ubo) const;

private:
    std::unique_ptr<Engine::Buffer>       m_Buffer;
    std::unique_ptr<Engine::DeviceMemory> m_Memory;
};

}    // namespace Render
