#include "Sampler.h"

#include "Device.h"

#include <Utils/VulkanException.h>

namespace Engine
{

Sampler::Sampler(const class Device& device, const SamplerConfig& config)
    : m_Device(device)
{
    VkSamplerCreateInfo samplerInfo     = {};
    samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter               = config.MagFilter;
    samplerInfo.minFilter               = config.MinFilter;
    samplerInfo.addressModeU            = config.AddressModeU;
    samplerInfo.addressModeV            = config.AddressModeV;
    samplerInfo.addressModeW            = config.AddressModeW;
    samplerInfo.anisotropyEnable        = config.AnisotropyEnable;
    samplerInfo.maxAnisotropy           = config.MaxAnisotropy;
    samplerInfo.borderColor             = config.BorderColor;
    samplerInfo.unnormalizedCoordinates = config.UnnormalizedCoordinates;
    samplerInfo.compareEnable           = config.CompareEnable;
    samplerInfo.compareOp               = config.CompareOp;
    samplerInfo.mipmapMode              = config.MipmapMode;
    samplerInfo.mipLodBias              = config.MipLodBias;
    samplerInfo.minLod                  = config.MinLod;
    samplerInfo.maxLod                  = config.MaxLod;

    if(vkCreateSampler(device.Handle(), &samplerInfo, nullptr, &m_Sampler) != VK_SUCCESS)
    {
        VK_THROW("failed to create sampler");
    }
}

Sampler::~Sampler()
{
    if(m_Sampler != nullptr)
    {
        vkDestroySampler(m_Device.Handle(), m_Sampler, nullptr);
        m_Sampler = nullptr;
    }
}

}    // namespace Engine
