
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
    virtual void OnUIRender() override
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
            "RTWeekRest: Cornell Box (Glass)"};
        ImGui::Combo("Scene", &m_SceneId, sceneList, IM_ARRAYSIZE(sceneList));
        ImGui::InputInt("Samples ", &m_SceneSamples);
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
            "CPU Ray Tracer: One Core",
            "CPU Ray Tracer: One Core (Accum)",
            "CPU Ray Tracer: Multi Core",
            "CPU Ray Tracer: Multi Core (Accum)",
            "CPU Ray Tracer: Multi Core Stratified"};
        ImGui::Combo("Renderer", &m_RendererId, rendererList, IM_ARRAYSIZE(rendererList));

        if((m_RendererId != 3) && (m_RendererId != 5))
        {
            if(ImGui::Button("Render", ImVec2(-FLT_MIN, 0.0f)))
            {
                Render();
            }
        }
        else
        {
            m_SceneSamples = static_cast<int>(m_Renderer.GetFrameCounter());
            if(ImGui::Button("Render", ImVec2(ImGui::GetWindowSize().x*0.45f, 0.0f)))
            {
                Render();
            }
            ImGui::SameLine();
            if(ImGui::Button("Reset", ImVec2(ImGui::GetWindowSize().x*0.45f, 0.0f)))
            {
                m_Renderer.ResetFrameCounter();
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
        const Timer            timer;
        std::shared_ptr<Scene> scene;
        switch(m_SceneId)
        {
            case 0:
                scene = std::make_shared<RTWeekOneDefaultScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 1:
                scene = std::make_shared<RTWeekOneTestScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 2:
                scene = std::make_shared<RTWeekOneFinalScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 3:
                scene = std::make_shared<RTWeekNextDefaultScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 4:
                scene = std::make_shared<RTWeekNextRandomSpheresScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 5:
                scene = std::make_shared<RTWeekNextTwoSpheresScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 6:
                scene = std::make_shared<RTWeekNextEarthScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 7:
                scene = std::make_shared<RTWeekNextTwoPerlinSpheresScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 8:
                scene = std::make_shared<RTWeekNextQuadsScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 9:
                scene = std::make_shared<RTWeekNextSimpleLightScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 10:
                scene = std::make_shared<RTWeekNextCornellBoxScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 11:
                scene = std::make_shared<RTWeekNextCornellSmokeScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 12:
                scene = std::make_shared<RTWeekNextFinalScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 13:
                scene = std::make_shared<RTWeekRestACornellBoxScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 14:
                scene = std::make_shared<RTWeekRestBCornellBoxMirrorScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
            case 15:
            default:
                scene = std::make_shared<RTWeekRestCCornellBoxGlassScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
        }

        m_Renderer.SetSize(m_ViewportWidth, m_ViewportHeight);
        switch(m_RendererId)
        {
            case 0:
                m_Renderer.RenderRandom();
                break;
            case 1:
                m_Renderer.RenderHelloWorld();
                break;
            case 2:
                m_Renderer.CPUOneCore(scene->GetCamera(), scene->GetWorld(), scene->GetLights());
                break;
            case 3:
                m_Renderer.CPUOneCoreAccumSamples(scene->GetCamera(), scene->GetWorld(), scene->GetLights());
                break;
            case 4:
                m_Renderer.CPUMultiCore(scene->GetCamera(), scene->GetWorld(), scene->GetLights());
                break;
            case 5:
                m_Renderer.CPUMultiCoreAccumSamples(scene->GetCamera(), scene->GetWorld(), scene->GetLights());
                break;
            case 6:
                m_Renderer.CPUMultiCoreStratified(scene->GetCamera(), scene->GetWorld(), scene->GetLights());
                break;
            default:
                m_Renderer.RenderRandom();
                break;
        }
        m_LastRenderTime = timer.ElapsedMilliseconds();
    }

private:
    Renderer m_Renderer;
    // double   m_AspectRatio    = 16.0 / 9.0;
    double   m_AspectRatio    = 1.0;
    int      m_Width          = 600;
    float    m_ImageWidth     = 0;
    float    m_ImageHeight    = 0;
    uint32_t m_ViewportWidth  = 600;
    uint32_t m_ViewportHeight = 0;
    double   m_LastRenderTime = 0.0;
    int      m_RendererId     = 6;
    int      m_SceneId        = 13;
    int      m_SceneSamples   = 64;
    int      m_SceneDepth     = 50;

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
