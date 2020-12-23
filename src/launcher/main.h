
#ifndef _MAIN_H_
#define _MAIN_H_

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl2.h"

#include "viewer.h"
#include "main_display.h"

#define MV_VERSION			"Alpha 2"

static ViewerInterface viewer;
static Display_Main disp_main;

extern int display_w, display_h;

#endif
