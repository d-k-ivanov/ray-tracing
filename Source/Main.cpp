
#include <Render/Renderer.h>
#include <Render/Scene.h>
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
        ImGuiSettings::ShowFontSelector("Fonts");

        ImGui::SeparatorText("Rendering");
        if(ImGui::Button("Render", ImVec2(-FLT_MIN, 0.0f)))
        {
            Render();
        }
        ImGui::Text("Render time: %.3fms", m_LastRenderTime);
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
            const ImVec2 center = ImVec2(
                (ImGui::GetWindowSize().x - static_cast<float>(image->GetWidth())) * 0.5f,
                (ImGui::GetWindowSize().y - static_cast<float>(image->GetHeight())) * 0.5f);
            ImGui::SetCursorPos(center);
            ImGui::Image(
                image->GetDescriptorSet(),
                {static_cast<float>(image->GetWidth()), static_cast<float>(image->GetHeight())});
        }

        ImGui::End();
        ImGui::PopStyleVar();

        // Render();
    }

    void Render()
    {
        const Timer timer;
        const auto scene = std::make_shared<RtOneSceneFinal>(m_AspectRatio, m_ViewportWidth, 10, 10);
        m_Renderer.OnResize(m_ViewportWidth, m_ViewportHeight);
        m_Renderer.Render(scene->GetCamera(), scene->GetWorld());
        m_LastRenderTime = timer.ElapsedMillis();
    }

private:
    double                 m_AspectRatio = 16.0 / 9.0;
    Renderer               m_Renderer;
    // int                    m_Width          = 400;
    uint32_t               m_ViewportWidth  = 0;
    uint32_t               m_ViewportHeight = 0;
    double                 m_LastRenderTime = 0.0;
};

// class ImGuiDemoLayer final : public Layer
// {
// public:
//     void OnUIRender() override
//     {
//         ImGui::ShowDemoWindow();
//     }
// };

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
