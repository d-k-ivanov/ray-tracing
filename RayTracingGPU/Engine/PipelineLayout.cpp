#include "PipelineLayout.h"
#include "DescriptorSetLayout.h"
#include "Device.h"

#include <Utils/VulkanException.h>

namespace Engine
{

PipelineLayout::PipelineLayout(const Device& device, const DescriptorSetLayout& descriptorSetLayout)
    : m_Device(device)
{
    const VkDescriptorSetLayout descriptorSetLayouts[] = {descriptorSetLayout.Handle()};

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType                      = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount             = 1;
    pipelineLayoutInfo.pSetLayouts                = descriptorSetLayouts;
    pipelineLayoutInfo.pushConstantRangeCount     = 0;          // Optional
    pipelineLayoutInfo.pPushConstantRanges        = nullptr;    // Optional

    VK_CHECK(vkCreatePipelineLayout(m_Device.Handle(), &pipelineLayoutInfo, nullptr, &m_PipelineLayout));
}

PipelineLayout::~PipelineLayout()
{
    if(m_PipelineLayout != nullptr)
    {
        vkDestroyPipelineLayout(m_Device.Handle(), m_PipelineLayout, nullptr);
        m_PipelineLayout = nullptr;
    }
}

}    // namespace Engine
