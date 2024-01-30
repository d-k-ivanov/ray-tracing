#include "ImGuiHelper.h"

#include <string_view>
#include <imgui.h>

void ImGuiSettings::ShowStyleSelector(const char* label)
{
    static int styleIdx = 1;
    if(ImGui::Combo(label, &styleIdx, "Dark\0Light\0Classic\0"))
    {
        switch(styleIdx)
        {
            case 0:
                ImGui::StyleColorsDark();
                break;
            case 1:
                ImGui::StyleColorsLight();
                break;
            case 2:
                ImGui::StyleColorsClassic();
                break;
            default:
                ImGui::StyleColorsLight();
                break;
        }
    }
}

void ImGuiSettings::ShowFontSelector(const char* label)
{
    ImGuiIO& io = ImGui::GetIO();

    const ImFont* fontCurrent = ImGui::GetFont();
    if(ImGui::BeginCombo(label, fontCurrent->GetDebugName()))
    {
        for(ImFont* font : io.Fonts->Fonts)
        {
            ImGui::PushID(font);
            if(ImGui::Selectable(font->GetDebugName(), font == fontCurrent))
                io.FontDefault = font;
            ImGui::PopID();
        }
        ImGui::EndCombo();
    }
}
