#include "DescriptorSetManager.h"

#include "DescriptorPool.h"
#include "DescriptorSetLayout.h"
#include "DescriptorSets.h"
#include "Device.h"

#include <Utils/VulkanException.h>

#include <set>

namespace Engine
{

DescriptorSetManager::DescriptorSetManager(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings, const size_t maxSets)
{
    // Sanity check to avoid binding different resources to the same binding point.
    std::map<uint32_t, VkDescriptorType> bindingTypes;

    for(const auto& binding : descriptorBindings)
    {
        if(!bindingTypes.insert(std::make_pair(binding.Binding, binding.Type)).second)
        {
            VK_THROW("binding collision");
        }
    }

    m_DescriptorPool.reset(new DescriptorPool(device, descriptorBindings, maxSets));
    m_DescriptorSetLayout.reset(new class DescriptorSetLayout(device, descriptorBindings));
    m_DescriptorSets.reset(new class DescriptorSets(*m_DescriptorPool, *m_DescriptorSetLayout, bindingTypes, maxSets));
}

DescriptorSetManager::~DescriptorSetManager()
{
    m_DescriptorSets.reset();
    m_DescriptorSetLayout.reset();
    m_DescriptorPool.reset();
}

}    // namespace Engine
