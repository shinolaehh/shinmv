
#ifndef _DISPLAY_MAIN_H_
#define _DISPLAY_MAIN_H_

#include "menu.h"

class Display_Main
{
public:
	void Render(void);
	void Menu_Bar(void);
public:
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
};

#endif