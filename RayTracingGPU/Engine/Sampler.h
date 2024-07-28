#pragma once

#include <vulkan/vulkan.h>

namespace Engine
{
class Device;

struct SamplerConfig final
{
    VkFilter             MagFilter               = VK_FILTER_LINEAR;
    VkFilter             MinFilter               = VK_FILTER_LINEAR;
    VkSamplerAddressMode AddressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    VkSamplerAddressMode AddressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    VkSamplerAddressMode AddressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    bool                 AnisotropyEnable        = true;
    float                MaxAnisotropy           = 16;
    VkBorderColor        BorderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    bool                 UnnormalizedCoordinates = false;
    bool                 CompareEnable           = false;
    VkCompareOp          CompareOp               = VK_COMPARE_OP_ALWAYS;
    VkSamplerMipmapMode  MipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    float                MipLodBias              = 0.0f;
    float                MinLod                  = 0.0f;
    float                MaxLod                  = 0.0f;
};

class Sampler final
{
public:
    Sampler(const Device& device, const SamplerConfig& config);
    ~Sampler();

    Sampler(const Sampler&)            = delete;
    Sampler(Sampler&&)                 = delete;
    Sampler& operator=(const Sampler&) = delete;
    Sampler& operator=(Sampler&&)      = delete;

    VkSampler     Handle() const { return m_Sampler; }
    const Device& Device() const { return m_Device; }

private:
    VkSampler           m_Sampler{};
    const class Device& m_Device;
};

}    // namespace Engine
