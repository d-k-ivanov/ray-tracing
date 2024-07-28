#include "GraphicsPipeline.h"

#include "Buffer.h"
#include "DescriptorPool.h"
#include "DescriptorSetManager.h"
#include "DescriptorSets.h"
#include "Device.h"
#include "PipelineLayout.h"
#include "RenderPass.h"
#include "ShaderModule.h"
#include "SwapChain.h"

#include <Objects/Vertex.h>
#include <Render/UniformBuffer.h>
#include <Scenes/Scene.h>
#include <Utils/Filesystem.h>
#include <Utils/VulkanException.h>

namespace Engine
{

GraphicsPipeline::GraphicsPipeline(
    const SwapChain&                          swapChain,
    const DepthBuffer&                        depthBuffer,
    const std::vector<Render::UniformBuffer>& uniformBuffers,
    const Scenes::Scene&                      scene,
    const bool                                isWireFrame)
    : m_SwapChain(swapChain)
    , m_IsWireFrame(isWireFrame)
{
    const auto& device                = swapChain.Device();
    const auto  bindingDescription    = Objects::Vertex::GetBindingDescription();
    const auto  attributeDescriptions = Objects::Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType                                = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount        = 1;
    vertexInputInfo.pVertexBindingDescriptions           = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount      = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions         = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology                               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable                 = VK_FALSE;

    VkViewport viewport = {};
    viewport.x          = 0.0f;
    viewport.y          = 0.0f;
    viewport.width      = static_cast<float>(swapChain.Extent().width);
    viewport.height     = static_cast<float>(swapChain.Extent().height);
    viewport.minDepth   = 0.0f;
    viewport.maxDepth   = 1.0f;

    VkRect2D scissor = {};
    scissor.offset   = {0, 0};
    scissor.extent   = swapChain.Extent();

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType                             = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount                     = 1;
    viewportState.pViewports                        = &viewport;
    viewportState.scissorCount                      = 1;
    viewportState.pScissors                         = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType                                  = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable                       = VK_FALSE;
    rasterizer.rasterizerDiscardEnable                = VK_FALSE;
    rasterizer.polygonMode                            = isWireFrame ? VK_POLYGON_MODE_LINE : VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth                              = 1.0f;
    rasterizer.cullMode                               = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace                              = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable                        = VK_FALSE;
    rasterizer.depthBiasConstantFactor                = 0.0f;    // Optional
    rasterizer.depthBiasClamp                         = 0.0f;    // Optional
    rasterizer.depthBiasSlopeFactor                   = 0.0f;    // Optional

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType                                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable                  = VK_FALSE;
    multisampling.rasterizationSamples                 = VK_SAMPLE_COUNT_1_BIT;
    multisampling.minSampleShading                     = 1.0f;        // Optional
    multisampling.pSampleMask                          = nullptr;     // Optional
    multisampling.alphaToCoverageEnable                = VK_FALSE;    // Optional
    multisampling.alphaToOneEnable                     = VK_FALSE;    // Optional

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType                                 = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable                       = VK_TRUE;
    depthStencil.depthWriteEnable                      = VK_TRUE;
    depthStencil.depthCompareOp                        = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable                 = VK_FALSE;
    depthStencil.minDepthBounds                        = 0.0f;    // Optional
    depthStencil.maxDepthBounds                        = 1.0f;    // Optional
    depthStencil.stencilTestEnable                     = VK_FALSE;
    depthStencil.front                                 = {};    // Optional
    depthStencil.back                                  = {};    // Optional

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask                      = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable                         = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor                 = VK_BLEND_FACTOR_ONE;     // Optional
    colorBlendAttachment.dstColorBlendFactor                 = VK_BLEND_FACTOR_ZERO;    // Optional
    colorBlendAttachment.colorBlendOp                        = VK_BLEND_OP_ADD;         // Optional
    colorBlendAttachment.srcAlphaBlendFactor                 = VK_BLEND_FACTOR_ONE;     // Optional
    colorBlendAttachment.dstAlphaBlendFactor                 = VK_BLEND_FACTOR_ZERO;    // Optional
    colorBlendAttachment.alphaBlendOp                        = VK_BLEND_OP_ADD;         // Optional

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType                               = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable                       = VK_FALSE;
    colorBlending.logicOp                             = VK_LOGIC_OP_COPY;    // Optional
    colorBlending.attachmentCount                     = 1;
    colorBlending.pAttachments                        = &colorBlendAttachment;
    colorBlending.blendConstants[0]                   = 0.0f;    // Optional
    colorBlending.blendConstants[1]                   = 0.0f;    // Optional
    colorBlending.blendConstants[2]                   = 0.0f;    // Optional
    colorBlending.blendConstants[3]                   = 0.0f;    // Optional

    // Create descriptor pool/sets.
    std::vector<DescriptorBinding> descriptorBindings =
        {
            {0, 1, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT},
            {1, 1, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT},
            {2, static_cast<uint32_t>(scene.TextureSamplers().size()), VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}};

    m_DescriptorSetManager.reset(new DescriptorSetManager(device, descriptorBindings, uniformBuffers.size()));

    auto& descriptorSets = m_DescriptorSetManager->DescriptorSets();

    for(uint32_t i = 0; i != swapChain.Images().size(); ++i)
    {
        // Uniform buffer
        VkDescriptorBufferInfo uniformBufferInfo = {};
        uniformBufferInfo.buffer                 = uniformBuffers[i].Buffer().Handle();
        uniformBufferInfo.range                  = VK_WHOLE_SIZE;

        // Material buffer
        VkDescriptorBufferInfo materialBufferInfo = {};
        materialBufferInfo.buffer                 = scene.MaterialBuffer().Handle();
        materialBufferInfo.range                  = VK_WHOLE_SIZE;

        // Image and texture samplers
        std::vector<VkDescriptorImageInfo> imageInfos(scene.TextureSamplers().size());

        for(size_t t = 0; t != imageInfos.size(); ++t)
        {
            auto& imageInfo       = imageInfos[t];
            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            imageInfo.imageView   = scene.TextureImageViews()[t];
            imageInfo.sampler     = scene.TextureSamplers()[t];
        }

        const std::vector descriptorWrites =
            {
                descriptorSets.Bind(i, 0, uniformBufferInfo),
                descriptorSets.Bind(i, 1, materialBufferInfo),
                descriptorSets.Bind(i, 2, *imageInfos.data(), static_cast<uint32_t>(imageInfos.size()))};

        descriptorSets.UpdateDescriptors(i, descriptorWrites);
    }

    // Create pipeline layout and render pass.
    m_PipelineLayout.reset(new class PipelineLayout(device, m_DescriptorSetManager->DescriptorSetLayout()));
    m_RenderPass.reset(new class RenderPass(swapChain, depthBuffer, VK_ATTACHMENT_LOAD_OP_CLEAR, VK_ATTACHMENT_LOAD_OP_CLEAR));

    // Load shaders.
    const ShaderModule vertShader(device, Utils::ThisExecutableLocation() + "/Resources/Shaders/Graphics.vert.spv");
    const ShaderModule fragShader(device, Utils::ThisExecutableLocation() + "/Resources/Shaders/Graphics.frag.spv");

    VkPipelineShaderStageCreateInfo shaderStages[] =
        {
            vertShader.CreateShaderStage(VK_SHADER_STAGE_VERTEX_BIT),
            fragShader.CreateShaderStage(VK_SHADER_STAGE_FRAGMENT_BIT)};

    // Create graphic pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType                        = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount                   = 2;
    pipelineInfo.pStages                      = shaderStages;
    pipelineInfo.pVertexInputState            = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState          = &inputAssembly;
    pipelineInfo.pViewportState               = &viewportState;
    pipelineInfo.pRasterizationState          = &rasterizer;
    pipelineInfo.pMultisampleState            = &multisampling;
    pipelineInfo.pDepthStencilState           = &depthStencil;
    pipelineInfo.pColorBlendState             = &colorBlending;
    pipelineInfo.pDynamicState                = nullptr;    // Optional
    pipelineInfo.basePipelineHandle           = nullptr;    // Optional
    pipelineInfo.basePipelineIndex            = -1;         // Optional
    pipelineInfo.layout                       = m_PipelineLayout->Handle();
    pipelineInfo.renderPass                   = m_RenderPass->Handle();
    pipelineInfo.subpass                      = 0;

    VK_CHECK(vkCreateGraphicsPipelines(device.Handle(), nullptr, 1, &pipelineInfo, nullptr, &m_Pipeline));
}

GraphicsPipeline::~GraphicsPipeline()
{
    if(m_Pipeline != nullptr)
    {
        vkDestroyPipeline(m_SwapChain.Device().Handle(), m_Pipeline, nullptr);
        m_Pipeline = nullptr;
    }

    m_RenderPass.reset();
    m_PipelineLayout.reset();
    m_DescriptorSetManager.reset();
}

VkDescriptorSet GraphicsPipeline::DescriptorSet(const uint32_t index) const
{
    return m_DescriptorSetManager->DescriptorSets().Handle(index);
}

}    // namespace Engine
