
#include <Render/Renderer.h>
#include <Scenes/Scene.h>
#include <UI/Application.h>
#include <UI/ImGuiHelper.h>
#include <UI/Image.h>
#include <UI/Layer.h>
#include <Utils/Log.h>
#include <Utils/Timers.h>

#include <imgui.h>

bool g_ApplicationRunning = true;

class RayTracinUILayer final : public Layer
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
        ImGuiSettings::ShowStyleSelector("Colors");
        ImGuiSettings::ShowFontSelector("Font");

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
            "Cornell Box Lights"
        };
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

        const char* rendererList[] = {
            "Random INT",
            "Hello World",
            "CPU: One Core",
            "CPU: One Core Accumulating",
            "CPU: One Core Stratified",
            "CPU: Multi Core",
            "CPU: Multi Core Accumulating",
            "CPU: Multi Core Stratified",
            "CPU: Multi Core Stratified (WIP)"};
        if(ImGui::ListBox("Renderer", &m_RendererId, rendererList, IM_ARRAYSIZE(rendererList)))
        {
            SetScene();
        }

        ImGui::Checkbox("Use PDF", &m_UsePDF);
        ImGui::Checkbox("Use Unidirectional Light", &m_UseUnidirectionalLight);

        // TBA
        // if(m_Renderer.IsAccumulating())
        if(m_RendererId == 3 || m_RendererId == 6 || m_RendererId == 8)
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
            }
        }
        else
        {
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
        const Timer timer;

        m_Renderer.SetImageSize(m_ViewportWidth, m_ViewportHeight);
        switch(m_RendererId)
        {
            case 0:
            {
                m_Renderer.RenderRandom();
                break;
            }
            case 1:
            {

                m_Renderer.RenderHelloWorld();
                break;
            }
            case 2:
            {
                SetScene();
                Camera camera                 = m_Scene->GetCamera();
                camera.UsePDF                 = m_UsePDF;
                camera.UseUnidirectionalLight = m_UseUnidirectionalLight;
                m_Renderer.CPUOneCore(camera, m_Scene->GetWorld(), m_Scene->GetLights());
                break;
            }
            case 3:
            {
                Camera camera                 = m_Scene->GetCamera();
                camera.UsePDF                 = m_UsePDF;
                camera.UseUnidirectionalLight = m_UseUnidirectionalLight;
                m_Renderer.CPUOneCoreAccumulating(camera, m_Scene->GetWorld(), m_Scene->GetLights());
                break;
            }
            case 4:
            {

                SetScene();
                Camera camera                 = m_Scene->GetCamera();
                camera.UsePDF                 = m_UsePDF;
                camera.UseUnidirectionalLight = m_UseUnidirectionalLight;
                m_Renderer.CPUOneCoreStratified(camera, m_Scene->GetWorld(), m_Scene->GetLights());
                break;
            }
            case 5:
            {
                SetScene();
                Camera camera                 = m_Scene->GetCamera();
                camera.UsePDF                 = m_UsePDF;
                camera.UseUnidirectionalLight = m_UseUnidirectionalLight;
                m_Renderer.CPUMultiCore(camera, m_Scene->GetWorld(), m_Scene->GetLights());
                break;
            }
            case 6:
            {
                Camera camera                 = m_Scene->GetCamera();
                camera.UsePDF                 = m_UsePDF;
                camera.UseUnidirectionalLight = m_UseUnidirectionalLight;
                m_Renderer.CPUMultiCoreAccumulating(camera, m_Scene->GetWorld(), m_Scene->GetLights());
                break;
            }
            case 7:
            {
                SetScene();
                Camera camera                 = m_Scene->GetCamera();
                camera.UsePDF                 = m_UsePDF;
                camera.UseUnidirectionalLight = m_UseUnidirectionalLight;
                m_Renderer.CPUMultiCoreStratified(camera, m_Scene->GetWorld(), m_Scene->GetLights());
                break;
            }
            case 8:
            {

                Camera camera                 = m_Scene->GetCamera();
                camera.UsePDF                 = m_UsePDF;
                camera.UseUnidirectionalLight = m_UseUnidirectionalLight;
                m_Renderer.CPUMultiCoreStratifiedAccumulating(camera, m_Scene->GetWorld(), m_Scene->GetLights());
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
                m_Scene = std::make_shared<RTWeekOneDefaultScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 1:
                m_Scene = std::make_shared<RTWeekOneTestScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 2:
                m_Scene = std::make_shared<RTWeekOneFinalScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 3:
                m_Scene = std::make_shared<RTWeekNextDefaultScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 4:
                m_Scene = std::make_shared<RTWeekNextRandomSpheresScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 5:
                m_Scene = std::make_shared<RTWeekNextTwoSpheresScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 6:
                m_Scene = std::make_shared<RTWeekNextEarthScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 7:
                m_Scene = std::make_shared<RTWeekNextTwoPerlinSpheresScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 8:
                m_Scene = std::make_shared<RTWeekNextQuadsScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 9:
                m_Scene = std::make_shared<RTWeekNextSimpleLightScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 10:
                m_Scene = std::make_shared<RTWeekNextCornellBoxScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 11:
                m_Scene = std::make_shared<RTWeekNextCornellSmokeScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 12:
                m_Scene = std::make_shared<RTWeekNextFinalScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 13:
                m_Scene = std::make_shared<RTWeekRestACornellBoxScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 14:
                m_Scene = std::make_shared<RTWeekRestBCornellBoxMirrorScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 15:
                m_Scene = std::make_shared<RTWeekRestCCornellBoxGlassScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 16:
            default:
                m_Scene = std::make_shared<CornellBoxLightsScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
        }
    }

private:
    // Rendering
    Renderer m_Renderer;
    int      m_RendererId             = 7;
    double   m_LastRenderTime         = 0.0;
    bool     m_UsePDF                 = true;
    bool     m_UseUnidirectionalLight = true;

    // Scene
    std::shared_ptr<Scene> m_Scene        = nullptr;
    int                    m_SceneId      = 16;
    int                    m_SceneDepth   = 50;
    int                    m_SceneSamples = 64;

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

class ImGuiDemoLayer final : public Layer
{
public:
    void OnUIRender() override
    {
        ImGui::ShowDemoWindow();
    }
};

Application* CreateApplication(int argc, char** argv)
{
    ApplicationSpecs spec;
    spec.Name = "Ray Tracing";

    Application* app = new Application(spec);
    app->PushLayer<RayTracinUILayer>();
    // app->PushLayer<ImGuiDemoLayer>();
    app->SetMenubarCallback([app]()
                            {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Exit"))
            {
                app->Close();
            }
            ImGui::EndMenu();
        } });
    return app;
}

int main(const int argc, char** argv)
{
    Log::Init();
    LOG_INFO("Starting...");
    while(g_ApplicationRunning)
    {
        Application* app = CreateApplication(argc, argv);
        app->Run();
        delete app;
    }
    return 0;
}
