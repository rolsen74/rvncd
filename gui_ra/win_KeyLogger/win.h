
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

enum
{
	GID_Root,
	GID_Window,
	GID_ListView,
	GID_Clear,
	GID_LAST
};

// --

void VARARGS68K KeyLogger_SetTags( struct Config *cfg, PTR object, ... );
#define mySetTags KeyLogger_SetTags

extern Object *KeyLogger_GUIObjects[GID_LAST];
#define GUIObjects KeyLogger_GUIObjects

// --
