// main.cpp

#include <ui/application.h>

#include <imgui.h>

bool g_ApplicationRunning = true;

class ExampleLayer final : public Layer
{
public:
    void OnUIRender() override
    {
        ImGui::Begin("Hello");
        ImGui::Button("Button");
        ImGui::End();

        ImGui::ShowDemoWindow();
    }
};

Application* CreateApplication(int argc, char** argv)
{
    ApplicationSpecs spec;
    spec.Name = "Ray Tracing";

    Application* app = new Application(spec);
    app->PushLayer<ExampleLayer>();
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
    while(g_ApplicationRunning)
    {
        Application* app = CreateApplication(argc, argv);
        app->Run();
        delete app;
    }
    return 0;
}
