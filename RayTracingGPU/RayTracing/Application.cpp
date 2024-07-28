#include "Application.h"

#include "BottomLevelAccelerationStructure.h"
#include "DeviceProcedures.h"
#include "RayTracingPipeline.h"
#include "ShaderBindingTable.h"
#include "TopLevelAccelerationStructure.h"
#include "Utils/Log.h"

#include <Engine/Buffer.h>
#include <Engine/BufferUtil.h>
#include <Engine/Image.h>
#include <Engine/ImageMemoryBarrier.h>
#include <Engine/ImageView.h>
#include <Engine/PipelineLayout.h>
#include <Engine/SingleTimeCommands.h>
#include <Engine/SwapChain.h>
#include <Objects/Model.h>
#include <Scenes/Scene.h>
#include <Utils/Glm.h>

#include <chrono>
#include <numeric>

namespace RayTracing
{

namespace
{
template <class TAccelerationStructure>
VkAccelerationStructureBuildSizesInfoKHR GetTotalRequirements(const std::vector<TAccelerationStructure>& accelerationStructures)
{
    VkAccelerationStructureBuildSizesInfoKHR total{};

    for(const auto& accelerationStructure : accelerationStructures)
    {
        total.accelerationStructureSize += accelerationStructure.BuildSizes().accelerationStructureSize;
        total.buildScratchSize += accelerationStructure.BuildSizes().buildScratchSize;
        total.updateScratchSize += accelerationStructure.BuildSizes().updateScratchSize;
    }

    return total;
}
}

Application::Application(const Engine::WindowConfig& windowConfig, const VkPresentModeKHR presentMode, const bool enableValidationLayers)
    : Engine::Application(windowConfig, presentMode, enableValidationLayers)
{
}

Application::~Application()
{
    Application::DeleteSwapChain();
    DeleteAccelerationStructures();

    m_RayTracingProperties.reset();
    m_DeviceProcedures.reset();
}

void Application::SetPhysicalDevice(const VkPhysicalDevice    physicalDevice,
                                    std::vector<const char*>& requiredExtensions,
                                    VkPhysicalDeviceFeatures& deviceFeatures,
                                    void*                     nextDeviceFeatures)
{
    // Required extensions.
    requiredExtensions.insert(requiredExtensions.end(),
                              {VK_KHR_DEFERRED_HOST_OPERATIONS_EXTENSION_NAME,
                               VK_KHR_ACCELERATION_STRUCTURE_EXTENSION_NAME,
                               VK_KHR_RAY_TRACING_PIPELINE_EXTENSION_NAME});

    // Required device features.
    VkPhysicalDeviceBufferDeviceAddressFeatures bufferDeviceAddressFeatures = {};

    bufferDeviceAddressFeatures.sType               = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_BUFFER_DEVICE_ADDRESS_FEATURES;
    bufferDeviceAddressFeatures.pNext               = nextDeviceFeatures;
    bufferDeviceAddressFeatures.bufferDeviceAddress = true;

    VkPhysicalDeviceDescriptorIndexingFeatures indexingFeatures = {};

    indexingFeatures.sType                                     = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES;
    indexingFeatures.pNext                                     = &bufferDeviceAddressFeatures;
    indexingFeatures.runtimeDescriptorArray                    = true;
    indexingFeatures.shaderSampledImageArrayNonUniformIndexing = true;

    VkPhysicalDeviceAccelerationStructureFeaturesKHR accelerationStructureFeatures = {};

    accelerationStructureFeatures.sType                 = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ACCELERATION_STRUCTURE_FEATURES_KHR;
    accelerationStructureFeatures.pNext                 = &indexingFeatures;
    accelerationStructureFeatures.accelerationStructure = true;

    VkPhysicalDeviceRayTracingPipelineFeaturesKHR rayTracingFeatures = {};

    rayTracingFeatures.sType              = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_RAY_TRACING_PIPELINE_FEATURES_KHR;
    rayTracingFeatures.pNext              = &accelerationStructureFeatures;
    rayTracingFeatures.rayTracingPipeline = true;

    Engine::Application::SetPhysicalDevice(physicalDevice, requiredExtensions, deviceFeatures, &rayTracingFeatures);
}

void Application::OnDeviceSet()
{
    Engine::Application::OnDeviceSet();

    m_DeviceProcedures.reset(new DeviceProcedures(Device()));
    m_RayTracingProperties.reset(new RayTracingProperties(Device()));
}

void Application::CreateAccelerationStructures()
{
    const auto timer = std::chrono::high_resolution_clock::now();

    Engine::SingleTimeCommands::Submit(CommandPool(), [this](const VkCommandBuffer commandBuffer)
    {
        CreateBottomLevelStructures(commandBuffer);
        CreateTopLevelStructures(commandBuffer);
    });

    m_TopScratchBuffer.reset();
    m_TopScratchBufferMemory.reset();
    m_BottomScratchBuffer.reset();
    m_BottomScratchBufferMemory.reset();

    const auto elapsed = std::chrono::duration<float, std::chrono::seconds::period>(std::chrono::high_resolution_clock::now() - timer).count();
    LOG_INFO("Acceleration Structures built in {} seconds", elapsed);
}

void Application::DeleteAccelerationStructures()
{
    m_TopAs.clear();
    m_InstancesBuffer.reset();
    m_InstancesBufferMemory.reset();
    m_TopScratchBuffer.reset();
    m_TopScratchBufferMemory.reset();
    m_TopBuffer.reset();
    m_TopBufferMemory.reset();

    m_BottomAs.clear();
    m_BottomScratchBuffer.reset();
    m_BottomScratchBufferMemory.reset();
    m_BottomBuffer.reset();
    m_BottomBufferMemory.reset();
}

void Application::CreateSwapChain()
{
    Engine::Application::CreateSwapChain();

    CreateOutputImage();

    m_RayTracingPipeline.reset(new RayTracingPipeline(*m_DeviceProcedures, SwapChain(), m_TopAs[0], *m_AccumulationImageView, *m_OutputImageView, UniformBuffers(), GetScene()));

    const std::vector<ShaderBindingTable::Entry> rayGenPrograms = {{m_RayTracingPipeline->RayGenShaderIndex(), {}}};
    const std::vector<ShaderBindingTable::Entry> missPrograms   = {{m_RayTracingPipeline->MissShaderIndex(), {}}};
    const std::vector<ShaderBindingTable::Entry> hitGroups      = {{m_RayTracingPipeline->TriangleHitGroupIndex(), {}}, {m_RayTracingPipeline->ProceduralHitGroupIndex(), {}}};

    m_ShaderBindingTable.reset(new ShaderBindingTable(*m_DeviceProcedures, *m_RayTracingPipeline, *m_RayTracingProperties, rayGenPrograms, missPrograms, hitGroups));
}

void Application::DeleteSwapChain()
{
    m_ShaderBindingTable.reset();
    m_RayTracingPipeline.reset();
    m_OutputImageView.reset();
    m_OutputImage.reset();
    m_OutputImageMemory.reset();
    m_AccumulationImageView.reset();
    m_AccumulationImage.reset();
    m_AccumulationImageMemory.reset();

    Engine::Application::DeleteSwapChain();
}

void Application::Render(const VkCommandBuffer commandBuffer, const uint32_t imageIndex)
{
    const auto extent = SwapChain().Extent();

    const VkDescriptorSet descriptorSets[] = {m_RayTracingPipeline->DescriptorSet(imageIndex)};

    VkImageSubresourceRange subresourceRange;
    subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
    subresourceRange.baseMipLevel   = 0;
    subresourceRange.levelCount     = 1;
    subresourceRange.baseArrayLayer = 0;
    subresourceRange.layerCount     = 1;

    // Acquire destination images for rendering.
    Engine::ImageMemoryBarrier::Insert(commandBuffer,
                                       m_AccumulationImage->Handle(),
                                       subresourceRange,
                                       0,
                                       VK_ACCESS_SHADER_WRITE_BIT,
                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_GENERAL);

    Engine::ImageMemoryBarrier::Insert(commandBuffer,
                                       m_OutputImage->Handle(),
                                       subresourceRange,
                                       0,
                                       VK_ACCESS_SHADER_WRITE_BIT,
                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_GENERAL);

    // Bind ray tracing pipeline.
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_RayTracingPipeline->Handle());
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_RayTracingPipeline->PipelineLayout().Handle(), 0, 1, descriptorSets, 0, nullptr);

    // Describe the shader binding table.
    VkStridedDeviceAddressRegionKHR raygenShaderBindingTable;
    raygenShaderBindingTable.deviceAddress = m_ShaderBindingTable->RayGenDeviceAddress();
    raygenShaderBindingTable.stride        = m_ShaderBindingTable->RayGenEntrySize();
    raygenShaderBindingTable.size          = m_ShaderBindingTable->RayGenSize();

    VkStridedDeviceAddressRegionKHR missShaderBindingTable;
    missShaderBindingTable.deviceAddress = m_ShaderBindingTable->MissDeviceAddress();
    missShaderBindingTable.stride        = m_ShaderBindingTable->MissEntrySize();
    missShaderBindingTable.size          = m_ShaderBindingTable->MissSize();

    VkStridedDeviceAddressRegionKHR hitShaderBindingTable;
    hitShaderBindingTable.deviceAddress = m_ShaderBindingTable->HitGroupDeviceAddress();
    hitShaderBindingTable.stride        = m_ShaderBindingTable->HitGroupEntrySize();
    hitShaderBindingTable.size          = m_ShaderBindingTable->HitGroupSize();

    constexpr VkStridedDeviceAddressRegionKHR callableShaderBindingTable = {};

    // Execute ray tracing shaders.
    m_DeviceProcedures->vkCmdTraceRaysKHR(commandBuffer,
                                          &raygenShaderBindingTable,
                                          &missShaderBindingTable,
                                          &hitShaderBindingTable,
                                          &callableShaderBindingTable,
                                          extent.width,
                                          extent.height, 1);

    // Acquire output image and swap-chain image for copying.
    Engine::ImageMemoryBarrier::Insert(commandBuffer,
                                       m_OutputImage->Handle(),
                                       subresourceRange,
                                       VK_ACCESS_SHADER_WRITE_BIT,
                                       VK_ACCESS_TRANSFER_READ_BIT,
                                       VK_IMAGE_LAYOUT_GENERAL,
                                       VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

    Engine::ImageMemoryBarrier::Insert(commandBuffer,
                                       SwapChain().Images()[imageIndex],
                                       subresourceRange,
                                       0,
                                       VK_ACCESS_TRANSFER_WRITE_BIT,
                                       VK_IMAGE_LAYOUT_UNDEFINED,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

    // Copy output image into swap-chain image.
    VkImageCopy copyRegion;
    copyRegion.srcSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    copyRegion.srcOffset      = {0, 0, 0};
    copyRegion.dstSubresource = {VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1};
    copyRegion.dstOffset      = {0, 0, 0};
    copyRegion.extent         = {extent.width, extent.height, 1};

    vkCmdCopyImage(commandBuffer,
                   m_OutputImage->Handle(),
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   SwapChain().Images()[imageIndex],
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1, &copyRegion);

    Engine::ImageMemoryBarrier::Insert(commandBuffer,
                                       SwapChain().Images()[imageIndex],
                                       subresourceRange,
                                       VK_ACCESS_TRANSFER_WRITE_BIT,
                                       0,
                                       VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                                       VK_IMAGE_LAYOUT_PRESENT_SRC_KHR);
}

void Application::CreateBottomLevelStructures(const VkCommandBuffer commandBuffer)
{
    const auto& scene      = GetScene();
    const auto& debugUtils = Device().DebugUtils();

    // Bottom level acceleration structure
    // Triangles via vertex buffers. Procedurals via AABBs.
    uint32_t vertexOffset = 0;
    uint32_t indexOffset  = 0;
    uint32_t aabbOffset   = 0;

    for(const auto& model : scene.Models())
    {
        const auto          vertexCount = model.NumberOfVertices();
        const auto          indexCount  = model.NumberOfIndices();
        BottomLevelGeometry geometries;

        model.Procedural()
            ? geometries.AddGeometryAabb(scene, aabbOffset, 1, true)
            : geometries.AddGeometryTriangles(scene, vertexOffset, vertexCount, indexOffset, indexCount, true);

        m_BottomAs.emplace_back(*m_DeviceProcedures, *m_RayTracingProperties, geometries);

        vertexOffset += vertexCount * sizeof(Objects::Vertex);
        indexOffset += indexCount * sizeof(uint32_t);
        aabbOffset += sizeof(VkAabbPositionsKHR);
    }

    // Allocate the structures memory.
    const auto total = GetTotalRequirements(m_BottomAs);

    m_BottomBuffer.reset(new Engine::Buffer(Device(), total.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT));
    m_BottomBufferMemory.reset(new Engine::DeviceMemory(m_BottomBuffer->AllocateMemory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
    m_BottomScratchBuffer.reset(new Engine::Buffer(Device(), total.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
    m_BottomScratchBufferMemory.reset(new Engine::DeviceMemory(m_BottomScratchBuffer->AllocateMemory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

    debugUtils.SetObjectName(m_BottomBuffer->Handle(), "BLAS Buffer");
    debugUtils.SetObjectName(m_BottomBufferMemory->Handle(), "BLAS Memory");
    debugUtils.SetObjectName(m_BottomScratchBuffer->Handle(), "BLAS Scratch Buffer");
    debugUtils.SetObjectName(m_BottomScratchBufferMemory->Handle(), "BLAS Scratch Memory");

    // Generate the structures.
    VkDeviceSize resultOffset  = 0;
    VkDeviceSize scratchOffset = 0;

    for(size_t i = 0; i != m_BottomAs.size(); ++i)
    {
        m_BottomAs[i].Generate(commandBuffer, *m_BottomScratchBuffer, scratchOffset, *m_BottomBuffer, resultOffset);

        resultOffset += m_BottomAs[i].BuildSizes().accelerationStructureSize;
        scratchOffset += m_BottomAs[i].BuildSizes().buildScratchSize;

        debugUtils.SetObjectName(m_BottomAs[i].Handle(), ("BLAS #" + std::to_string(i)).c_str());
    }
}

void Application::CreateTopLevelStructures(const VkCommandBuffer commandBuffer)
{
    const auto& scene      = GetScene();
    const auto& debugUtils = Device().DebugUtils();

    // Top level acceleration structure
    std::vector<VkAccelerationStructureInstanceKHR> instances;

    // Hit group 0: triangles
    // Hit group 1: procedurals
    uint32_t instanceId = 0;

    for(const auto& model : scene.Models())
    {
        instances.push_back(TopLevelAccelerationStructure::CreateInstance(
            m_BottomAs[instanceId], glm::mat4(1), instanceId, model.Procedural() ? 1 : 0));
        instanceId++;
    }

    // Create and copy instances buffer (do it in a separate one-time synchronous command buffer).
    Engine::BufferUtil::CreateDeviceBuffer(CommandPool(),
                                           "TLAS Instances",
                                           VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT,
                                           instances,
                                           m_InstancesBuffer,
                                           m_InstancesBufferMemory);

    // Memory barrier for the bottom level acceleration structure builds.
    AccelerationStructure::MemoryBarrier(commandBuffer);

    m_TopAs.emplace_back(*m_DeviceProcedures, *m_RayTracingProperties, m_InstancesBuffer->GetDeviceAddress(), static_cast<uint32_t>(instances.size()));

    // Allocate the structure memory.
    const auto total = GetTotalRequirements(m_TopAs);

    m_TopBuffer.reset(new Engine::Buffer(Device(), total.accelerationStructureSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR));
    m_TopBufferMemory.reset(new Engine::DeviceMemory(m_TopBuffer->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

    m_TopScratchBuffer.reset(new Engine::Buffer(Device(), total.buildScratchSize, VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT));
    m_TopScratchBufferMemory.reset(new Engine::DeviceMemory(m_TopScratchBuffer->AllocateMemory(VK_MEMORY_ALLOCATE_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));

    debugUtils.SetObjectName(m_TopBuffer->Handle(), "TLAS Buffer");
    debugUtils.SetObjectName(m_TopBufferMemory->Handle(), "TLAS Memory");
    debugUtils.SetObjectName(m_TopScratchBuffer->Handle(), "TLAS Scratch Buffer");
    debugUtils.SetObjectName(m_TopScratchBufferMemory->Handle(), "TLAS Scratch Memory");
    debugUtils.SetObjectName(m_InstancesBuffer->Handle(), "TLAS Instances Buffer");
    debugUtils.SetObjectName(m_InstancesBufferMemory->Handle(), "TLAS Instances Memory");

    // Generate the structures.
    m_TopAs[0].Generate(commandBuffer, *m_TopScratchBuffer, 0, *m_TopBuffer, 0);

    debugUtils.SetObjectName(m_TopAs[0].Handle(), "TLAS");
}

void Application::CreateOutputImage()
{
    const auto     extent = SwapChain().Extent();
    const auto     format = SwapChain().Format();
    constexpr auto tiling = VK_IMAGE_TILING_OPTIMAL;

    m_AccumulationImage.reset(new Engine::Image(Device(), extent, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_STORAGE_BIT));
    m_AccumulationImageMemory.reset(new Engine::DeviceMemory(m_AccumulationImage->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
    m_AccumulationImageView.reset(new Engine::ImageView(Device(), m_AccumulationImage->Handle(), VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT));

    m_OutputImage.reset(new Engine::Image(Device(), extent, format, tiling, VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT));
    m_OutputImageMemory.reset(new Engine::DeviceMemory(m_OutputImage->AllocateMemory(VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT)));
    m_OutputImageView.reset(new Engine::ImageView(Device(), m_OutputImage->Handle(), format, VK_IMAGE_ASPECT_COLOR_BIT));

    const auto& debugUtils = Device().DebugUtils();

    debugUtils.SetObjectName(m_AccumulationImage->Handle(), "Accumulation Image");
    debugUtils.SetObjectName(m_AccumulationImageMemory->Handle(), "Accumulation Image Memory");
    debugUtils.SetObjectName(m_AccumulationImageView->Handle(), "Accumulation ImageView");

    debugUtils.SetObjectName(m_OutputImage->Handle(), "Output Image");
    debugUtils.SetObjectName(m_OutputImageMemory->Handle(), "Output Image Memory");
    debugUtils.SetObjectName(m_OutputImageView->Handle(), "Output ImageView");
}

}    // namespace RayTracing
