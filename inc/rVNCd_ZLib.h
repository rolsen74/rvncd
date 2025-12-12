
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_ZLIB_H__
#define __INC__RVNCD_ZLIB_H__

// --

#include <proto/z.h>

// --

S32		ZLib_Init(		struct Config *cfg );
void	ZLib_Free(		struct Config *cfg );
void	ZLib_Cleanup(	struct Config *cfg );
void	ZLib_Setup(	struct Config *cfg );

// --

#endif // __INC__RVNCD_ZLIB_H__
