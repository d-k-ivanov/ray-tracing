#pragma once

#include "DescriptorBinding.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Engine
{
class Device;

class DescriptorSetLayout final
{
public:
    DescriptorSetLayout(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings);
    ~DescriptorSetLayout();

    DescriptorSetLayout(const DescriptorSetLayout&)            = delete;
    DescriptorSetLayout(DescriptorSetLayout&&)                 = delete;
    DescriptorSetLayout& operator=(const DescriptorSetLayout&) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout&&)      = delete;

    VkDescriptorSetLayout Handle() const { return m_Layout; }

private:
    VkDescriptorSetLayout m_Layout{};
    const Device&         m_Device;
};

}    // namespace Engine
