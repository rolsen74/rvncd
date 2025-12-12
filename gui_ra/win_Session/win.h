
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

#ifdef GUI_RA

#ifndef WIN_H
#define WIN_H

// --

enum
{
	GID_Root,
	GID_Window,

	GID_Session_IP,
	GID_Session_Pixels,
	GID_Session_Pxl_Avr,
	GID_Session_Pxl_Min,
	GID_Session_Pxl_Max,
	GID_Session_Load,
	GID_Session_Read,
	GID_Session_Send,
	GID_Session_Tiles,
	GID_Session_Rich,
	GID_Session_ZLib,
	GID_Session_RRE,
	GID_Session_Raw,
	GID_Session_Connect,
	GID_Session_Duration,

	GID_LAST
};

// --

void VARARGS68K Session_SetTags( struct Config *cfg, PTR object, ... );
#define mySetTags Session_SetTags

extern Object *Session_GUIObjects[GID_LAST];
#define GUIObjects Session_GUIObjects

// --



// --

#endif // WIN_H
#endif // GUI_RA

// --
