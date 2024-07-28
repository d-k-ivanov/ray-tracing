#pragma once

#include <Engine/Buffer.h>

#include <vulkan/vulkan.h>

#include <memory>
#include <vector>

namespace Engine
{
class Device;
class DeviceMemory;
}

namespace RayTracing
{
class DeviceProcedures;
class RayTracingPipeline;
class RayTracingProperties;

class ShaderBindingTable final
{
public:
    struct Entry
    {
        uint32_t                   GroupIndex;
        std::vector<unsigned char> InlineData;
    };

    ShaderBindingTable(const DeviceProcedures&     deviceProcedures,
                       const RayTracingPipeline&   rayTracingPipeline,
                       const RayTracingProperties& rayTracingProperties,
                       const std::vector<Entry>&   rayGenPrograms,
                       const std::vector<Entry>&   missPrograms,
                       const std::vector<Entry>&   hitGroups);

    ~ShaderBindingTable();

    ShaderBindingTable(const ShaderBindingTable&)            = delete;
    ShaderBindingTable(ShaderBindingTable&&)                 = delete;
    ShaderBindingTable& operator=(const ShaderBindingTable&) = delete;
    ShaderBindingTable& operator=(ShaderBindingTable&&)      = delete;

    const Engine::Buffer& GetBuffer() const { return *m_Buffer; }

    VkDeviceAddress RayGenDeviceAddress() const { return GetBuffer().GetDeviceAddress() + RayGenOffset(); }
    VkDeviceAddress MissDeviceAddress() const { return GetBuffer().GetDeviceAddress() + MissOffset(); }
    VkDeviceAddress HitGroupDeviceAddress() const { return GetBuffer().GetDeviceAddress() + HitGroupOffset(); }

    size_t RayGenOffset() const { return m_RayGenOffset; }
    size_t MissOffset() const { return m_MissOffset; }
    size_t HitGroupOffset() const { return m_HitGroupOffset; }

    size_t RayGenSize() const { return m_RayGenSize; }
    size_t MissSize() const { return m_MissSize; }
    size_t HitGroupSize() const { return m_HitGroupSize; }

    size_t RayGenEntrySize() const { return m_RayGenEntrySize; }
    size_t MissEntrySize() const { return m_MissEntrySize; }
    size_t HitGroupEntrySize() const { return m_HitGroupEntrySize; }

private:
    const size_t m_RayGenEntrySize;
    const size_t m_MissEntrySize;
    const size_t m_HitGroupEntrySize;

    const size_t m_RayGenOffset;
    const size_t m_MissOffset;
    const size_t m_HitGroupOffset;

    const size_t m_RayGenSize;
    const size_t m_MissSize;
    const size_t m_HitGroupSize;

    std::unique_ptr<Engine::Buffer>       m_Buffer;
    std::unique_ptr<Engine::DeviceMemory> m_BufferMemory;
};

}    // namespace RayTracing
