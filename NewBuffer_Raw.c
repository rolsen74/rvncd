
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

#if 0

struct GfxRectBuffer
{
	uint16	br_XPos;
	uint16	br_YPos;
	uint16	br_Width;
	uint16	br_Height;
	uint32	br_Encoding;
};

#endif

// --

int myEnc_Raw( struct Config *cfg, struct UpdateNode *un, int tile, int hardcursor )
{
struct GfxRectBuffer *rect;
struct TileInfo *ti;
uint8 *data;
int datalen;
int len;
int x;
int y;
int w;
int h;

	len = -1;

	ti = & cfg->GfxRead_Screen_TileInfoBuffer[tile];

	rect = cfg->NetSend_SendBuffer;
	data = cfg->NetSend_SendBuffer;

	x = ti->X;
	y = ti->Y;
	w = ti->W;
	h = ti->H;

	if ( un->un_XPos > x )
	{
		x = un->un_XPos;
		w -= ( un->un_XPos - x );
	}

	if ( un->un_YPos > y )
	{
		y = un->un_YPos;
		h -= ( un->un_YPos - y );
	}

	if (( cfg->GfxRead_Encode_RenderTile )
	&&	( x >= 0 ) && ( y >= 0 ) && ( w > 0 ) && ( h > 0 ))
	{
		rect->br_XPos		= x;
		rect->br_YPos		= y;
		rect->br_Width		= w;
		rect->br_Height		= h;
		rect->br_Encoding	= 0; // Raw

		IExec->ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

		datalen = cfg->GfxRead_Encode_RenderTile( cfg, & data[ sizeof( struct GfxRectBuffer ) ], tile );

		if ( datalen <= 0 )
		{
			printf( "RAW: GfxRead_Encode_RenderTile error\n" );
		}

		IExec->ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );

		if ( ! hardcursor )
		{
			NewBuffer_AddCursor( cfg, & data[ sizeof( struct GfxRectBuffer ) ], tile );
		}
	}
	else
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Internal: TileRender not set" );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "TileRender not set" );
		datalen = -1;
	}

	if ( datalen <= 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Internal: Data Length error" );
		}

		printf( "raw data error\n" );
		goto bailout;
	}

	len  = datalen;
	len += sizeof( struct GfxRectBuffer ); 

	cfg->SessionStatus.si_Tiles_Raw++;
	cfg->SessionStatus.si_Tiles_Total++;

bailout:

	return( len );
}

// --
