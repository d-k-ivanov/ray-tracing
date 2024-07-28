#include "DescriptorSets.h"

#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "Device.h"

#include <Utils/VulkanException.h>

#include <utility>

namespace Engine
{

DescriptorSets::DescriptorSets(
    const DescriptorPool&                descriptorPool,
    const DescriptorSetLayout&           layout,
    std::map<uint32_t, VkDescriptorType> bindingTypes,
    const size_t                         size)
    : m_DescriptorPool(descriptorPool)
    , m_BindingTypes(std::move(bindingTypes))
{
    const std::vector layouts(size, layout.Handle());

    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType                       = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool              = descriptorPool.Handle();
    allocInfo.descriptorSetCount          = static_cast<uint32_t>(size);
    allocInfo.pSetLayouts                 = layouts.data();

    m_DescriptorSets.resize(size);

    VK_CHECK(vkAllocateDescriptorSets(descriptorPool.Device().Handle(), &allocInfo, m_DescriptorSets.data()));
}

DescriptorSets::~DescriptorSets()
{
    // if(!descriptorSets_.empty())
    // {
    //     vkFreeDescriptorSets(
    //         descriptorPool_.Device().Handle(),
    //         descriptorPool_.Handle(),
    //         static_cast<uint32_t>(descriptorSets_.size()),
    //         descriptorSets_.data());
    //     descriptorSets_.clear();
    // }
}

VkWriteDescriptorSet DescriptorSets::Bind(const uint32_t index, const uint32_t binding, const VkDescriptorBufferInfo& bufferInfo, const uint32_t count) const
{
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet               = m_DescriptorSets[index];
    descriptorWrite.dstBinding           = binding;
    descriptorWrite.dstArrayElement      = 0;
    descriptorWrite.descriptorType       = GetBindingType(binding);
    descriptorWrite.descriptorCount      = count;
    descriptorWrite.pBufferInfo          = &bufferInfo;

    return descriptorWrite;
}

VkWriteDescriptorSet DescriptorSets::Bind(const uint32_t index, const uint32_t binding, const VkDescriptorImageInfo& imageInfo, const uint32_t count) const
{
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet               = m_DescriptorSets[index];
    descriptorWrite.dstBinding           = binding;
    descriptorWrite.dstArrayElement      = 0;
    descriptorWrite.descriptorType       = GetBindingType(binding);
    descriptorWrite.descriptorCount      = count;
    descriptorWrite.pImageInfo           = &imageInfo;

    return descriptorWrite;
}

VkWriteDescriptorSet DescriptorSets::Bind(const uint32_t index, const uint32_t binding, const VkWriteDescriptorSetAccelerationStructureKHR& structureInfo, const uint32_t count) const
{
    VkWriteDescriptorSet descriptorWrite = {};
    descriptorWrite.sType                = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrite.dstSet               = m_DescriptorSets[index];
    descriptorWrite.dstBinding           = binding;
    descriptorWrite.dstArrayElement      = 0;
    descriptorWrite.descriptorType       = GetBindingType(binding);
    descriptorWrite.descriptorCount      = count;
    descriptorWrite.pNext                = &structureInfo;

    return descriptorWrite;
}

void DescriptorSets::UpdateDescriptors(uint32_t index, const std::vector<VkWriteDescriptorSet>& descriptorWrites) const
{
    vkUpdateDescriptorSets(
        m_DescriptorPool.Device().Handle(),
        static_cast<uint32_t>(descriptorWrites.size()),
        descriptorWrites.data(), 0, nullptr);
}

VkDescriptorType DescriptorSets::GetBindingType(const uint32_t binding) const
{
    const auto it = m_BindingTypes.find(binding);
    if(it == m_BindingTypes.end())
    {
        VK_THROW("binding not found");
    }

    return it->second;
}

}    // namespace Engine
