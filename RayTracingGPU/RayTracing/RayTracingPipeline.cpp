#include "RayTracingPipeline.h"

#include "DeviceProcedures.h"
#include "TopLevelAccelerationStructure.h"

#include <Engine/Buffer.h>
#include <Engine/DescriptorBinding.h>
#include <Engine/DescriptorSetManager.h>
#include <Engine/DescriptorSets.h>
#include <Engine/Device.h>
#include <Engine/ImageView.h>
#include <Engine/PipelineLayout.h>
#include <Engine/ShaderModule.h>
#include <Engine/SwapChain.h>
#include <Render/UniformBuffer.h>
#include <Scenes/Scene.h>
#include <Utils/Filesystem.h>
#include <Utils/VulkanException.h>

namespace RayTracing
{

RayTracingPipeline::RayTracingPipeline(
    const DeviceProcedures&                   deviceProcedures,
    const Engine::SwapChain&                  swapChain,
    const TopLevelAccelerationStructure&      accelerationStructure,
    const Engine::ImageView&                  accumulationImageView,
    const Engine::ImageView&                  outputImageView,
    const std::vector<Render::UniformBuffer>& uniformBuffers,
    const Scenes::Scene&                      scene)
    : m_SwapChain(swapChain)
{
    // Create descriptor pool/sets.
    const auto&                                  device = swapChain.Device();
    const std::vector<Engine::DescriptorBinding> descriptorBindings =
        {
            // Top level acceleration structure.
            {0, 1, VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR, VK_SHADER_STAGE_RAYGEN_BIT_KHR},

            // Image accumulation & output
            {1, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR},
            {2, 1, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, VK_SHADER_STAGE_RAYGEN_BIT_KHR},

            // Camera information & co
            {3, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_RAYGEN_BIT_KHR | VK_SHADER_STAGE_MISS_BIT_KHR},

            // Vertex buffer, Index buffer, Material buffer, Offset buffer
            {4, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},
            {5, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},
            {6, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},
            {7, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},

            // Textures and image samplers
            {8, static_cast<uint32_t>(scene.TextureSamplers().size()), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR},

            // The Procedural buffer.
            {9, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR | VK_SHADER_STAGE_INTERSECTION_BIT_KHR}};

    m_DescriptorSetManager.reset(new Engine::DescriptorSetManager(device, descriptorBindings, uniformBuffers.size()));

    auto& descriptorSets = m_DescriptorSetManager->DescriptorSets();

    for(uint32_t i = 0; i != swapChain.Images().size(); ++i)
    {
        // Top level acceleration structure.
        const auto                                   accelerationStructureHandle = accelerationStructure.Handle();
        VkWriteDescriptorSetAccelerationStructureKHR structureInfo               = {};
        structureInfo.sType                                                      = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
        structureInfo.pNext                                                      = nullptr;
        structureInfo.accelerationStructureCount                                 = 1;
        structureInfo.pAccelerationStructures                                    = &accelerationStructureHandle;

        // Accumulation image
        VkDescriptorImageInfo accumulationImageInfo = {};
        accumulationImageInfo.imageView             = accumulationImageView.Handle();
        accumulationImageInfo.imageLayout           = VK_IMAGE_LAYOUT_GENERAL;

        // Output image
        VkDescriptorImageInfo outputImageInfo = {};
        outputImageInfo.imageView             = outputImageView.Handle();
        outputImageInfo.imageLayout           = VK_IMAGE_LAYOUT_GENERAL;

        // Uniform buffer
        VkDescriptorBufferInfo uniformBufferInfo = {};
        uniformBufferInfo.buffer                 = uniformBuffers[i].Buffer().Handle();
        uniformBufferInfo.range                  = VK_WHOLE_SIZE;

        // Vertex buffer
        VkDescriptorBufferInfo vertexBufferInfo = {};
        vertexBufferInfo.buffer                 = scene.VertexBuffer().Handle();
        vertexBufferInfo.range                  = VK_WHOLE_SIZE;

        // Index buffer
        VkDescriptorBufferInfo indexBufferInfo = {};
        indexBufferInfo.buffer                 = scene.IndexBuffer().Handle();
        indexBufferInfo.range                  = VK_WHOLE_SIZE;

        // Material buffer
        VkDescriptorBufferInfo materialBufferInfo = {};
        materialBufferInfo.buffer                 = scene.MaterialBuffer().Handle();
        materialBufferInfo.range                  = VK_WHOLE_SIZE;

        // Offsets buffer
        VkDescriptorBufferInfo offsetsBufferInfo = {};
        offsetsBufferInfo.buffer                 = scene.OffsetsBuffer().Handle();
        offsetsBufferInfo.range                  = VK_WHOLE_SIZE;

        // Image and texture samplers.
        std::vector<VkDescriptorImageInfo> imageInfos(scene.TextureSamplers().size());

        for(size_t t = 0; t != imageInfos.size(); ++t)
        {
            auto& imageInfo       = imageInfos[t];
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView   = scene.TextureImageViews()[t];
            imageInfo.sampler     = scene.TextureSamplers()[t];
        }

        std::vector descriptorWrites =
            {
                descriptorSets.Bind(i, 0, structureInfo),
                descriptorSets.Bind(i, 1, accumulationImageInfo),
                descriptorSets.Bind(i, 2, outputImageInfo),
                descriptorSets.Bind(i, 3, uniformBufferInfo),
                descriptorSets.Bind(i, 4, vertexBufferInfo),
                descriptorSets.Bind(i, 5, indexBufferInfo),
                descriptorSets.Bind(i, 6, materialBufferInfo),
                descriptorSets.Bind(i, 7, offsetsBufferInfo),
                descriptorSets.Bind(i, 8, *imageInfos.data(), static_cast<uint32_t>(imageInfos.size()))};

        // Procedural buffer (optional)
        VkDescriptorBufferInfo proceduralBufferInfo = {};

        if(scene.HasProcedurals())
        {
            proceduralBufferInfo.buffer = scene.ProceduralBuffer().Handle();
            proceduralBufferInfo.range  = VK_WHOLE_SIZE;

            descriptorWrites.push_back(descriptorSets.Bind(i, 9, proceduralBufferInfo));
        }

        descriptorSets.UpdateDescriptors(i, descriptorWrites);
    }

    m_PipelineLayout.reset(new Engine::PipelineLayout(device, m_DescriptorSetManager->DescriptorSetLayout()));

    // Load shaders.
    const Engine::ShaderModule rayGenShader(device, Utils::ThisExecutableLocation() + "/Resources/Shaders/RayTracing.rgen.spv");
    const Engine::ShaderModule missShader(device, Utils::ThisExecutableLocation() + "/Resources/Shaders/RayTracing.rmiss.spv");
    const Engine::ShaderModule closestHitShader(device, Utils::ThisExecutableLocation() + "/Resources/Shaders/RayTracing.rchit.spv");
    const Engine::ShaderModule proceduralClosestHitShader(device, Utils::ThisExecutableLocation() + "/Resources/Shaders/RayTracing.Procedural.rchit.spv");
    const Engine::ShaderModule proceduralIntersectionShader(device, Utils::ThisExecutableLocation() + "/Resources/Shaders/RayTracing.Procedural.rint.spv");

    std::vector shaderStages =
        {
            rayGenShader.CreateShaderStage(VK_SHADER_STAGE_RAYGEN_BIT_KHR),
            missShader.CreateShaderStage(VK_SHADER_STAGE_MISS_BIT_KHR),
            closestHitShader.CreateShaderStage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
            proceduralClosestHitShader.CreateShaderStage(VK_SHADER_STAGE_CLOSEST_HIT_BIT_KHR),
            proceduralIntersectionShader.CreateShaderStage(VK_SHADER_STAGE_INTERSECTION_BIT_KHR)};

    // Shader groups
    VkRayTracingShaderGroupCreateInfoKHR rayGenGroupInfo = {};
    rayGenGroupInfo.sType                                = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    rayGenGroupInfo.pNext                                = nullptr;
    rayGenGroupInfo.type                                 = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    rayGenGroupInfo.generalShader                        = 0;
    rayGenGroupInfo.closestHitShader                     = VK_SHADER_UNUSED_KHR;
    rayGenGroupInfo.anyHitShader                         = VK_SHADER_UNUSED_KHR;
    rayGenGroupInfo.intersectionShader                   = VK_SHADER_UNUSED_KHR;
    m_RayGenIndex                                        = 0;

    VkRayTracingShaderGroupCreateInfoKHR missGroupInfo = {};
    missGroupInfo.sType                                = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    missGroupInfo.pNext                                = nullptr;
    missGroupInfo.type                                 = VK_RAY_TRACING_SHADER_GROUP_TYPE_GENERAL_KHR;
    missGroupInfo.generalShader                        = 1;
    missGroupInfo.closestHitShader                     = VK_SHADER_UNUSED_KHR;
    missGroupInfo.anyHitShader                         = VK_SHADER_UNUSED_KHR;
    missGroupInfo.intersectionShader                   = VK_SHADER_UNUSED_KHR;
    m_MissIndex                                        = 1;

    VkRayTracingShaderGroupCreateInfoKHR triangleHitGroupInfo = {};
    triangleHitGroupInfo.sType                                = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    triangleHitGroupInfo.pNext                                = nullptr;
    triangleHitGroupInfo.type                                 = VK_RAY_TRACING_SHADER_GROUP_TYPE_TRIANGLES_HIT_GROUP_KHR;
    triangleHitGroupInfo.generalShader                        = VK_SHADER_UNUSED_KHR;
    triangleHitGroupInfo.closestHitShader                     = 2;
    triangleHitGroupInfo.anyHitShader                         = VK_SHADER_UNUSED_KHR;
    triangleHitGroupInfo.intersectionShader                   = VK_SHADER_UNUSED_KHR;
    m_TriangleHitGroupIndex                                   = 2;

    VkRayTracingShaderGroupCreateInfoKHR proceduralHitGroupInfo = {};
    proceduralHitGroupInfo.sType                                = VK_STRUCTURE_TYPE_RAY_TRACING_SHADER_GROUP_CREATE_INFO_KHR;
    proceduralHitGroupInfo.pNext                                = nullptr;
    proceduralHitGroupInfo.type                                 = VK_RAY_TRACING_SHADER_GROUP_TYPE_PROCEDURAL_HIT_GROUP_KHR;
    proceduralHitGroupInfo.generalShader                        = VK_SHADER_UNUSED_KHR;
    proceduralHitGroupInfo.closestHitShader                     = 3;
    proceduralHitGroupInfo.anyHitShader                         = VK_SHADER_UNUSED_KHR;
    proceduralHitGroupInfo.intersectionShader                   = 4;
    m_ProceduralHitGroupIndex                                   = 3;

    std::vector groups =
        {
            rayGenGroupInfo,
            missGroupInfo,
            triangleHitGroupInfo,
            proceduralHitGroupInfo,
        };

    // Create graphic pipeline
    VkRayTracingPipelineCreateInfoKHR pipelineInfo = {};
    pipelineInfo.sType                             = VK_STRUCTURE_TYPE_RAY_TRACING_PIPELINE_CREATE_INFO_KHR;
    pipelineInfo.pNext                             = nullptr;
    pipelineInfo.flags                             = 0;
    pipelineInfo.stageCount                        = static_cast<uint32_t>(shaderStages.size());
    pipelineInfo.pStages                           = shaderStages.data();
    pipelineInfo.groupCount                        = static_cast<uint32_t>(groups.size());
    pipelineInfo.pGroups                           = groups.data();
    pipelineInfo.maxPipelineRayRecursionDepth      = 1;
    pipelineInfo.layout                            = m_PipelineLayout->Handle();
    pipelineInfo.basePipelineHandle                = nullptr;
    pipelineInfo.basePipelineIndex                 = 0;

    VK_CHECK(deviceProcedures.vkCreateRayTracingPipelinesKHR(device.Handle(), nullptr, nullptr, 1, &pipelineInfo, nullptr, &m_Pipeline));
}

RayTracingPipeline::~RayTracingPipeline()
{
    if(m_Pipeline != nullptr)
    {
        vkDestroyPipeline(m_SwapChain.Device().Handle(), m_Pipeline, nullptr);
        m_Pipeline = nullptr;
    }

    m_PipelineLayout.reset();
    m_DescriptorSetManager.reset();
}

VkDescriptorSet RayTracingPipeline::DescriptorSet(const uint32_t index) const
{
    return m_DescriptorSetManager->DescriptorSets().Handle(index);
}

}    // namespace RayTracing
