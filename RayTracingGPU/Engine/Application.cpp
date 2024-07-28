#include "Application.h"

#include "Buffer.h"
#include "CommandBuffers.h"
#include "CommandPool.h"
#include "DebugUtilsMessenger.h"
#include "DepthBuffer.h"
#include "Device.h"
#include "Fence.h"
#include "FrameBuffer.h"
#include "GraphicsPipeline.h"
#include "Instance.h"
#include "PipelineLayout.h"
#include "RenderPass.h"
#include "Semaphore.h"
#include "Strings.h"
#include "Surface.h"
#include "SwapChain.h"
#include "Window.h"

#include <Objects/Model.h>
#include <Render/UniformBuffer.h>
#include <Scenes/Scene.h>
#include <Utils/VulkanException.h>

#include <array>

namespace Engine
{

Application::Application(const WindowConfig& windowConfig, const VkPresentModeKHR presentMode, const bool enableValidationLayers)
    : m_PresentMode(presentMode)
{
    const auto validationLayers = enableValidationLayers
                                      ? std::vector{"VK_LAYER_KHRONOS_validation"}
                                      : std::vector<const char*>();

    m_Window.reset(new class Window(windowConfig));
    m_Instance.reset(new Instance(*m_Window, validationLayers, VK_API_VERSION_1_2));
    m_DebugUtilsMessenger.reset(enableValidationLayers ? new DebugUtilsMessenger(*m_Instance, VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT) : nullptr);
    m_Surface.reset(new Surface(*m_Instance));
}

Application::~Application()
{
    Application::DeleteSwapChain();

    m_CommandPool.reset();
    m_Device.reset();
    m_Surface.reset();
    m_DebugUtilsMessenger.reset();
    m_Instance.reset();
    m_Window.reset();
}

const std::vector<VkExtensionProperties>& Application::Extensions() const
{
    return m_Instance->Extensions();
}

const std::vector<VkLayerProperties>& Application::Layers() const
{
    return m_Instance->Layers();
}

const std::vector<VkPhysicalDevice>& Application::PhysicalDevices() const
{
    return m_Instance->PhysicalDevices();
}

void Application::SetPhysicalDevice(const VkPhysicalDevice physicalDevice)
{
    if(m_Device)
    {
        VK_THROW("physical device has already been set");
    }

    // VK_KHR_swapchain
    std::vector requiredExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

    VkPhysicalDeviceFeatures deviceFeatures = {};

    SetPhysicalDevice(physicalDevice, requiredExtensions, deviceFeatures, nullptr);
    OnDeviceSet();

    // Create swap chain and command buffers.
    CreateSwapChain();
}

void Application::Run()
{
    if(!m_Device)
    {
        VK_THROW("physical device has not been set");
    }

    m_CurrentFrame = 0;

    m_Window->DrawFrame = [this]()
    {
        DrawFrame();
    };
    m_Window->OnKey = [this](const int key, const int scancode, const int action, const int mods)
    {
        OnKey(key, scancode, action, mods);
    };
    m_Window->OnCursorPosition = [this](const double xpos, const double ypos)
    {
        OnCursorPosition(xpos, ypos);
    };
    m_Window->OnMouseButton = [this](const int button, const int action, const int mods)
    {
        OnMouseButton(button, action, mods);
    };
    m_Window->OnScroll = [this](const double xoffset, const double yoffset)
    {
        OnScroll(xoffset, yoffset);
    };
    m_Window->Run();
    m_Device->WaitIdle();
}

void Application::SetPhysicalDevice(const VkPhysicalDevice    physicalDevice,
                                    std::vector<const char*>& requiredExtensions,
                                    VkPhysicalDeviceFeatures& deviceFeatures,
                                    void*                     nextDeviceFeatures)
{
    m_Device.reset(new class Device(physicalDevice, *m_Surface, requiredExtensions, deviceFeatures, nextDeviceFeatures));
    m_CommandPool.reset(new class CommandPool(*m_Device, m_Device->GraphicsFamilyIndex(), true));
}

void Application::OnDeviceSet()
{
}

void Application::CreateSwapChain()
{
    // Wait until the window is visible.
    while(m_Window->IsMinimized())
    {
        m_Window->WaitForEvents();
    }

    m_SwapChain.reset(new class SwapChain(*m_Device, m_PresentMode));
    m_DepthBuffer.reset(new class DepthBuffer(*m_CommandPool, m_SwapChain->Extent()));

    for(size_t i = 0; i != m_SwapChain->ImageViews().size(); ++i)
    {
        m_ImageAvailableSemaphores.emplace_back(*m_Device);
        m_RenderFinishedSemaphores.emplace_back(*m_Device);
        m_InFlightFences.emplace_back(*m_Device, true);
        m_UniformBuffers.emplace_back(*m_Device);
    }

    m_GraphicsPipeline.reset(new class GraphicsPipeline(*m_SwapChain, *m_DepthBuffer, m_UniformBuffers, GetScene(), m_IsWireFrame));

    for(const auto& imageView : m_SwapChain->ImageViews())
    {
        m_SwapChainFramebuffers.emplace_back(*imageView, m_GraphicsPipeline->RenderPass());
    }

    m_CommandBuffers.reset(new CommandBuffers(*m_CommandPool, static_cast<uint32_t>(m_SwapChainFramebuffers.size())));
}

void Application::DeleteSwapChain()
{
    m_CommandBuffers.reset();
    m_SwapChainFramebuffers.clear();
    m_GraphicsPipeline.reset();
    m_UniformBuffers.clear();
    m_InFlightFences.clear();
    m_RenderFinishedSemaphores.clear();
    m_ImageAvailableSemaphores.clear();
    m_DepthBuffer.reset();
    m_SwapChain.reset();
}

void Application::DrawFrame()
{
    constexpr auto noTimeout = std::numeric_limits<uint64_t>::max();

    const auto& inFlightFence           = m_InFlightFences[m_CurrentFrame];
    const auto  imageAvailableSemaphore = m_ImageAvailableSemaphores[m_CurrentFrame].Handle();
    const auto  renderFinishedSemaphore = m_RenderFinishedSemaphores[m_CurrentFrame].Handle();

    inFlightFence.Wait(noTimeout);

    uint32_t imageIndex;
    auto     result = vkAcquireNextImageKHR(m_Device->Handle(), m_SwapChain->Handle(), noTimeout, imageAvailableSemaphore, nullptr, &imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m_IsWireFrame != m_GraphicsPipeline->IsWireFrame())
    {
        RecreateSwapChain();
        return;
    }

    if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
    {
        VK_THROW(std::string("failed to acquire next image ("), Strings::Result(result), ")");
    }

    const auto commandBuffer = m_CommandBuffers->Begin(imageIndex);
    Render(commandBuffer, imageIndex);
    m_CommandBuffers->End(imageIndex);

    UpdateUniformBuffer(imageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType        = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    const VkCommandBuffer          commandBuffers[]{commandBuffer};
    const VkSemaphore              waitSemaphores[]   = {imageAvailableSemaphore};
    constexpr VkPipelineStageFlags waitStages[]       = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
    const VkSemaphore              signalSemaphores[] = {renderFinishedSemaphore};

    submitInfo.waitSemaphoreCount   = 1;
    submitInfo.pWaitSemaphores      = waitSemaphores;
    submitInfo.pWaitDstStageMask    = waitStages;
    submitInfo.commandBufferCount   = 1;
    submitInfo.pCommandBuffers      = commandBuffers;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores    = signalSemaphores;

    inFlightFence.Reset();

    VK_CHECK(vkQueueSubmit(m_Device->GraphicsQueue(), 1, &submitInfo, inFlightFence.Handle()));

    const VkSwapchainKHR swapChains[] = {m_SwapChain->Handle()};
    VkPresentInfoKHR     presentInfo  = {};
    presentInfo.sType                 = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount    = 1;
    presentInfo.pWaitSemaphores       = signalSemaphores;
    presentInfo.swapchainCount        = 1;
    presentInfo.pSwapchains           = swapChains;
    presentInfo.pImageIndices         = &imageIndex;
    presentInfo.pResults              = nullptr;    // Optional

    result = vkQueuePresentKHR(m_Device->PresentQueue(), &presentInfo);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
    {
        RecreateSwapChain();
        return;
    }

    if(result != VK_SUCCESS)
    {
        VK_THROW(std::string("failed to present next image ("), Strings::Result(result), ")");
    }

    m_CurrentFrame = (m_CurrentFrame + 1) % m_InFlightFences.size();
}

void Application::Render(const VkCommandBuffer commandBuffer, const uint32_t imageIndex)
{
    std::array<VkClearValue, 2> clearValues = {};
    clearValues[0].color                    = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil             = {1.0f, 0};

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass            = m_GraphicsPipeline->RenderPass().Handle();
    renderPassInfo.framebuffer           = m_SwapChainFramebuffers[imageIndex].Handle();
    renderPassInfo.renderArea.offset     = {0, 0};
    renderPassInfo.renderArea.extent     = m_SwapChain->Extent();
    renderPassInfo.clearValueCount       = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues          = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    {
        const auto& scene = GetScene();

        const VkDescriptorSet  descriptorSets[] = {m_GraphicsPipeline->DescriptorSet(imageIndex)};
        const VkBuffer         vertexBuffers[]  = {scene.VertexBuffer().Handle()};
        const VkBuffer         indexBuffer      = scene.IndexBuffer().Handle();
        constexpr VkDeviceSize offsets[]        = {0};

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->Handle());
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_GraphicsPipeline->PipelineLayout().Handle(), 0, 1, descriptorSets, 0, nullptr);
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);

        uint32_t vertexOffset = 0;
        uint32_t indexOffset  = 0;

        for(const auto& model : scene.Models())
        {
            const auto vertexCount = model.NumberOfVertices();
            const auto indexCount  = model.NumberOfIndices();

            vkCmdDrawIndexed(commandBuffer, indexCount, 1, indexOffset, static_cast<int32_t>(vertexOffset), 0);

            vertexOffset += vertexCount;
            indexOffset += indexCount;
        }
    }
    vkCmdEndRenderPass(commandBuffer);
}

void Application::UpdateUniformBuffer(const uint32_t imageIndex) const
{
    m_UniformBuffers[imageIndex].SetValue(GetUniformBufferObject(m_SwapChain->Extent()));
}

void Application::RecreateSwapChain()
{
    m_Device->WaitIdle();
    DeleteSwapChain();
    CreateSwapChain();
}

}    // namespace Engine
