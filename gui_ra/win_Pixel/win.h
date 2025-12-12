
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

	GID_Pixel_BPP,
	GID_Pixel_Depth,
	GID_Pixel_Endian,
	GID_Pixel_TrueColor,
	GID_Pixel_MaxRed,
	GID_Pixel_MaxGreen,
	GID_Pixel_MaxBlue,
	GID_Pixel_ShiftRed,
	GID_Pixel_ShiftGreen,
	GID_Pixel_ShiftBlue,

	GID_ScreenMode,
	GID_ScreenWidth,
	GID_ScreenHeight,

	GID_EncMode,
	GID_EncMethod,

	GID_LAST
};

// --

void VARARGS68K Pixel_SetTags( struct Config *cfg, PTR object, ... );
#define mySetTags Pixel_SetTags

extern Object *Pixel_GUIObjects[GID_LAST];
#define GUIObjects Pixel_GUIObjects

// --



// --

#endif // WIN_H
#endif // GUI_RA

// --
