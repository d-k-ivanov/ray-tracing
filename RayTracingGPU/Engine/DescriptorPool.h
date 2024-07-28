#pragma once

#include "DescriptorBinding.h"

#include <vulkan/vulkan.h>

#include <vector>

namespace Engine
{
class Device;

class DescriptorPool final
{
public:
    DescriptorPool(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings, size_t maxSets);
    ~DescriptorPool();

    DescriptorPool(const DescriptorPool&)            = delete;
    DescriptorPool(DescriptorPool&&)                 = delete;
    DescriptorPool& operator=(const DescriptorPool&) = delete;
    DescriptorPool& operator=(DescriptorPool&&)      = delete;

    VkDescriptorPool Handle() const { return m_DescriptorPool; }
    const Device&    Device() const { return m_Device; }

private:
    VkDescriptorPool    m_DescriptorPool{};
    const class Device& m_Device;
};

}    // namespace Engine
