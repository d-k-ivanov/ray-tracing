#include "ShaderBindingTable.h"

#include "DeviceProcedures.h"
#include "RayTracingPipeline.h"
#include "RayTracingProperties.h"

#include <Engine/Buffer.h>
#include <Engine/Device.h>
#include <Engine/DeviceMemory.h>
#include <Utils/VulkanException.h>

#include <algorithm>
#include <cstring>

namespace RayTracing
{

namespace
{
size_t RoundUp(const size_t size, const size_t powerOf2Alignment)
{
    return (size + powerOf2Alignment - 1) & ~(powerOf2Alignment - 1);
}

size_t GetEntrySize(const RayTracingProperties& rayTracingProperties, const std::vector<ShaderBindingTable::Entry>& entries)
{
    // Find the maximum number of parameters used by a single entry
    size_t maxArgs = 0;

    for(const auto& entry : entries)
    {
        maxArgs = std::max(maxArgs, entry.InlineData.size());
    }

    // A SBT entry is made of a program ID and a set of 4-byte parameters (see shaderRecordEXT).
    // Its size is ShaderGroupHandleSize (plus parameters) and must be aligned to ShaderGroupBaseAlignment.
    return RoundUp(rayTracingProperties.ShaderGroupHandleSize() + maxArgs, rayTracingProperties.ShaderGroupBaseAlignment());
}

size_t CopyShaderData(
    uint8_t* const                                dst,
    const RayTracingProperties&                   rayTracingProperties,
    const std::vector<ShaderBindingTable::Entry>& entries,
    const size_t                                  entrySize,
    const uint8_t* const                          shaderHandleStorage)
{
    const auto handleSize = rayTracingProperties.ShaderGroupHandleSize();

    uint8_t* pDst = dst;

    for(const auto& entry : entries)
    {
        // Copy the shader identifier that was previously obtained with vkGetRayTracingShaderGroupHandlesKHR.
        std::memcpy(pDst, shaderHandleStorage + entry.GroupIndex * handleSize, handleSize);
        std::memcpy(pDst + handleSize, entry.InlineData.data(), entry.InlineData.size());

        pDst += entrySize;
    }

    return entries.size() * entrySize;
}

}

ShaderBindingTable::ShaderBindingTable(
    const DeviceProcedures&     deviceProcedures,
    const RayTracingPipeline&   rayTracingPipeline,
    const RayTracingProperties& rayTracingProperties,
    const std::vector<Entry>&   rayGenPrograms,
    const std::vector<Entry>&   missPrograms,
    const std::vector<Entry>&   hitGroups)
    :

    m_RayGenEntrySize(GetEntrySize(rayTracingProperties, rayGenPrograms))
    , m_MissEntrySize(GetEntrySize(rayTracingProperties, missPrograms))
    , m_HitGroupEntrySize(GetEntrySize(rayTracingProperties, hitGroups))
    ,

    m_RayGenOffset(0)
    , m_MissOffset(rayGenPrograms.size() * m_RayGenEntrySize)
    , m_HitGroupOffset(m_MissOffset + missPrograms.size() * m_MissEntrySize)
    ,

    m_RayGenSize(rayGenPrograms.size() * m_RayGenEntrySize)
    , m_MissSize(missPrograms.size() * m_MissEntrySize)
    , m_HitGroupSize(hitGroups.size() * m_HitGroupEntrySize)
{
    // Compute the size of the table.
    const size_t sbtSize =
        rayGenPrograms.size() * m_RayGenEntrySize + missPrograms.size() * m_MissEntrySize + hitGroups.size() * m_HitGroupEntrySize;

    // Allocate buffer & memory.
    const auto& device = rayTracingProperties.Device();

    m_Buffer.reset(new Engine::Buffer(device, sbtSize, VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR));
    m_BufferMemory.reset(new Engine::DeviceMemory(m_Buffer->AllocateMemory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT)));

    // Generate the table.
    const uint32_t       handleSize = rayTracingProperties.ShaderGroupHandleSize();
    const size_t         groupCount = rayGenPrograms.size() + missPrograms.size() + hitGroups.size();
    std::vector<uint8_t> shaderHandleStorage(groupCount * handleSize);

    VK_CHECK(deviceProcedures.vkGetRayTracingShaderGroupHandlesKHR(device.Handle(),
                                                                   rayTracingPipeline.Handle(),
                                                                   0, static_cast<uint32_t>(groupCount),
                                                                   shaderHandleStorage.size(),
                                                                   shaderHandleStorage.data()));

    // Copy the shader identifiers followed by their resource pointers or root constants:
    // first the ray generation, then the miss shaders, and finally the set of hit groups.
    auto* pData = static_cast<uint8_t*>(m_BufferMemory->Map(0, sbtSize));

    pData += CopyShaderData(pData, rayTracingProperties, rayGenPrograms, m_RayGenEntrySize, shaderHandleStorage.data());
    pData += CopyShaderData(pData, rayTracingProperties, missPrograms, m_MissEntrySize, shaderHandleStorage.data());
    CopyShaderData(pData, rayTracingProperties, hitGroups, m_HitGroupEntrySize, shaderHandleStorage.data());

    m_BufferMemory->Unmap();
}

ShaderBindingTable::~ShaderBindingTable()
{
    m_Buffer.reset();
    m_BufferMemory.reset();
}

}    // namespace RayTracing
