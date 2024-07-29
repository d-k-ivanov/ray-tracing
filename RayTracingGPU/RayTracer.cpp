#include "RayTracer.h"

#include <Engine/Device.h>
#include <Engine/SwapChain.h>
#include <Engine/Window.h>
#include <Objects/Model.h>
#include <Render/Texture.h>
#include <Render/UniformBuffer.h>
#include <Scenes/Scene.h>
#include <UI/UserInterface.h>
#include <UI/UserSettings.h>
#include <Utils/Filesystem.h>
#include <Utils/Glm.h>
#include <Utils/VulkanException.h>

#include <algorithm>
#include <sstream>

namespace
{
constexpr bool EnableValidationLayers =
#ifdef NDEBUG
    false;
#else
    true;
#endif
}

RayTracer::RayTracer(UI::UserSettings userSettings, const Engine::WindowConfig& windowConfig, const VkPresentModeKHR presentMode)
    : Application(windowConfig, presentMode, EnableValidationLayers)
    , m_UserSettings(std::move(userSettings))
{
    CheckFramebufferSize();
}

RayTracer::~RayTracer()
{
    m_Scene.reset();
}

Render::UniformBufferObject RayTracer::GetUniformBufferObject(const VkExtent2D extent) const
{
    const auto& init = m_CameraInitialSate;

    Render::UniformBufferObject ubo;

    ubo.ModelView  = m_ModelViewController.ModelView();
    ubo.Projection = glm::perspective(glm::radians(m_UserSettings.FieldOfView), static_cast<float>(extent.width) / static_cast<float>(extent.height), 0.1f, 10000.0f);
    ubo.Projection[1][1] *= -1;    // Inverting Y for Vulkan, https://matthewwellings.com/blog/the-new-vulkan-coordinate-system/
    ubo.ModelViewInverse     = glm::inverse(ubo.ModelView);
    ubo.ProjectionInverse    = glm::inverse(ubo.Projection);
    ubo.Aperture             = m_UserSettings.Aperture;
    ubo.FocusDistance        = m_UserSettings.FocusDistance;
    ubo.TotalNumberOfSamples = m_TotalNumberOfSamples;
    ubo.NumberOfSamples      = m_NumberOfSamples;
    ubo.NumberOfBounces      = m_UserSettings.NumberOfBounces;
    ubo.RandomSeed           = 1;
    ubo.HasSky               = init.HasSky;
    ubo.ShowHeatmap          = m_UserSettings.ShowHeatmap;
    ubo.HeatmapScale         = m_UserSettings.HeatmapScale;

    return ubo;
}

void RayTracer::SetPhysicalDevice(
    const VkPhysicalDevice    physicalDevice,
    std::vector<const char*>& requiredExtensions,
    VkPhysicalDeviceFeatures& deviceFeatures,
    void*                     nextDeviceFeatures)
{
    // Required extensions.
    requiredExtensions.insert(requiredExtensions.end(),
                              {// VK_KHR_SHADER_CLOCK is required for heatmap
                               VK_KHR_SHADER_CLOCK_EXTENSION_NAME});

    // Opt-in into mandatory device features.
    VkPhysicalDeviceShaderClockFeaturesKHR shaderClockFeatures = {};
    shaderClockFeatures.sType                                  = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SHADER_CLOCK_FEATURES_KHR;
    shaderClockFeatures.pNext                                  = nextDeviceFeatures;
    shaderClockFeatures.shaderSubgroupClock                    = true;

    deviceFeatures.fillModeNonSolid  = true;
    deviceFeatures.samplerAnisotropy = true;
    deviceFeatures.shaderInt64       = true;

    Application::SetPhysicalDevice(physicalDevice, requiredExtensions, deviceFeatures, &shaderClockFeatures);
}

void RayTracer::OnDeviceSet()
{
    Application::OnDeviceSet();

    LoadScene(m_UserSettings.SceneIndex);
    CreateAccelerationStructures();
}

void RayTracer::CreateSwapChain()
{
    Application::CreateSwapChain();

    m_UserInterface.reset(new UI::UserInterface(CommandPool(), SwapChain(), DepthBuffer(), m_UserSettings));
    m_ResetAccumulation = true;

    CheckFramebufferSize();
}

void RayTracer::DeleteSwapChain()
{
    m_UserInterface.reset();

    Application::DeleteSwapChain();
}

void RayTracer::DrawFrame()
{
    // Check if the scene has been changed by the user.
    if(m_SceneIndex != static_cast<uint32_t>(m_UserSettings.SceneIndex))
    {
        Device().WaitIdle();
        DeleteSwapChain();
        DeleteAccelerationStructures();
        LoadScene(m_UserSettings.SceneIndex);
        CreateAccelerationStructures();
        CreateSwapChain();
        return;
    }

    // Check if the accumulation buffer needs to be reset.
    if(m_ResetAccumulation || m_UserSettings.RequiresAccumulationReset(m_PreviousSettings) || !m_UserSettings.AccumulateRays)
    {
        m_TotalNumberOfSamples = 0;
        m_ResetAccumulation    = false;
    }

    m_PreviousSettings = m_UserSettings;

    // Keep track of our sample count.
    m_NumberOfSamples = glm::clamp(m_UserSettings.MaxNumberOfSamples - m_TotalNumberOfSamples, 0u, m_UserSettings.NumberOfSamples);
    m_TotalNumberOfSamples += m_NumberOfSamples;

    Application::DrawFrame();
}

void RayTracer::Render(const VkCommandBuffer commandBuffer, const uint32_t imageIndex)
{
    // Record delta time between calls to Render.
    const auto prevTime  = m_Time;
    m_Time               = Window().GetTime();
    const auto timeDelta = m_Time - prevTime;

    // Update the camera position / angle.
    m_ResetAccumulation = m_ModelViewController.UpdateCamera(m_CameraInitialSate.ControlSpeed, timeDelta);

    // Check the current state of the benchmark, update it for the new frame.
    CheckAndUpdateBenchmarkState(prevTime);

    // Render the scene
    m_UserSettings.IsRayTraced
        ? RayTracing::Application::Render(commandBuffer, imageIndex)
        : Engine::Application::Render(commandBuffer, imageIndex);

    // Render the UI
    UI::Statistics stats  = {};
    stats.FramebufferSize = Window().FramebufferSize();
    stats.FrameRate       = static_cast<float>(1 / timeDelta);

    if(m_UserSettings.IsRayTraced)
    {
        const auto extent = SwapChain().Extent();

        stats.RayRate = static_cast<float>(static_cast<double>(extent.width * extent.height) * m_NumberOfSamples / (timeDelta * 1000000000));
        stats.TotalSamples = m_TotalNumberOfSamples;
    }
    stats.CameraLocation = m_ModelViewController.ModelView()[3];

    m_UserInterface->Render(commandBuffer, SwapChainFrameBuffer(imageIndex), stats);
}

void RayTracer::OnKey(const int key, const int scancode, const int action, const int mods)
{
    if(m_UserInterface->WantsToCaptureKeyboard())
    {
        return;
    }

    if(action == GLFW_PRESS)
    {
        switch(key)
        {
            case GLFW_KEY_ESCAPE:
                Window().Close();
                break;
            default:
                break;
        }

        // Settings (toggle switches)
        if(!m_UserSettings.Benchmark)
        {
            switch(key)
            {
                case GLFW_KEY_F1:
                    m_UserSettings.ShowSettings = !m_UserSettings.ShowSettings;
                    break;
                case GLFW_KEY_F2:
                    m_UserSettings.ShowStats = !m_UserSettings.ShowStats;
                    break;
                case GLFW_KEY_R:
                    m_UserSettings.IsRayTraced = !m_UserSettings.IsRayTraced;
                    break;
                case GLFW_KEY_H:
                    m_UserSettings.ShowHeatmap = !m_UserSettings.ShowHeatmap;
                    break;
                case GLFW_KEY_P:
                    m_IsWireFrame = !m_IsWireFrame;
                    break;
                default:
                    break;
            }
        }
    }

    // Camera motions
    if(!m_UserSettings.Benchmark)
    {
        m_ResetAccumulation |= m_ModelViewController.OnKey(key, scancode, action, mods);
    }
}

void RayTracer::OnCursorPosition(const double xpos, const double ypos)
{
    if(!HasSwapChain() || m_UserSettings.Benchmark || m_UserInterface->WantsToCaptureKeyboard() || m_UserInterface->WantsToCaptureMouse())
    {
        return;
    }

    // Camera motions
    m_ResetAccumulation |= m_ModelViewController.OnCursorPosition(xpos, ypos);
}

void RayTracer::OnMouseButton(const int button, const int action, const int mods)
{
    if(!HasSwapChain() || m_UserSettings.Benchmark || m_UserInterface->WantsToCaptureMouse())
    {
        return;
    }

    // Camera motions
    m_ResetAccumulation |= m_ModelViewController.OnMouseButton(button, action, mods);
}

void RayTracer::OnScroll(const double xoffset, const double yoffset)
{
    if(!HasSwapChain() || m_UserSettings.Benchmark || m_UserInterface->WantsToCaptureMouse())
    {
        return;
    }

    const auto prevFov         = m_UserSettings.FieldOfView;
    m_UserSettings.FieldOfView = std::clamp(
        static_cast<float>(prevFov - yoffset),
        UI::UserSettings::FieldOfViewMinValue,
        UI::UserSettings::FieldOfViewMaxValue);

    m_ResetAccumulation = prevFov != m_UserSettings.FieldOfView;
}

void RayTracer::LoadScene(const uint32_t sceneIndex)
{
    auto [models, textures] = Scenes::SceneList::AllScenes[sceneIndex].second(m_CameraInitialSate);

    // If there are no texture, add a dummy one. It makes the pipeline setup a lot easier.
    if(textures.empty())
    {
        const auto texturePath = Utils::ThisExecutableLocation() + "/Resources/Textures/white.png";
        textures.push_back(Render::Texture::LoadTexture(texturePath, Engine::SamplerConfig()));
    }

    m_Scene.reset(new Scenes::Scene(CommandPool(), std::move(models), std::move(textures)));
    m_SceneIndex = sceneIndex;

    m_UserSettings.FieldOfView   = m_CameraInitialSate.FieldOfView;
    m_UserSettings.Aperture      = m_CameraInitialSate.Aperture;
    m_UserSettings.FocusDistance = m_CameraInitialSate.FocusDistance;

    m_ModelViewController.Reset(m_CameraInitialSate.ModelView);

    m_PeriodTotalFrames = 0;
    m_ResetAccumulation = true;
}

void RayTracer::CheckAndUpdateBenchmarkState(const double prevTime)
{
    if(!m_UserSettings.Benchmark)
    {
        return;
    }

    // Initialise scene benchmark timers
    if(m_PeriodTotalFrames == 0)
    {
        LOG_DEBUG("Benchmark: Start scene #{} '{}'", m_SceneIndex, Scenes::SceneList::AllScenes[m_SceneIndex].first);

        m_SceneInitialTime  = m_Time;
        m_PeriodInitialTime = m_Time;
    }

    // Print out the frame rate at regular intervals.
    {
        constexpr double period        = 5;
        const double     prevTotalTime = prevTime - m_PeriodInitialTime;
        const double     totalTime     = m_Time - m_PeriodInitialTime;

        if(m_PeriodTotalFrames != 0 && static_cast<uint64_t>(prevTotalTime / period) != static_cast<uint64_t>(totalTime / period))
        {
            LOG_DEBUG("Benchmark: {} frames in {:.2f} seconds ({:.0f} fps)", m_PeriodTotalFrames, totalTime, m_PeriodTotalFrames / totalTime);
            m_PeriodInitialTime = m_Time;
            m_PeriodTotalFrames = 0;
        }

        m_PeriodTotalFrames++;
    }

    // If in benchmark mode, bail out from the scene if we've reached the time or sample limit.
    {
        const bool timeLimitReached   = m_PeriodTotalFrames != 0 && Window().GetTime() - m_SceneInitialTime > m_UserSettings.BenchmarkMaxTime;
        const bool sampleLimitReached = m_NumberOfSamples == 0;

        if(timeLimitReached || sampleLimitReached)
        {
            if(!m_UserSettings.BenchmarkNextScenes || static_cast<size_t>(m_UserSettings.SceneIndex) == Scenes::SceneList::AllScenes.size() - 1)
            {
                Window().Close();
            }

            m_UserSettings.SceneIndex += 1;
        }
    }
}

void RayTracer::CheckFramebufferSize() const
{
    // Check the framebuffer size when requesting a fullscreen window, as it's not guaranteed to match.
    const auto& cfg    = Window().Config();
    const auto  fbSize = Window().FramebufferSize();

    if(m_UserSettings.Benchmark && cfg.Fullscreen && (fbSize.width != cfg.Width || fbSize.height != cfg.Height))
    {
        std::ostringstream out;
        out << "framebuffer fullscreen size mismatch (requested: ";
        out << cfg.Width << "x" << cfg.Height;
        out << ", got: ";
        out << fbSize.width << "x" << fbSize.height << ")";

        VK_THROW(out.str());
    }
}
