
#ifndef _VIEWER_H_
#define _VIEWER_H_

#include <string>
#include <GLFW/glfw3.h>

#include "mathlib.h"

#include "studio.h"
#include "studio_model.h"
#include "studio_event.h"

#include "menu.h"

class ViewerInterface
{
public:
	void Init(char* modelname);
	void Render(void);

	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
	void CursorPosCallback(GLFWwindow* window, double xpos, double ypos);
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	//////////////////////////////////////////////////////////////////

	void Menu(void);
	void Menu_Bar(void);

	void Preferences(void);

	void ModelInfo(void);
	MENU_DEFINE(ModelInfo, ModelInfo(), true);

	void SequenceInfo(void);
	MENU_DEFINE(SequenceInfo, SequenceInfo(), true);

	//////////////////////////////////////////////////////////////////

private:
	void DrawModel(void);

	void Clear(void);

	void ModifySequence(bool advance);

	//////////////////////////////////////////////////////////////////

	inline void Pan(int x, int y)
	{
		transx += (x - ox) / 500.;
		transy -= (y - oy) / 500.;

		ox = x; oy = y;
	}

	inline void Rotate(int x, int y)
	{
#if 0
		rotx += x - ox;
		if (rotx > 360.) rotx -= 360.;
		else if (rotx < -360.) rotx += 360.;

		roty += y - oy;
		if (roty > 360.) roty -= 360.;
		else if (roty < -360.) roty += 360.;

		ox = x; oy = y;
#endif
		rotx += y - oy;
		if (rotx > 360.) rotx -= 360.;
		else if (rotx < -360.) rotx += 360.;

		rotz += x - ox;
		if (rotz > 360.) rotz -= 360.;
		else if (rotz < -360.) rotz += 360.;

		ox = x; oy = y;
	}

	inline void Orbit(int x, int y)
	{
		roty += x - ox;
		if (roty > 360.) roty -= 360.;
		else if (roty < -360.) roty += 360.;

		ox = x;
	}

	inline void Zoom(int x, int y)
	{
#if 0
		// transz += (x - ox) / 20.;
		transz += (x - ox) / 50.;

		ox = x;
#endif
		float zoom_modifier = 50;

		transz += (y - oy) / zoom_modifier;
		oy = y;
	}

	inline void Motion(int x, int y)
	{
		switch (mot)
		{
		case PAN: Pan(x, y); break;
		case ROT: Rotate(x, y); break;
		case ZOOM: Zoom(x, y); break;
		default: break;
		}
	}

	inline void ModifyView(void)
	{
		if (!viewmodel)
		{
			oldtx = transx, oldty = transy, oldtz = transz;
			oldrx = rotx, oldry = roty, oldrz = rotz;

			transx = 0, transy = 0, transz = -0.125;
			rotx = 90, roty = 180, rotz = -90;

			viewmodel = true;
		}
		else
		{
			transx = oldtx, transy = oldty, transz = oldtz;
			rotx = oldrx, roty = oldry, rotz = oldrz;

			viewmodel = false;
		}
	}

	inline void DefaultView(void)
	{
		transx = 0.2, transy = -0.4, transz = -1.25;
		rotx = -90, roty = 0, rotz = -90;
	}

public:
	static StudioModel model;

private:
	//
	// motion
	//
	enum mot_t {
		PAN = 1,
		ROT,
		ZOOM
	};

	int ox, oy;
	int mot;
	bool mot_press = false;

	//
	// starting position
	//
	float transx = 0.2,
		transy = -0.4,
		transz = -1.25;

	//
	// starting angles
	//
	float rotx = -90,
		roty = 0,
		rotz = -90;

	//
	// time variables
	//
	static float prev;
	float curr;

	//
	// sequence count modifier
	//
	int increment = model.GetSequence();

	//
	// current bodygroup + submodel
	//
	int body_current = 0;
	int sub_current = 0;

	//
	// current blend angle + controller value
	//
	int blend_angle = 0;
	float con_angle[MAXSTUDIOCONTROLLERS] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	//
	// old view position + angles + viewmodel
	//
	bool viewmodel = false;

	float oldtx, oldty, oldtz,
		oldrx, oldry, oldrz;
};

#endif
