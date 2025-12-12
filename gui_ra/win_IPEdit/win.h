
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
	GID_Type,
	GID_Str1,
	GID_Str2,
	GID_Str3,
	GID_Str4,
	GID_Update,
	GID_Cancel,
	GID_LAST
};

// --

void VARARGS68K IPEdit_SetTags( struct Config *cfg, PTR object, ... );
#define mySetTags IPEdit_SetTags

extern Object *IPEdit_GUIObjects[GID_LAST];
#define GUIObjects IPEdit_GUIObjects

// --



// --

#endif // WIN_H
#endif // GUI_RA

// --
