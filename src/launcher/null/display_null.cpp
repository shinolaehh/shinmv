//
// display_null.cpp
// template display
//
#include <imgui/imgui.h>

#include "display_null.hpp"

bool display_null_draw = false;

// Display_Null::Display_Null
// what the display constructs on load
Display_Null::Display_Null(void)
{
	ImGui::Begin("Template", false, ImGuiWindowFlags_NoResize);
	ImGui::Text("Hello, world!");
	ImGui::End();
}
