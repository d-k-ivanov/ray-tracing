#include <UI/Application.h>
#include <UI/Image.h>
#include <UI/Layer.h>
#include <Utils/Random.h>
#include <Utils/Timers.h>

#include <imgui.h>

#include "UI/ImGuiHelper.h"
#include "Utils/Log.h"

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
        ImGui::Text("Last render: %.3fms", m_LastRenderTime);
        if(ImGui::Button("Render"))
        {
            Render();
        }
        ImGui::End();

        // ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(400, 225));
        ImGui::SetNextWindowSizeConstraints(ImVec2(400, 225), ImVec2(400, 225));
        ImGui::Begin("Viewport");

        // m_ViewportWidth  = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
        // m_ViewportHeight = static_cast<uint32_t>(ImGui::GetContentRegionAvail().y);

        constexpr double aspectRatio = 16.0 / 9.0;
        m_ViewportWidth              = static_cast<uint32_t>(400);
        // m_ViewportWidth  = static_cast<uint32_t>(ImGui::GetContentRegionAvail().x);
        m_ViewportHeight = static_cast<uint32_t>(m_ViewportWidth / aspectRatio);
        m_ViewportHeight = (m_ViewportHeight < 1) ? 1 : m_ViewportHeight;

        if(m_Image)
        {
            const ImVec2 center = ImVec2(
                (ImGui::GetWindowSize().x - static_cast<float>(m_Image->GetWidth())) * 0.5f,
                (ImGui::GetWindowSize().y - static_cast<float>(m_Image->GetHeight())) * 0.5f);
            ImGui::SetCursorPos(center);
            ImGui::Image(
                m_Image->GetDescriptorSet(),
                {static_cast<float>(m_Image->GetWidth()), static_cast<float>(m_Image->GetHeight())});
        }

        ImGui::End();
        ImGui::PopStyleVar();

        // Render();
    }

    void Render()
    {
        const Timer timer;

        if(!m_Image || m_ViewportWidth != m_Image->GetWidth() || m_ViewportHeight != m_Image->GetHeight())
        {
            m_Image = std::make_shared<Image>(m_ViewportWidth, m_ViewportHeight, ImageFormat::RGBA);
            delete[] m_ImageData;
            m_ImageData = new uint32_t[static_cast<uint64_t>(m_ViewportWidth * m_ViewportHeight)];
        }

        for(uint32_t j = 0; j < m_ViewportHeight; j++)
        {
            for(uint32_t i = 0; i < m_ViewportWidth; i++)
            {
                const auto r = static_cast<double>(i) / (m_ViewportWidth - 1);
                const auto g = static_cast<double>(j) / (m_ViewportHeight - 1);
                const auto b = 0;
                const auto a = 255;

                const auto ir = static_cast<uint8_t>(255.999 * r);
                const auto ig = static_cast<uint8_t>(255.999 * g);
                const auto ib = static_cast<uint8_t>(255.999 * b);
                const auto ia = static_cast<uint8_t>(255.999 * a);

                m_ImageData[j * m_Image->GetWidth() + i] = (ia << 24) | (ib << 16) | (ig << 8) | ir;
            }
        }

        // for(uint32_t i = 0; i < m_ViewportWidth * m_ViewportHeight; i++)
        // {
        //     m_ImageData[i] = Random::UInt();
        //     m_ImageData[i] |= 0xff000000;
        // }

        m_Image->SetData(m_ImageData);

        m_LastRenderTime = timer.ElapsedMillis();
    }

private:
    std::shared_ptr<Image> m_Image;
    uint32_t*              m_ImageData      = nullptr;
    double                 m_LastRenderTime = 0.0;
    uint32_t               m_ViewportWidth  = 0;
    uint32_t               m_ViewportHeight = 0;
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
