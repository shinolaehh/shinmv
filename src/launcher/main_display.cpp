
#include "main.h"

// #define SHOW_DEMO_WINDOW		// <- uncomment for imgui demo

void Display_Main::Render(void)
{
#ifdef SHOW_DEMO_WINDOW
	if (show_demo_window)
		ImGui::ShowDemoWindow(&show_demo_window);
#endif

#if 0
	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

		ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		ImGui::Checkbox("Another Window", &show_another_window);

		ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
			counter++;
		ImGui::SameLine();
		ImGui::Text("counter = %d", counter);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
		ImGui::Text("Hello from another window!");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}
#endif

	Menu_Bar();

	viewer.Menu();
}

void Display_Main::Menu_Bar(void)
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Load...", "")) {}
			if (ImGui::MenuItem("Save...", "", false, false)) {}
			ImGui::Separator();
			if (ImGui::MenuItem("Exit", "")) {}
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Preferences"))
		{
			ImGui::Text("General");

			//
			// imgui preferences
			//
			ImGui::ColorEdit3("Background Colour", (float*)&clear_color);
			ImGui::Separator();

			//
			// model viewer preferences
			//
			viewer.Preferences();

			//
			// little about section
			//
			ImGui::Separator();

			if (ImGui::Button("About..."))
				ImGui::OpenPopup("About");

			if (ImGui::BeginPopupModal("About", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
			{
				ImGui::Text("Shinola's Model Viewer\n%s, %s", MV_VERSION, __DATE__);
				ImGui::Separator();

				if (ImGui::Button("OK", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
				ImGui::EndPopup();
			}

			ImGui::EndMenu();
		}

		viewer.Menu_Bar();

		ImGui::EndMainMenuBar();
	}
}
