#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class DescriptorSetLayout;
class Device;

class PipelineLayout final
{
public:
    PipelineLayout(const Device& device, const DescriptorSetLayout& descriptorSetLayout);
    ~PipelineLayout();

    PipelineLayout(const PipelineLayout&)            = delete;
    PipelineLayout(PipelineLayout&&)                 = delete;
    PipelineLayout& operator=(const PipelineLayout&) = delete;
    PipelineLayout& operator=(PipelineLayout&&)      = delete;

    VkPipelineLayout Handle() const { return m_PipelineLayout; }

private:
    VkPipelineLayout m_PipelineLayout{};
    const Device&    m_Device;
};

}    // namespace Engine
