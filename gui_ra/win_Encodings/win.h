
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

	GID_Encoding_Listview,

	GID_LAST
};

// --

void VARARGS68K Encodings_SetTags( struct Config *cfg, PTR object, ... );
#define mySetTags Encodings_SetTags

extern Object *Encodings_GUIObjects[GID_LAST];
#define GUIObjects Encodings_GUIObjects

// --



// --

#endif // WIN_H
#endif // GUI_RA

// --
