
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
	GID_BMC,
	GID_SRC,

	GID_LAST
};

// --

void VARARGS68K About_SetTags( struct Config *cfg, PTR object, ... );
#define mySetTags About_SetTags

extern Object *About_GUIObjects[GID_LAST];
#define GUIObjects About_GUIObjects

// --

// --

#endif // WIN_H
#endif // GUI_RA

// --
