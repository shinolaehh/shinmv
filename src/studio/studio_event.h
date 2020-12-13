/***
*
*	Copyright (c) 1996-2002, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
#ifndef _STUDIO_EVENT_H_
#define _STUDIO_EVENT_H_

typedef struct mstudioevent_s
{
	int 				frame;
	int					event;
	int					type;
	char				options[64];
} mstudioevent_t;

//////////////////////////////////////////////////////////////////

typedef struct {
	int	type;
	char* name;
	char* argument;
} event_map_t;

#define EVENT_MAP_SIZE 21

extern event_map_t event_map[EVENT_MAP_SIZE];
extern int event_map_element;

#endif // _STUDIO_EVENT_H_
