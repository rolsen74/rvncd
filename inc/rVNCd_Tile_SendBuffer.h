
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_TILEBUFFER_H__
#define __INC__RVNCD_TILEBUFFER_H__

// --

S32		Tile_SendBuffer_Init( struct Config *cfg );
void	Tile_SendBuffer_Free( struct Config *cfg );
S32		Tile_SendBuffer_Send( struct Config *cfg, PTR data, U32 len );
S32		Tile_SendBuffer_Flush( struct Config *cfg );

// --

#endif // __INC__RVNCD_ZLIB_H__
