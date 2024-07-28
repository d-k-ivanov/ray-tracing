#pragma once

#include "ModelViewController.h"

#include <RayTracing/Application.h>
#include <Scenes/SceneList.h>
#include <UI/UserSettings.h>

namespace UI
{
class UserInterface;
}

class RayTracer final : public RayTracing::Application
{
public:
    RayTracer(UI::UserSettings userSettings, const Engine::WindowConfig& windowConfig, VkPresentModeKHR presentMode);
    ~RayTracer() override;

    RayTracer(const RayTracer&)            = delete;
    RayTracer(RayTracer&&)                 = delete;
    RayTracer& operator=(const RayTracer&) = delete;
    RayTracer& operator=(RayTracer&&)      = delete;

protected:
    const Scenes::Scene&        GetScene() const override { return *m_Scene; }
    Render::UniformBufferObject GetUniformBufferObject(VkExtent2D extent) const override;

    void SetPhysicalDevice(VkPhysicalDevice physicalDevice, std::vector<const char*>& requiredExtensions, VkPhysicalDeviceFeatures& deviceFeatures, void* nextDeviceFeatures) override;

    void OnDeviceSet() override;
    void CreateSwapChain() override;
    void DeleteSwapChain() override;
    void DrawFrame() override;
    void Render(VkCommandBuffer commandBuffer, uint32_t imageIndex) override;

    void OnKey(int key, int scancode, int action, int mods) override;
    void OnCursorPosition(double xpos, double ypos) override;
    void OnMouseButton(int button, int action, int mods) override;
    void OnScroll(double xoffset, double yoffset) override;

private:
    void LoadScene(uint32_t sceneIndex);
    void CheckAndUpdateBenchmarkState(double prevTime);
    void CheckFramebufferSize() const;

    uint32_t                             m_SceneIndex{};
    UI::UserSettings                     m_UserSettings{};
    UI::UserSettings                     m_PreviousSettings{};
    Scenes::SceneList::CameraInitialSate m_CameraInitialSate{};
    ModelViewController                  m_ModelViewController{};

    std::unique_ptr<const Scenes::Scene> m_Scene;
    std::unique_ptr<UI::UserInterface>   m_UserInterface;

    double m_Time{};

    uint32_t m_TotalNumberOfSamples{};
    uint32_t m_NumberOfSamples{};
    bool     m_ResetAccumulation{};

    // Benchmark stats
    double   m_SceneInitialTime{};
    double   m_PeriodInitialTime{};
    uint32_t m_PeriodTotalFrames{};
};
