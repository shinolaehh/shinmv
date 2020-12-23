
#include <stdio.h>
#include <windows.h>

#include <gl/GL.h>
#include <gl/GLU.h>

#include "viewer.h"

float ViewerInterface::prev;
bool ViewerInterface::viewmodel = false;

StudioModel ViewerInterface::model;

void ViewerInterface::Clear(void)
{
	float depthmin = 0, depthmax = 10.0;

	glDepthFunc(GL_LEQUAL);
	glDepthRange(depthmin, depthmax);
	glDepthMask(1);
}

void ViewerInterface::DrawModel(void)
{
	Clear();

	// !!TODO!! - make this more portable!
	if (!model.IsFramePaused())
	{
		curr = GetTickCount() / 1000.0;
		model.AdvanceFrame(curr - prev);
		prev = curr;
	}

	model.DrawModel();
}

void ViewerInterface::Render(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPushMatrix();

	glTranslatef(transx, transy, transz);

	glRotatef(rotx, 1, 0, 0);
	glRotatef(roty, 0, 1, 0);
	glRotatef(rotz, 0, 0, 1);

	glScalef(0.01, 0.01, 0.01);

	glCullFace(GL_FRONT);
	glEnable(GL_DEPTH_TEST);

	DrawModel();

	glPopMatrix();
}

void ViewerInterface::Init(char* modelname)
{
	model.Init(modelname);
	model.SetSequence(0);

	// !!TODO!! - sliders??
	/*
	model.SetController(0, 0.0);
	model.SetController(1, 0.0);
	model.SetController(2, 0.0);
	model.SetController(3, 0.0);
	model.SetMouth(0);
	*/

	glEnable(GL_TEXTURE_2D);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	// gluPerspective(50, 1, 0.1, 10);
	gluPerspective(90, 1, 0.00001, 1024);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void ViewerInterface::ModifySequence(bool advance)
{
#if 0
	int sequence = model.GetSequence();

	if (advance)
		increment = sequence + 1;
	else
		increment = sequence - 1;

	if (sequence == model.SetSequence(increment))
		model.SetSequence(0);
#endif

	increment = model.GetSequence();

	advance ? increment++ : increment--;

	if (increment == (model.MaxSequence() + 1))
		model.SetSequence(0);
	else
		model.SetSequence(increment);
}

void ViewerInterface::CursorPosCallback(GLFWwindow* window, double xpos, double ypos)
{
	// printf("xpos: %i, ypos: %i\n", ox, oy);

	if (mot_press && !viewmodel)
		Motion(xpos, ypos);

	ox = xpos, oy = ypos;
}

void ViewerInterface::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
	if (action == GLFW_PRESS && !viewmodel)
	{
		mot_press = true;

		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT: mot = ROT; break;
		case GLFW_MOUSE_BUTTON_RIGHT: mot = ZOOM; break;
		case GLFW_MOUSE_BUTTON_MIDDLE: mot = PAN; break;
		default: break;
		}
	}

	if (action == GLFW_RELEASE)
		mot_press = false;
}

void ViewerInterface::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		//
		// F - toggle filtering
		//
		case GLFW_KEY_F:
		{
			model.ModifyFilter();
			break;
		}

		//
		// O - view relative to the origin
		//
		case GLFW_KEY_O:
		{
			transx = 0, transy = 0, transz = -1;
			break;
		}

		//
		// P - toggle player viewmodel view
		//
		case GLFW_KEY_P:
		{
			ModifyView();
			break;
		}

		//
		// PAGE UP - next sequence
		//
		case GLFW_KEY_PAGE_UP:
		{
			if (model.IsFramePaused())
				model.ModifyPause();

			ModifySequence(true);
			break;
		}

		//
		// PAGE DOWN - previous sequence
		//
		case GLFW_KEY_PAGE_DOWN:
		{
			if (model.IsFramePaused())
				model.ModifyPause();

			ModifySequence(false);
			break;
		}

		//
		// SPACE - pause the sequence
		//
		case GLFW_KEY_SPACE:
		{
			model.ModifyPause();
			break;
		}

		//
		// HOME - default view
		//
		case GLFW_KEY_HOME:
		{
			DefaultView();
			break;
		}

		default: break;
		}
	}

}
