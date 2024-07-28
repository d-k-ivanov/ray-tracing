#pragma once

#include <vulkan/vulkan.h>

#include <map>
#include <vector>

namespace Engine
{
class Buffer;
class DescriptorPool;
class DescriptorSetLayout;
class ImageView;

class DescriptorSets final
{
public:
    DescriptorSets(const DescriptorPool& descriptorPool, const DescriptorSetLayout& layout, std::map<uint32_t, VkDescriptorType> bindingTypes, size_t size);
    ~DescriptorSets();

    DescriptorSets(const DescriptorSets&)            = delete;
    DescriptorSets(DescriptorSets&&)                 = delete;
    DescriptorSets& operator=(const DescriptorSets&) = delete;
    DescriptorSets& operator=(DescriptorSets&&)      = delete;

    VkDescriptorSet Handle(const uint32_t index) const { return m_DescriptorSets[index]; }

    VkWriteDescriptorSet Bind(uint32_t index, uint32_t binding, const VkDescriptorBufferInfo& bufferInfo, uint32_t count = 1) const;
    VkWriteDescriptorSet Bind(uint32_t index, uint32_t binding, const VkDescriptorImageInfo& imageInfo, uint32_t count = 1) const;
    VkWriteDescriptorSet Bind(uint32_t index, uint32_t binding, const VkWriteDescriptorSetAccelerationStructureKHR& structureInfo, uint32_t count = 1) const;

    void UpdateDescriptors(uint32_t index, const std::vector<VkWriteDescriptorSet>& descriptorWrites) const;

private:
    VkDescriptorType GetBindingType(uint32_t binding) const;

    const DescriptorPool&                      m_DescriptorPool;
    const std::map<uint32_t, VkDescriptorType> m_BindingTypes;

    std::vector<VkDescriptorSet> m_DescriptorSets;
};

}    // namespace Engine
