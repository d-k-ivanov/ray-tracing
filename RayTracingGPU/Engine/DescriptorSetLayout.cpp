#include "DescriptorSetLayout.h"
#include "Device.h"

#include <Utils/VulkanException.h>

namespace Engine
{

DescriptorSetLayout::DescriptorSetLayout(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings)
    : m_Device(device)
{
    std::vector<VkDescriptorSetLayoutBinding> layoutBindings;

    for(const auto& binding : descriptorBindings)
    {
        VkDescriptorSetLayoutBinding b = {};
        b.binding                      = binding.Binding;
        b.descriptorCount              = binding.DescriptorCount;
        b.descriptorType               = binding.Type;
        b.stageFlags                   = binding.Stage;

        layoutBindings.push_back(b);
    }

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType                           = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount                    = static_cast<uint32_t>(layoutBindings.size());
    layoutInfo.pBindings                       = layoutBindings.data();

    VK_CHECK(vkCreateDescriptorSetLayout(device.Handle(), &layoutInfo, nullptr, &m_Layout));
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    if(m_Layout != nullptr)
    {
        vkDestroyDescriptorSetLayout(m_Device.Handle(), m_Layout, nullptr);
        m_Layout = nullptr;
    }
}

}    // namespace Engine
