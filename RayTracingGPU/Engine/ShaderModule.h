#pragma once

#include <vulkan/vulkan.h>

#include <string>
#include <vector>

namespace Engine
{
class Device;

class ShaderModule final
{
public:
    ShaderModule(const Device& device, const std::string& filename);
    ShaderModule(const Device& device, const std::vector<char>& code);
    ~ShaderModule();

    ShaderModule(const ShaderModule&)            = delete;
    ShaderModule(ShaderModule&&)                 = delete;
    ShaderModule& operator=(const ShaderModule&) = delete;
    ShaderModule& operator=(ShaderModule&&)      = delete;

    VkShaderModule Handle() const { return m_ShaderModule; }
    const Device&  Device() const { return m_Device; }

    VkPipelineShaderStageCreateInfo CreateShaderStage(VkShaderStageFlagBits stage) const;

private:
    static std::vector<char> ReadFile(const std::string& filename);

    VkShaderModule      m_ShaderModule{};
    const class Device& m_Device;
};

}    // namespace Engine
