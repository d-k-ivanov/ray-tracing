
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

        ImGui::SeparatorText("Scene:");
        const char* sceneList[] = {
            "RTWeekOne: Default",
            "RTWeekOne: Test",
            "RTWeekOne: Final",
            "RTWeekNext: Default",
            "RTWeekNext: Random Spheres",
            "RTWeekNext: Two Spheres",
            "RTWeekNext: Earth",
            "RTWeekNext: Two Perlin Spheres",
            "RTWeekNext: Quads"
        };
        ImGui::Combo("Name", &m_SceneId, sceneList, IM_ARRAYSIZE(sceneList));
        ImGui::InputInt("Samples ", &m_SceneSamples);
        ImGui::InputInt("Depth", &m_SceneDepth);

        ImGui::SeparatorText("Rendering");
        // ImGui::Text("Aspect Ratio: %.6f", m_AspectRatio);
        ImGui::InputDouble("Aspect Ratio", &m_AspectRatio);
        ImGui::Text("Viewport Width: %d", m_ViewportWidth);
        ImGui::Text("Viewport Height: %d", m_ViewportHeight);
        ImGui::Text("Image Width: %.2f", m_ImageWidth);
        ImGui::Text("Image Height: %.2f", m_ImageHeight);

        const char* rendererList[] = {
            "Random INT",
            "Hello World",
            "Ray Tracer 1Core",
            "Ray Tracer Parallel"
        };
        ImGui::Combo("Renderer", &m_RendererId, rendererList, IM_ARRAYSIZE(rendererList));

        if(ImGui::Button("Render", ImVec2(-FLT_MIN, 0.0f)))
        {
            Render();
        }

        static bool loopRendering = false;
        ImGui::Checkbox("Loop Rendering", &loopRendering);


        ImGui::Text("Rendering time: %.3fms", m_LastRenderTime);
        ImGui::End();

        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        // ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 225));
        // ImGui::SetNextWindowSizeConstraints(ImVec2(400, 225), ImVec2(400, 225));
        ImGui::Begin("Viewport");

        // m_ViewportWidth  = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
        // m_ViewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

        // m_ViewportWidth = static_cast<uint32_t>(m_Width);
        m_ViewportWidth  = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
        m_ViewportHeight = static_cast<uint32_t>(m_ViewportWidth / m_AspectRatio);
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


        if (loopRendering)
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
            default:
                scene = std::make_shared<RTWeekOneDefaultScene>(m_AspectRatio, m_ViewportWidth, m_SceneSamples, m_SceneDepth);
                break;
        }

        m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
        switch(m_RendererId)
        {
            case 0:
                m_Renderer.RenderRandom();
                break;
            case 1:
                m_Renderer.RenderHelloWorld();
                break;
            case 2:
                m_Renderer.RenderSingleCore(scene->GetCamera(), scene->GetWorld());
                break;
            case 3:
                m_Renderer.RenderMultiCore(scene->GetCamera(), scene->GetWorld());
                break;
            default:
                m_Renderer.RenderRandom();
                break;
        }
        m_LastRenderTime = timer.ElapsedMillis();
    }

private:
    Renderer m_Renderer;
    double   m_AspectRatio    = 16.0 / 9.0;
    float    m_ImageWidth     = 0;
    float    m_ImageHeight    = 0;
    uint32_t m_ViewportWidth  = 0;
    uint32_t m_ViewportHeight = 0;
    double   m_LastRenderTime = 0.0;
    int      m_RendererId     = 3;
    int      m_SceneId        = 7;
    int      m_SceneSamples   = 10;
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
    app->PushLayer<ImGuiDemoLayer>();
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
