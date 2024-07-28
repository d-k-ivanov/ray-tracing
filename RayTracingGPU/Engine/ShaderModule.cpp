#include "ShaderModule.h"

#include "Device.h"

#include <Utils/VulkanException.h>

#include <fstream>

namespace Engine
{

ShaderModule::ShaderModule(const class Device& device, const std::string& filename)
    : ShaderModule(device, ReadFile(filename))
{
}

ShaderModule::ShaderModule(const class Device& device, const std::vector<char>& code)
    : m_Device(device)
{
    VkShaderModuleCreateInfo createInfo = {};
    createInfo.sType                    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize                 = code.size();
    createInfo.pCode                    = reinterpret_cast<const uint32_t*>(code.data());

    VK_CHECK(vkCreateShaderModule(device.Handle(), &createInfo, nullptr, &m_ShaderModule));
}

ShaderModule::~ShaderModule()
{
    if(m_ShaderModule != nullptr)
    {
        vkDestroyShaderModule(m_Device.Handle(), m_ShaderModule, nullptr);
        m_ShaderModule = nullptr;
    }
}

VkPipelineShaderStageCreateInfo ShaderModule::CreateShaderStage(const VkShaderStageFlagBits stage) const
{
    VkPipelineShaderStageCreateInfo createInfo = {};
    createInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    createInfo.stage                           = stage;
    createInfo.module                          = m_ShaderModule;
    createInfo.pName                           = "main";

    return createInfo;
}

std::vector<char> ShaderModule::ReadFile(const std::string& filename)
{
    std::ifstream file(filename, std::ios::ate | std::ios::binary);

    if(!file.is_open())
    {
        VK_THROW("failed to open file '", filename, "'");
    }

    const auto        fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), fileSize);
    file.close();

    return buffer;
}

}    // namespace Engine
