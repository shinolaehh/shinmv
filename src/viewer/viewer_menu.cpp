
#include <cmath>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"

#include "main.h"
#include "viewer.h"

#include "studio_activity.h"
#include "studio_activitymap.h"

void ViewerInterface::Menu(void)
{
	Menu_ModelInfo();
	Menu_SequenceInfo();
	Menu_TextureInfo();
}

void ViewerInterface::Menu_Bar(void)
{
	ImGui::Checkbox("Model Info", &Show_ModelInfo);
	ImGui::Checkbox("Sequence Info", &Show_SequenceInfo);
	ImGui::Checkbox("Texture Info", &Show_TextureInfo);
}

void ViewerInterface::Preferences(void)
{
	ImGui::Text("Model Viewer");

	bool newfilter = model.IsFiltered();
	bool newview = viewmodel;

	if (ImGui::Checkbox("Texture Filtering", &newfilter))
		model.ModifyFilter();

	// NOTE: for the life of me this just doesnt work yet
	// if (ImGui::Checkbox("Viewmodel View", &newview)) {}
}

void ViewerInterface::ModelInfo(void)
{
	//
	// window flags
	//
	ImGui::SetNextWindowPos(ImVec2(5, 410), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(325, 380), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(325, 250), ImVec2(325, (display_h - 35)));
	ImGui::Begin("Model Info", &Show_ModelInfo);

	studiohdr_t* header = model.ExposeHeader();
	mstudiobodyparts_t* body = (mstudiobodyparts_t*)((byte*)header + header->bodypartindex);

	//
	// general model stuff
	//
	ImGui::Text("Number of Bones: %i", header->numbones);
	ImGui::Text("Number of Hitboxes: %i", header->numhitboxes);
	ImGui::Text("Number of Attachments: %i", header->numhitboxes);

	ImGui::Separator();

	//
	// vector stuff
	//
	ImGui::Text("Eye Position:\n%f %f %f",
		header->eyeposition[0], header->eyeposition[1], header->eyeposition[2]);

	/*
	// !!TODO!! - unused??
	ImGui::Text("Minimum Movement Hull Size:\n%f %f %f",
		header->min[0], header->min[1], header->min[2]);

	ImGui::Text("Maximum Movement Hull Size:\n%f %f %f",
		header->max[0], header->max[1], header->max[2]);

	ImGui::Text("Minimum Bounding Box:\n%f %f %f",
		header->bbmin[0], header->bbmin[1], header->bbmin[2]);

	ImGui::Text("Maximum Bounding Box:\n%f %f %f",
		header->bbmax[0], header->bbmax[1], header->bbmax[2]);
	*/

	ImGui::Separator();

	//
	// bodygroup selector
	//
	ImGui::Text("Number of Bodygroups: %i", header->numbodyparts);

	if (ImGui::BeginCombo("    ", "Bodygroups"))
	{
		for (int i = 0; i < header->numbodyparts; i++)
		{
			const bool is_selected = (body_current == i);
			mstudiobodyparts_t* index = body + i;

			if (ImGui::Selectable(index->name, is_selected))
				body_current = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}

	//
	// submodel selector
	//
	mstudiobodyparts_t* index = body + body_current;
	std::string name;

	if (index->nummodels > 1 && ImGui::BeginCombo("     ", "Submodels"))
	{
		for (int i = 0; i < index->nummodels; i++)
		{
			const bool is_selected = (sub_current == i);

			name = "Submodel " + std::to_string(i + 1);

			if (ImGui::Selectable(name.c_str(), is_selected))
				sub_current = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();

			if (ImGui::IsItemDeactivatedAfterEdit())
				model.SetBodygroup(body_current, sub_current);
		}

		ImGui::EndCombo();
	}

	//
	// skingroup selector
	//
	studiohdr_t* texhdr;

	if (header->textureindex == 0)
		texhdr = model.ExposeTextureHeader();
	else
		texhdr = header;

	mstudiotexture_t* tex = (mstudiotexture_t*)((byte*)texhdr + texhdr->textureindex);

	if (texhdr->numskinfamilies > 1)
	{
		ImGui::Text("Number of Skins: %i", texhdr->numskinfamilies);

		if (ImGui::BeginCombo("       ", "Skins"))
		{
			for (int i = 0; i < texhdr->numskinfamilies; i++)
			{
				const bool is_selected = (skin_current == i);

				std::string title = "Skin " + std::to_string(i);

				if (ImGui::Selectable(title.c_str(), is_selected))
					skin_current = i;

				if (is_selected)
					ImGui::SetItemDefaultFocus();

				if (ImGui::IsItemDeactivatedAfterEdit())
					model.SetSkin(skin_current);
			}

			ImGui::EndCombo();
		}
	}

	ImGui::Separator();
	ImGui::NewLine();
	ImGui::Separator();

	//
	// bone controller modifiers
	//
	mstudiobonecontroller_t* controller = (mstudiobonecontroller_t*)((byte*)header + header->bonecontrollerindex);
	static bool mouth = false;

	ImGui::Text("Number of Bone Controllers: %i", header->numbonecontrollers);

	if (header->numbonecontrollers > 0)
	{
		// ImGui::Separator();

		for (int j = 0; j < header->numbonecontrollers; j++)
		{
			ImGui::Separator();

			// ImGui::Text("Index: %i", controller[j].index);

			float con_angle_min = model.RoundController(j, controller[j].start, true);
			float con_angle_max = model.RoundController(j, controller[j].end, true);

			std::string title;
			std::string title2;

			//
			// if the mouth index is found, set the name for the definition and
			// the reset button
			//
			if (controller[j].index == 4)
			{
				title = "Mouth";
				title2 = "Reset Mouth";
			}
			else
			{
				title = "Controller " + std::to_string(j);
				title2 = "Reset Controller " + std::to_string(j);
			}

			ImGui::SliderFloat(title.c_str(), &con_angle[j], con_angle_min, con_angle_max);

			if (ImGui::Button(title2.c_str()))
				con_angle[j] = 0;

			ImGui::Separator();

			int flagcount = model.FindFrameMotionFlags(controller[j].type);

			if (flagcount > 0)
			{
				ImGui::Text("Controller Flags:");

				for (int i = 0; i < flagcount; i++)
				{
					ImGui::Text(" ");
					ImGui::SameLine();
					ImGui::Text(flag_map[flag_map_element[i]].name);
				}
			}
			else
				ImGui::Text("Controller Flags: None");

			//
			// if the mouth index is found, set the mouth instead of the controller
			//
			if (controller[j].index == 4)
				model.SetMouth(con_angle[j]);
			else
				model.SetController(j, con_angle[j]);

			ImGui::Separator();
		}

		ImGui::NewLine();
	}


	ImGui::End();
}

void ViewerInterface::SequenceInfo(void)
{
	mstudioseqdesc_t* seqdata = model.ExposeSequence();

	//
	// window flags
	//
	ImGui::SetNextWindowPos(ImVec2(5, 25), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(325, 380), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(325, 325), ImVec2(325, (display_h - 35)));

	ImGui::Begin("Sequence Info", &Show_SequenceInfo);

	//
	// generic sequence information
	//
	studiohdr_t* header = model.ExposeHeader();

	ImGui::Text("Number of Sequences: %i", header->numseq);
	ImGui::Text("Number of Sequence Groups: %i", header->numseqgroups);

	ImGui::Separator();

	//
	// specific sequence information
	//
	ImGui::Text("Name: %s", seqdata->label);

	int newinc = model.GetSequence();
	ImGui::SliderInt("        ", &newinc, 0, model.MaxSequence(), "Sequence %d");

	if (ImGui::IsItemEdited())
	{
		increment = newinc;

		if (model.IsFramePaused())
			model.ModifyPause();

		model.SetSequence(increment);
	}

	ImGui::Text("Number of Frames: %i", seqdata->numframes);
	// ImGui::Text("Number of Pivots: %i", seqdata->numpivots);

	int newframe = model.GetFrame();
	ImGui::SliderInt("  ", &newframe, 0, seqdata->numframes - 1, "Frame %d");

	if (ImGui::IsItemEdited())
		model.SetFrame(newframe);

	if (ImGui::IsItemDeactivated())
	{
		if (!model.IsFramePaused())
			model.ModifyPause();

		model.AdvanceFrameFrom(0, newframe);
	}

	ImGui::SameLine();
	
	if (ImGui::Button("Pause"))
		model.ModifyPause();

	ImGui::Text("Sequence Group: %i", seqdata->seqgroup);

	ImGui::Separator();

	//
	// frame information
	//
	ImGui::Text("FPS: %f", seqdata->fps);

	bool looping;
	seqdata->flags ? looping = true : looping = false;
	ImGui::Checkbox("Looping", &looping);

	ImGui::Separator();

	//
	// activity information
	//
	ImGui::Text("Activity: %s", activity_map[seqdata->activity].name);
	ImGui::Text("Activity Weight: %i", seqdata->actweight);

	ImGui::Separator();

	//
	// motion flag information
	//
	int flagcount = model.FindFrameMotionFlags(seqdata->motiontype);

	if (flagcount > 0)
	{
		ImGui::Text("Motion Flags:");

		for (int i = 0; i < flagcount; i++)
		{
			ImGui::Text(" ");
			ImGui::SameLine();
			ImGui::Text(flag_map[flag_map_element[i]].name);
		}
	}
	else
		ImGui::Text("Motion Flags: None");

	ImGui::Separator();
	ImGui::NewLine();
	ImGui::Separator();

	//
	// blend information
	//
	ImGui::Text("Number of Blends: %i", model.MaxBlend());

	if (seqdata->numblends > 1)
	{
		// ImGui::Separator();

		for (int j = 0; j < model.MaxBlend(); j++)
		{
			ImGui::Separator();

			int angle_min = model.RoundBlend(j, seqdata->blendstart[j], true);
			int angle_max = model.RoundBlend(j, seqdata->blendend[j], true);
			static int angle = 0;

			std::string title = "Blend " + std::to_string(j);
			ImGui::SliderInt(title.c_str(), &angle, angle_min, angle_max);

			std::string title2 = "Reset Blend " + std::to_string(j);
			if (ImGui::Button(title2.c_str()))
				angle = 0;

			ImGui::Separator();

			ImGui::Text("Blend Parent: %i", seqdata->blendparent);

			flagcount = model.FindFrameMotionFlags(seqdata->blendtype[j]);

			if (flagcount > 0)
			{
				ImGui::Text("Blend Flags:");

				for (int i = 0; i < flagcount; i++)
				{
					ImGui::Text(" ");
					ImGui::SameLine();
					ImGui::Text(flag_map[flag_map_element[i]].name);
				}
			}
			else
				ImGui::Text("Blend Flags: None");

			//
			// this gets called, no matter what, so blends can properly work
			//
			model.SetBlending(j, angle);

			ImGui::Separator();
		}

		ImGui::NewLine();
	}

	ImGui::Separator();

	//
	// event information
	//
	ImGui::Text("Number of Events: %i", model.MaxEvent());

	if (model.MaxEvent() > 0)
	{
		mstudioevent_t* mdlevent;

		// ImGui::Separator();

		for (int i = 0; i < model.MaxEvent(); i++)
		{
			mdlevent = model.GetEventInfo(i);

			ImGui::Separator();

			ImGui::Text("Event %i (Frame %i)", i, mdlevent->frame);
			// ImGui::Text("Sequence Frame: %i", mdlevent->frame);

			ImGui::Separator();

			//
			// mmm... conditionals...
			// !!TODO!! - the argument code is VERY unsafe!!
			// it should check in general if the options variable isn't empty!
			//
			if (model.FindEvent(mdlevent->event))
				ImGui::Text("Event Number: %i\n%s", mdlevent->event, event_map[event_map_element].name);
			else
				ImGui::Text("Event Number: %i", mdlevent->event);

			// !!TODO!! - unused?? check studiomdl code
			if (mdlevent->type > 0)
				ImGui::Text("Type: %i", mdlevent->type);

			if (event_map[event_map_element].argument != "")
				ImGui::Text("%s:\n%s", event_map[event_map_element].argument, mdlevent->options);

			ImGui::Separator();
		}
	}
	else
		ImGui::Separator();

	ImGui::End();
}

void ViewerInterface::TextureInfo(void)
{
	studiohdr_t* header = model.ExposeTextureHeader();
	mstudiotexture_t* tex = (mstudiotexture_t*)((byte*)header + header->textureindex);
	mstudiotexture_t* skin = (mstudiotexture_t*)((byte*)header + header->skinindex);

	//
	// window flags
	//
	ImGui::SetNextWindowPos(ImVec2(15, 50), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(325, 380), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSizeConstraints(ImVec2(325, 380), ImVec2(display_w, display_h));

	ImGui::Begin("Texture Info", &Show_TextureInfo);

	//
	// general model texture information
	//
	ImGui::Text("Number of Textures: %i", header->numtextures);

	ImGui::Separator();

	//
	// specific texture information
	//
	static int tex_number = 0;
	int tex_width = tex[tex_number].width;
	int tex_height = tex[tex_number].height;

	ImGui::Text("Name: %s", tex[tex_number].name);
	ImGui::Text("Size: %d x %d", tex_width, tex_height);
	ImGui::SliderInt("     ", &tex_number, 0, header->numtextures - 1, "Texture %d");

	ImGui::Separator();

#if 0
	//
	// skingroup selector
	// NOTE: THIS HAS MOVED TO MODELINFO
	//
	ImGui::Text("Number of Skingroups: %i", header->numskinfamilies);

	if (header->numskinfamilies > 1 && ImGui::BeginCombo("       ", "Skingroups"))
	{
		for (int i = 0; i < header->numskinfamilies; i++)
		{
			const bool is_selected = (skin_current == i);
			mstudiotexture_t* skin_tex = tex + (i * header->numskinref);

			std::string title = "Skin " + std::to_string(i);

			if (ImGui::Selectable(title.c_str(), is_selected))
				skin_current = i;

			if (is_selected)
				ImGui::SetItemDefaultFocus();

			if (ImGui::IsItemDeactivatedAfterEdit())
				model.SetSkin(skin_current);
		}
		ImGui::EndCombo();
	}

	ImGui::Separator();
#endif

	//
	// texture flags
	// TODO: use checkboxes!!
	//
	int flagcount = model.FindTextureFlags(tex[tex_number].flags);

	if (flagcount > 0)
	{
		ImGui::Text("Texture Flags:");

		for (int i = 0; i < flagcount; i++)
		{
			ImGui::Text(" ");
			ImGui::SameLine();
			ImGui::Text(flag_tex_map[flag_tex_map_element[i]].name);
		}
	}
	else
		ImGui::Text("Texture Flags: None");

	ImGui::Separator();

	//
	// scale settings
	// oh god, this code is messy....
	//
	static float tex_scale = 1.0f;
	std::string scale_text = std::to_string(tex_scale);

	ImGui::Text("Scale:");
	ImGui::SliderFloat("    ", &tex_scale, 0.5, 5, scale_text.c_str());

	//
	// (^ x) for more decimal values
	// x = power of / place to put decimal
	//
	const int scale_factor = 10 ^ 2;

	if (ImGui::IsItemEdited() && tex_scale > 0.5)
		tex_scale = std::round(tex_scale * scale_factor) / scale_factor;

	if (ImGui::Button("Reset Scale (1.0)")) // Regular Scale
		tex_scale = 1.0;

	/*
	ImGui::SameLine();

	if (ImGui::Button("Editor Scale (0.5)"))
		tex_scale = 0.5;
	*/

	ImGui::Separator();

	//
	// texture data
	// NOTE: ExposeTextureData is a yucky hack!!!
	// TODO: make it a one-off call?? its a necessary evil but it shouldn't occur
	// every single time the user wants to open the fucking texture info tab
	//
	model.ExposeTextureData();

	GLuint* tex_data = (GLuint*)tex[tex_number].index;
	ImGui::Image((void*)(intptr_t)tex_data, ImVec2(tex_width * tex_scale, tex_height * tex_scale));

	ImGui::End();
}
