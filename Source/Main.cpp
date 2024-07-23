
#include <Render/Renderer.h>
#include <Scenes/Scene.h>
#include <UI/ImGuiHelper.h>
#include <UI/Layer.h>
#include <Utils/Log.h>
#include <Utils/Timers.h>
#include <Vulkan/Application.h>
#include <Vulkan/Image.h>

#include <imgui.h>

bool g_ApplicationRunning = true;

class RayTracinUILayer final : public UI::Layer
{
public:
    void OnAttach() override
    {
        SetScene();
    }

    void OnUIRender() override
    {
        ImGui::Begin("Settings");
        ImGui::SeparatorText("Style");
        UI::ImGuiSettings::ShowStyleSelector("Colors");
        UI::ImGuiSettings::ShowFontSelector("Font");

        ImGui::SeparatorText("Rendering");
        const char* sceneList[] = {
            "RTWeekOne: Default",
            "RTWeekOne: Test",
            "RTWeekOne: Final",
            "RTWeekNext: Default",
            "RTWeekNext: Random Spheres",
            "RTWeekNext: Two Spheres",
            "RTWeekNext: Earth",
            "RTWeekNext: Two Perlin Spheres",
            "RTWeekNext: Quads",
            "RTWeekNext: Simple Light",
            "RTWeekNext: Cornell Box",
            "RTWeekNext: Cornell Smoke",
            "RTWeekNext: Final",
            "RTWeekRest: Cornell Box (Simple)",
            "RTWeekRest: Cornell Box (Mirror)",
            "RTWeekRest: Cornell Box (Glass)",
            "White Speres",
            "Cornell Box Lights"};
        if(ImGui::Combo("Scene", &m_SceneId, sceneList, IM_ARRAYSIZE(sceneList)))
        {
            m_Renderer.ResetFrameCounter();
            SetScene();
        }

        if(ImGui::InputInt("Samples ", &m_SceneSamples))
        {
            SetScene();
        }
        ImGui::InputInt("Depth", &m_SceneDepth);

        // ImGui::Text("Aspect Ratio: %.6f", m_AspectRatio);
        ImGui::InputDouble("Aspect Ratio", &m_AspectRatio, 0.0, 0.0, "%.2f");
        ImGui::InputInt("Width", &m_Width, 0, 1920);
        // ImGui::Text("Viewport Height: %d", m_ViewportHeight);
        // ImGui::Text("Image Width: %.2f", m_ImageWidth);
        // ImGui::Text("Image Height: %.2f", m_ImageHeight);

        const char* renderersList[] = {"Ray Tracer", "Random INT", "Hello World"};
        if(ImGui::Combo("Renderer", &m_RendererId, renderersList, IM_ARRAYSIZE(renderersList)))
        {
            SetScene();
        }

        ImGui::Text("Renderer:");
        ImGui::RadioButton("CPU Single Core", &m_RendererType, 0);
        ImGui::SameLine();
        ImGui::RadioButton("CPU Multi Core", &m_RendererType, 1);
        ImGui::SameLine();
        ImGui::RadioButton("GPU", &m_RendererType, 2);

        ImGui::Text("Pixel Sampling Type:");
        ImGui::RadioButton("Normal", &m_SamplingType, 0);
        ImGui::SameLine();
        ImGui::RadioButton("Accumulation", &m_SamplingType, 1);
        ImGui::SameLine();
        ImGui::RadioButton("Stratified", &m_SamplingType, 2);

        ImGui::Checkbox("Use Probability Density Functions (PDF)", &m_UsePDF);
        ImGui::Checkbox("Use Unidirectional Light", &m_UseUnidirectionalLight);

        if(static_cast<Render::Camera::SamplerType>(m_SamplingType) == Render::Camera::SamplerType::Accumulation)
        {
            m_SceneSamples = static_cast<int>(m_Renderer.GetFrameCounter());
            if(ImGui::Button("Render", ImVec2(ImGui::GetWindowSize().x * 0.45f, 0.0f)))
            {
                Render();
            }
            ImGui::SameLine();
            if(ImGui::Button("Reset", ImVec2(ImGui::GetWindowSize().x * 0.45f, 0.0f)))
            {
                m_Renderer.ResetFrameCounter();
                Render();
            }
        }
        else
        {
            m_Renderer.ResetFrameCounter();
            if(ImGui::Button("Render", ImVec2(-FLT_MIN, 0.0f)))
            {
                Render();
            }
        }

        static bool loopRendering = false;
        ImGui::Checkbox("Loop Rendering", &loopRendering);

        const int miliseconds = static_cast<int>(m_LastRenderTime) % 1000;
        const int seconds     = static_cast<int>(m_LastRenderTime) / 1000 % 60;
        const int minutes     = static_cast<int>(m_LastRenderTime) / 1000 / 60 % 60;
        const int hours       = static_cast<int>(m_LastRenderTime) / 1000 / 60 / 60;

        ImGui::Text("Rendering time: %.3f ms", m_LastRenderTime);
        ImGui::Text("Rendering time (human): ");
        ImGui::SameLine();
        if(hours >= 1)
        {
            ImGui::Text("%dh ", hours);
            ImGui::SameLine();
        }
        if(minutes >= 1)
        {
            ImGui::Text("%dm ", minutes);
            ImGui::SameLine();
        }
        if(seconds >= 1)
        {
            ImGui::Text("%ds ", seconds);
            ImGui::SameLine();
        }
        ImGui::Text("%dms", miliseconds);

        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 225));
        // ImGui::SetNextWindowSizeConstraints(ImVec2(400, 225), ImVec2(400, 225));
        ImGui::Begin("Viewport");

        // m_ViewportWidth  = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
        // m_ViewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

        m_ViewportWidth = static_cast<uint32_t>(m_Width);
        // m_ViewportWidth  = static_cast<int>(ImGui::GetContentRegionAvail().x);
        m_ViewportHeight = static_cast<int>(m_ViewportWidth / m_AspectRatio);
        m_ViewportHeight = (m_ViewportHeight < 1) ? 1 : m_ViewportHeight;

        if(const auto image = m_Renderer.GetImage())
        {
            m_ImageWidth  = static_cast<float>(image->GetWidth());
            m_ImageHeight = static_cast<float>(image->GetHeight());

            const ImVec2 cursorPosition = ImVec2((ImGui::GetWindowSize().x - m_ImageWidth) * 0.5f, (ImGui::GetWindowSize().y - m_ImageHeight) * 0.5f);
            ImGui::SetCursorPos(cursorPosition);
            ImGui::Image(image->GetDescriptorSet(), {m_ImageWidth, m_ImageHeight});
        }
        ImGui::End();
        ImGui::PopStyleVar();

        if(loopRendering)
        {
            Render();
        }
    }

    void Render()
    {
        const Utils::Timer timer;

        m_Renderer.SetImageSize(m_ViewportWidth, m_ViewportHeight);
        switch(m_RendererId)
        {
            case 0:
            {
                Render::Camera camera         = m_Scene->GetCamera();
                camera.RenderingType          = static_cast<Render::Camera::RenderType>(m_RendererType);
                camera.SamplingType           = static_cast<Render::Camera::SamplerType>(m_SamplingType);
                camera.UsePDF                 = m_UsePDF;
                camera.UseUnidirectionalLight = m_UseUnidirectionalLight;
                m_Renderer.Render(camera, m_Scene->GetWorld(), m_Scene->GetLights());
                break;
            }
            case 1:
            {
                m_Renderer.RenderRandom();
                break;
            }
            case 2:
            {

                m_Renderer.RenderHelloWorld();
                break;
            }
            default:
            {

                m_Renderer.RenderRandom();
                break;
            }
        }
        m_LastRenderTime = timer.ElapsedMilliseconds();
    }

    void SetScene()
    {
        switch(m_SceneId)
        {
            case 0:
                m_Scene = std::make_shared<Scenes::RTWeekOneDefaultScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 1:
                m_Scene = std::make_shared<Scenes::RTWeekOneTestScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 2:
                m_Scene = std::make_shared<Scenes::RTWeekOneFinalScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 3:
                m_Scene = std::make_shared<Scenes::RTWeekNextDefaultScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 4:
                m_Scene = std::make_shared<Scenes::RTWeekNextRandomSpheresScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 5:
                m_Scene = std::make_shared<Scenes::RTWeekNextTwoSpheresScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 6:
                m_Scene = std::make_shared<Scenes::RTWeekNextEarthScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 7:
                m_Scene = std::make_shared<Scenes::RTWeekNextTwoPerlinSpheresScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 8:
                m_Scene = std::make_shared<Scenes::RTWeekNextQuadsScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 9:
                m_Scene = std::make_shared<Scenes::RTWeekNextSimpleLightScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 10:
                m_Scene = std::make_shared<Scenes::RTWeekNextCornellBoxScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 11:
                m_Scene = std::make_shared<Scenes::RTWeekNextCornellSmokeScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 12:
                m_Scene = std::make_shared<Scenes::RTWeekNextFinalScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 13:
                m_Scene = std::make_shared<Scenes::RTWeekRestACornellBoxScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 14:
                m_Scene = std::make_shared<Scenes::RTWeekRestBCornellBoxMirrorScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 15:
                m_Scene = std::make_shared<Scenes::RTWeekRestCCornellBoxGlassScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 16:
                m_Scene = std::make_shared<Scenes::WhiteSperesScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 17:
            default:
                m_Scene = std::make_shared<Scenes::CornellBoxLightsScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
        }
    }

private:
    // Rendering
    Render::Renderer m_Renderer;
    int              m_RendererId             = 0;
    int              m_RendererType           = 1;
    int              m_SamplingType           = 2;
    double           m_LastRenderTime         = 0.0;
    bool             m_UsePDF                 = true;
    bool             m_UseUnidirectionalLight = true;

    // Scene
    std::shared_ptr<Scenes::Scene> m_Scene        = nullptr;
    int                            m_SceneId      = 0;
    int                            m_SceneDepth   = 50;
    int                            m_SceneSamples = 4;

    // Output
    // double   m_AspectRatio    = 16.0 / 9.0;
    double   m_AspectRatio    = 1.0;
    float    m_ImageHeight    = 0;
    float    m_ImageWidth     = 0;
    uint32_t m_ViewportHeight = 0;
    uint32_t m_ViewportWidth  = 600;
    int      m_Width          = 600;

    // ImGui Stuff
    ImVec2 m_InvertedX = {0, 1};
    ImVec2 m_InvertedY = {1, 0};
};

class ImGuiDemoLayer final : public UI::Layer
{
public:
    void OnUIRender() override
    {
        ImGui::ShowDemoWindow();
    }
};

Vulkan::Application* CreateApplication(int argc, char** argv)
{
    Vulkan::ApplicationSpecs spec;
    spec.Name = "Ray Tracing";

    Vulkan::Application* app = new Vulkan::Application(spec);
    app->PushLayer<RayTracinUILayer>();
    app->PushLayer<ImGuiDemoLayer>();
    app->SetMenubarCallback(
        [app]()
        {
            if(ImGui::BeginMenu("File"))
            {
                if(ImGui::MenuItem("Exit"))
                {
                    app->Close();
                }
                ImGui::EndMenu();
            }
        });
    LOG_INFO("Started...");
    LOG_INFO("Vulkan SDK Header Version: {}", VK_HEADER_VERSION);
    return app;
}

int main(const int argc, char** argv)
{
    Utils::Log::Init();
    LOG_INFO("Starting...");
    while(g_ApplicationRunning)
    {
        Vulkan::Application* app = CreateApplication(argc, argv);
        app->Run();
        delete app;
    }
    return 0;
}
