
#ifndef _MENU_H_
#define _MENU_H_

// x = menu name, y = function, z = default open
#define MENU_DEFINE(x, y, z) bool Show_##x = ##z; \
	void Menu_##x(void) { if (Show_##x) { ##y; } }

#endif
