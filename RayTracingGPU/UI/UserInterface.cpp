#include "UserInterface.h"

#include "Engine/GraphicsPipeline.h"
#include "ImGuiHelper.h"
#include "UserSettings.h"

#include <Engine/DescriptorPool.h>
#include <Engine/Device.h>
#include <Engine/FrameBuffer.h>
#include <Engine/Instance.h>
#include <Engine/RenderPass.h>
#include <Engine/SingleTimeCommands.h>
#include <Engine/Strings.h>
#include <Engine/Surface.h>
#include <Engine/SwapChain.h>
#include <Engine/Window.h>
#include <Scenes/SceneList.h>
#include <Utils/Filesystem.h>
#include <Utils/VulkanException.h>

#include <imgui.h>
#include <imgui_freetype.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include <array>
#include <memory>

namespace
{
void CheckVulkanResultCallback(const VkResult err)
{
    if(err != VK_SUCCESS)
    {
        VK_THROW(std::string("ImGui Vulkan error ("), Engine::Strings::Result(err), ")");
    }
}
}

namespace UI
{

UserInterface::UserInterface(Engine::CommandPool& commandPool, const Engine::SwapChain& swapChain, const Engine::DepthBuffer& depthBuffer, UserSettings& userSettings)
    : m_UserSettings(userSettings)
{
    const auto& device = swapChain.Device();
    const auto& window = device.Surface().Instance().Window();

    // Initialise descriptor pool and render pass for ImGui.
    const std::vector<Engine::DescriptorBinding> descriptorBindings =
        {
            {0, 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 0},
        };
    m_DescriptorPool.reset(new Engine::DescriptorPool(device, descriptorBindings, 1));
    m_RenderPass.reset(new Engine::RenderPass(swapChain, depthBuffer, VK_ATTACHMENT_LOAD_OP_LOAD, VK_ATTACHMENT_LOAD_OP_LOAD));

    // Initialise ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    auto& io = ImGui::GetIO();
    // io.IniFilename = nullptr;    // No ini file.

    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;    // Enable Keyboard Controls
    // io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;     // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;      // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;    // Enable Multi-Viewport / Platform Windows
    // io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange; // Disable mouse cursos changing
    // io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleFonts;    // DPI awareness
    // io.ConfigFlags |= ImGuiConfigFlags_DpiEnableScaleViewports;
    // io.ConfigViewportsNoAutoMerge = true;
    // io.ConfigViewportsNoTaskBarIcon = true;

    // Window scaling and style.
    // const auto scaleFactor = window.ContentScale();

    // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    // ImGui::StyleColorsClassic();

    ImGuiStyle& style = ImGui::GetStyle();
    // style.ScaleAllSizes(scaleFactor);
    style.WindowRounding              = 0.0f;
    style.WindowBorderSize            = 0.0f;
    style.FrameBorderSize             = 1.0f;
    style.FrameRounding               = 5.0f;
    style.WindowMenuButtonPosition    = ImGuiDir_None;
    style.Colors[ImGuiCol_WindowBg].w = 1.0f;

    // Initialise ImGui GLFW adapter
    if(!ImGui_ImplGlfw_InitForVulkan(window.Handle(), true))
    {
        VK_THROW("failed to initialise ImGui GLFW adapter");
    }

    // Initialise ImGui Vulkan adapter
    ImGui_ImplVulkan_InitInfo vulkanInit = {};
    vulkanInit.Instance                  = device.Surface().Instance().Handle();
    vulkanInit.PhysicalDevice            = device.PhysicalDevice();
    vulkanInit.Device                    = device.Handle();
    vulkanInit.QueueFamily               = device.GraphicsFamilyIndex();
    vulkanInit.Queue                     = device.GraphicsQueue();
    vulkanInit.PipelineCache             = nullptr;
    vulkanInit.DescriptorPool            = m_DescriptorPool->Handle();
    vulkanInit.RenderPass                = m_RenderPass->Handle();
    vulkanInit.MinImageCount             = swapChain.MinImageCount();
    vulkanInit.ImageCount                = static_cast<uint32_t>(swapChain.Images().size());
    vulkanInit.Allocator                 = nullptr;
    vulkanInit.CheckVkResultFn           = CheckVulkanResultCallback;

    if(!ImGui_ImplVulkan_Init(&vulkanInit))
    {
        VK_THROW("failed to initialise ImGui vulkan adapter");
    }

    ImFontConfig fontConfig;
    fontConfig.FontDataOwnedByAtlas = false;
    // Upload ImGui fonts (use ImGuiFreeType for better font rendering, see https://github.com/ocornut/imgui/tree/master/misc/freetype).
    io.Fonts->FontBuilderIO = ImGuiFreeType::GetBuilderForFreeType();

    const auto robotoFontPath = Utils::ThisExecutableLocation() + "/Resources/Fonts/Roboto-Regular.ttf";

    io.Fonts->AddFontFromFileTTF(robotoFontPath.c_str(), 10.0f);
    io.Fonts->AddFontFromFileTTF(robotoFontPath.c_str(), 11.0f);
    io.Fonts->AddFontFromFileTTF(robotoFontPath.c_str(), 12.0f);
    ImFont* robotoFont = io.Fonts->AddFontFromFileTTF(robotoFontPath.c_str(), 14.0f);
    io.Fonts->AddFontFromFileTTF(robotoFontPath.c_str(), 16.0f);
    io.Fonts->AddFontFromFileTTF(robotoFontPath.c_str(), 18.0f);
    io.Fonts->AddFontFromFileTTF(robotoFontPath.c_str(), 20.0f);

    if(!robotoFont)
    {
        VK_THROW("Failed to load ImGui font: ", robotoFontPath);
    }
    io.FontDefault = robotoFont;

    // Upload fonts to the GPU
    Engine::SingleTimeCommands::Submit(commandPool, [](VkCommandBuffer commandBuffer)
    {
        if(!ImGui_ImplVulkan_CreateFontsTexture())
        {
            VK_THROW("Failed to create ImGui font textures");
        }
    });
}

UserInterface::~UserInterface()
{
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void UserInterface::Render(const VkCommandBuffer commandBuffer, const Engine::FrameBuffer& frameBuffer, const Statistics& statistics) const
{
    const ImGuiIO& io = ImGui::GetIO();

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplVulkan_NewFrame();
    ImGui::NewFrame();

    {
        // static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_None;
        static ImGuiDockNodeFlags dockspaceFlags = ImGuiDockNodeFlags_PassthruCentralNode;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking;
        if(m_MenubarCallback != nullptr)
        {
            windowFlags |= ImGuiWindowFlags_MenuBar;
        }

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->WorkPos);
        ImGui::SetNextWindowSize(viewport->WorkSize);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
        windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if(dockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
        {
            windowFlags |= ImGuiWindowFlags_NoBackground;
        }

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace", nullptr, windowFlags);
        ImGui::PopStyleVar();

        ImGui::PopStyleVar(2);

        // Submit the DockSpace
        if(io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            const ImGuiID dockspaceId = ImGui::GetID("ApplicationDockspace");
            ImGui::DockSpace(dockspaceId, ImVec2(0.0f, 0.0f), dockspaceFlags);
        }

        if(m_MenubarCallback != nullptr)
        {
            if(ImGui::BeginMenuBar())
            {
                m_MenubarCallback();
                ImGui::EndMenuBar();
            }
        }

        DrawSettings(statistics);
        // DrawViewport(graphicsPipeline);
        // DrawStats(statistics);
        // ImGui::ShowStyleEditor();

        ImGui::End();
    }
    ImGui::Render();

    // Update and Render additional Platform Windows
    if(io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType                 = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass            = m_RenderPass->Handle();
    renderPassInfo.framebuffer           = frameBuffer.Handle();
    renderPassInfo.renderArea.offset     = {0, 0};
    renderPassInfo.renderArea.extent     = m_RenderPass->SwapChain().Extent();
    renderPassInfo.clearValueCount       = 0;
    renderPassInfo.pClearValues          = nullptr;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), commandBuffer);
    vkCmdEndRenderPass(commandBuffer);
}

bool UserInterface::WantsToCaptureKeyboard() const
{
    return ImGui::GetIO().WantCaptureKeyboard;
}

bool UserInterface::WantsToCaptureMouse() const
{
    return ImGui::GetIO().WantCaptureMouse;
}

void UserInterface::DrawSettings(const Statistics& statistics) const
{
    if(!Settings().ShowSettings)
    {
        return;
    }

    constexpr float  distance = 20.0f;
    constexpr ImVec2 pos      = ImVec2(distance, distance);
    constexpr ImVec2 posPivot = ImVec2(0.0f, 0.0f);
    // ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
    // ImGui::SetNextWindowPos(pos, ImGuiCond_Once, posPivot);
    ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver, posPivot);

    //  | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse
    // constexpr auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings;
    constexpr auto flags = ImGuiWindowFlags_AlwaysAutoResize;

    if(ImGui::Begin("Settings", &Settings().ShowSettings, flags))
    {
        const auto& window = m_DescriptorPool->Device().Surface().Instance().Window();

        ImGui::Text("Statistics (%dx%d):", statistics.FramebufferSize.width, statistics.FramebufferSize.height);
        ImGui::Separator();
        ImGui::Text("Frame rate: %.1f fps", statistics.FrameRate);
        ImGui::Text("Primary ray rate: %.2f Gr/s", statistics.RayRate);
        ImGui::Text("Accumulated samples:  %u", statistics.TotalSamples);
        ImGui::Text("Camera position:  %.1f %.1f %.1f", statistics.CameraLocation.x, statistics.CameraLocation.y, statistics.CameraLocation.z);
        ImGui::NewLine();

        ImGui::Text("Help");
        ImGui::Separator();
        ImGui::BulletText("F1: toggle Settings.");
        ImGui::BulletText("F2: toggle Statistics.");
        ImGui::BulletText(
            "%c%c%c%c/SHIFT/CTRL: move camera.",
            std::toupper(window.GetKeyName(GLFW_KEY_W, 0)[0]),
            std::toupper(window.GetKeyName(GLFW_KEY_A, 0)[0]),
            std::toupper(window.GetKeyName(GLFW_KEY_S, 0)[0]),
            std::toupper(window.GetKeyName(GLFW_KEY_D, 0)[0]));
        ImGui::BulletText("L/R Mouse: rotate camera/scene.");
        ImGui::NewLine();

        ImGui::SeparatorText("Style");
        ImGuiSettings::ShowStyleSelector("Colors");
        ImGuiSettings::ShowFontSelector("Font");

        std::vector<const char*> scenes;
        scenes.reserve(Scenes::SceneList::AllScenes.size());
        for(const auto& scene : Scenes::SceneList::AllScenes)
        {
            scenes.push_back(scene.first.c_str());
        }

        ImGui::Text("Scene");
        ImGui::Separator();
        ImGui::PushItemWidth(-1);
        ImGui::Combo("##SceneList", &Settings().SceneIndex, scenes.data(), static_cast<int>(scenes.size()));
        ImGui::PopItemWidth();
        ImGui::NewLine();

        ImGui::Text("Ray Tracing");
        ImGui::Separator();
        ImGui::Checkbox("Enable ray tracing", &Settings().IsRayTraced);
        ImGui::Checkbox("Accumulate rays between frames", &Settings().AccumulateRays);

        uint32_t min = 1, max = 1024;
        ImGui::SliderScalar("Samples", ImGuiDataType_U32, &Settings().NumberOfSamples, &min, &max);
        min = 1, max = 512;
        ImGui::SliderScalar("Depth", ImGuiDataType_U32, &Settings().NumberOfBounces, &min, &max);
        ImGui::NewLine();

        ImGui::Text("Camera");
        ImGui::Separator();
        ImGui::SliderFloat("FoV", &Settings().FieldOfView, UserSettings::FieldOfViewMinValue, UserSettings::FieldOfViewMaxValue, "%.0f");
        ImGui::SliderFloat("Aperture", &Settings().Aperture, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Focus", &Settings().FocusDistance, 0.1f, 20.0f, "%.1f");
        ImGui::NewLine();

        ImGui::Text("Profiler");
        ImGui::Separator();
        ImGui::Checkbox("Show heatmap", &Settings().ShowHeatmap);
        ImGui::SliderFloat("Scaling", &Settings().HeatmapScale, 0.10f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
        ImGui::NewLine();
    }
    ImGui::End();
}

// TBA
void UserInterface::DrawViewport(const Engine::GraphicsPipeline& graphicsPipeline) const
{
    if(!Settings().ShowViewport)
    {
        return;
    }

    const auto& io = ImGui::GetIO();
    // constexpr auto pos = ImVec2(0.0f, 0.0f);
    // ImGui::SetNextWindowPos(pos, ImGuiCond_Always, ImVec2(0.0f, 0.0f));
    // ImGui::SetNextWindowSize(io.DisplaySize, ImGuiCond_Always);
    // ImGui::SetNextWindowBgAlpha(0.0f);    // Transparent background

    //  | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav;
    // constexpr auto flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav;
    constexpr auto flags = ImGuiWindowFlags_NoCollapse;

    // m_ViewportWidth  = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
    // m_ViewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

    // m_ViewportWidth = static_cast<uint32_t>(m_Width);
    // m_ViewportWidth  = static_cast<int>(ImGui::GetContentRegionAvail().x);
    // m_ViewportHeight = static_cast<int>(m_ViewportWidth / m_AspectRatio);
    // m_ViewportHeight = (m_ViewportHeight < 1) ? 1 : m_ViewportHeight;

    if(ImGui::Begin("Viewport", &Settings().ShowViewport, flags))
    {
        const ImVec2 cursorPosition = ImVec2((ImGui::GetWindowSize().x - ImGui::GetContentRegionAvail().x) * 0.5f, (ImGui::GetWindowSize().y - ImGui::GetContentRegionAvail().y) * 0.5f);
        ImGui::SetCursorPos(cursorPosition);
        // ImGui::Image(, {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y});
        // ImGui::Image(reinterpret_cast<ImTextureID>(m_RenderPass->SwapChain().Images()), {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y}, {0, 1}, {1, 0});
        // ImGui::Image(reinterpret_cast<ImTextureID>(), {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y}, {0, 1}, {1, 0});
        // ImGui::Image(, {ImGui::GetContentRegionAvail().x, ImGui::GetContentRegionAvail().y}, {0, 1}, {1, 0});
    }
    ImGui::End();
}

void UserInterface::DrawStats(const Statistics& statistics) const
{
    if(!Settings().ShowStats)
    {
        return;
    }

    const auto&      io       = ImGui::GetIO();
    constexpr float  distance = 50.0f;
    const ImVec2     pos      = ImVec2(io.DisplaySize.x - distance, distance);
    constexpr ImVec2 posPivot = ImVec2(1.0f, 0.0f);
    // ImGui::SetNextWindowPos(pos, ImGuiCond_Always, posPivot);
    // ImGui::SetNextWindowPos(pos, ImGuiCond_Once, posPivot);
    ImGui::SetNextWindowPos(pos, ImGuiCond_FirstUseEver, posPivot);
    // ImGui::SetNextWindowBgAlpha(0.3f);    // Transparent background

    //  | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoNav;
    // constexpr auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings;
    constexpr auto flags = ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing;

    if(ImGui::Begin("Statistics", &Settings().ShowStats, flags))
    {
        ImGui::Text("Statistics (%dx%d):", statistics.FramebufferSize.width, statistics.FramebufferSize.height);
        ImGui::Separator();
        ImGui::Text("Frame rate: %.1f fps", statistics.FrameRate);
        ImGui::Text("Primary ray rate: %.2f Gr/s", statistics.RayRate);
        ImGui::Text("Accumulated samples:  %u", statistics.TotalSamples);
        ImGui::Text("Camera position:  %.1f %.1f %.1f", statistics.CameraLocation.x, statistics.CameraLocation.y, statistics.CameraLocation.z);
    }
    ImGui::End();
}

}    // namespace UI
