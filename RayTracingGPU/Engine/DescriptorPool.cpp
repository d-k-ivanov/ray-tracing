#include "DescriptorPool.h"

#include "Device.h"

#include <Utils/VulkanException.h>

namespace Engine
{

DescriptorPool::DescriptorPool(const Engine::Device& device, const std::vector<DescriptorBinding>& descriptorBindings, const size_t maxSets)
    : m_Device(device)
{
    std::vector<VkDescriptorPoolSize> poolSizes;

    for(const auto& binding : descriptorBindings)
    {
        poolSizes.push_back(VkDescriptorPoolSize{binding.Type, static_cast<uint32_t>(binding.DescriptorCount * maxSets)});
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType                      = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags                      = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.poolSizeCount              = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes                 = poolSizes.data();
    poolInfo.maxSets                    = static_cast<uint32_t>(maxSets);

    VK_CHECK(vkCreateDescriptorPool(device.Handle(), &poolInfo, nullptr, &m_DescriptorPool));
}

DescriptorPool::~DescriptorPool()
{
    if(m_DescriptorPool != nullptr)
    {
        vkDestroyDescriptorPool(m_Device.Handle(), m_DescriptorPool, nullptr);
        m_DescriptorPool = nullptr;
    }
}

}    // namespace Engine
