#pragma once

#include "RayTracingProperties.h"

#include <Engine/Application.h>

#include <vulkan/vulkan.h>

namespace Engine
{
class CommandBuffers;
class Buffer;
class DeviceMemory;
class Image;
class ImageView;
}

namespace RayTracing
{
class Application : public Engine::Application
{
public:
    Application(const Application&)            = delete;
    Application(Application&&)                 = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&)      = delete;

protected:
    Application(const Engine::WindowConfig& windowConfig, VkPresentModeKHR presentMode, bool enableValidationLayers);
    ~Application() override;

    void SetPhysicalDevice(VkPhysicalDevice          physicalDevice,
                           std::vector<const char*>& requiredExtensions,
                           VkPhysicalDeviceFeatures& deviceFeatures,
                           void*                     nextDeviceFeatures) override;

    void OnDeviceSet() override;
    void CreateAccelerationStructures();
    void DeleteAccelerationStructures();
    void CreateSwapChain() override;
    void DeleteSwapChain() override;
    void Render(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

private:
    void CreateBottomLevelStructures(VkCommandBuffer commandBuffer);
    void CreateTopLevelStructures(VkCommandBuffer commandBuffer);
    void CreateOutputImage();

    std::unique_ptr<class DeviceProcedures> m_DeviceProcedures;
    std::unique_ptr<RayTracingProperties>   m_RayTracingProperties;

    std::vector<class BottomLevelAccelerationStructure> m_BottomAs;
    std::unique_ptr<Engine::Buffer>                     m_BottomBuffer;
    std::unique_ptr<Engine::DeviceMemory>               m_BottomBufferMemory;
    std::unique_ptr<Engine::Buffer>                     m_BottomScratchBuffer;
    std::unique_ptr<Engine::DeviceMemory>               m_BottomScratchBufferMemory;
    std::vector<class TopLevelAccelerationStructure>    m_TopAs;
    std::unique_ptr<Engine::Buffer>                     m_TopBuffer;
    std::unique_ptr<Engine::DeviceMemory>               m_TopBufferMemory;
    std::unique_ptr<Engine::Buffer>                     m_TopScratchBuffer;
    std::unique_ptr<Engine::DeviceMemory>               m_TopScratchBufferMemory;
    std::unique_ptr<Engine::Buffer>                     m_InstancesBuffer;
    std::unique_ptr<Engine::DeviceMemory>               m_InstancesBufferMemory;

    std::unique_ptr<Engine::Image>        m_AccumulationImage;
    std::unique_ptr<Engine::DeviceMemory> m_AccumulationImageMemory;
    std::unique_ptr<Engine::ImageView>    m_AccumulationImageView;

    std::unique_ptr<Engine::Image>        m_OutputImage;
    std::unique_ptr<Engine::DeviceMemory> m_OutputImageMemory;
    std::unique_ptr<Engine::ImageView>    m_OutputImageView;

    std::unique_ptr<class RayTracingPipeline> m_RayTracingPipeline;
    std::unique_ptr<class ShaderBindingTable> m_ShaderBindingTable;
};

}    // namespace RayTracing
