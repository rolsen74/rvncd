
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __RVNCD_GFX_H__
#define __RVNCD_GFX_H__

// --

#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>

// --

enum VNCPix			myConvert_CGFX_2_VNC_Mode( U32 cgfx_mode );
U32					myConvert_VNC_2_CGFX_Mode( enum VNCPix vnc_mode );
void				myUpdate_Screen_Info( struct Config *cfg, struct Screen *scr );
void				myGet_Screen_Tile( struct Config *cfg, struct Screen *scr, U32 tilenr, U8 *buf1, S32 size );

// --

#endif // __RVNCD_GFX_H__
