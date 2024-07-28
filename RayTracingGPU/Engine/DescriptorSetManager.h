#pragma once

#include "DescriptorBinding.h"

#include <memory>
#include <vector>

namespace Engine
{
class Device;
class DescriptorPool;
class DescriptorSetLayout;
class DescriptorSets;

class DescriptorSetManager final
{
public:
    explicit DescriptorSetManager(const Device& device, const std::vector<DescriptorBinding>& descriptorBindings, size_t maxSets);
    ~DescriptorSetManager();

    DescriptorSetManager(const DescriptorSetManager&)            = delete;
    DescriptorSetManager(DescriptorSetManager&&)                 = delete;
    DescriptorSetManager& operator=(const DescriptorSetManager&) = delete;
    DescriptorSetManager& operator=(DescriptorSetManager&&)      = delete;

    const DescriptorSetLayout& DescriptorSetLayout() const { return *m_DescriptorSetLayout; }
    DescriptorSets&            DescriptorSets() const { return *m_DescriptorSets; }

private:
    std::unique_ptr<DescriptorPool>            m_DescriptorPool;
    std::unique_ptr<class DescriptorSetLayout> m_DescriptorSetLayout;
    std::unique_ptr<class DescriptorSets>      m_DescriptorSets;
};

}    // namespace Engine
