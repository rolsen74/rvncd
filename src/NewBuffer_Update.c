
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#ifdef SHOW_WORK_TILE
U32 TileOOO = 0;
#endif

enum UpdateStat NewUpdateReq_Msg( struct Config *cfg, struct UpdateNode *un )
{
struct GfxRectHeader *header;
enum UpdateStat stat;
S32 hardcursor;
S32 doCursor;
S32 lastrect;
U32 tilesend;
U32 total;
U32 tiles;

	tilesend = 0;
	lastrect = 0;

	// --
	// Sending RichCursor Updates, without a BufferUpdateReq
	// could fail if Client changed Screen format
	// happens with RealVNC client

	// --
	// Make a copy, so it don't change during Render
	cfg->cfg_RenderMouseX = cfg->cfg_CurMouseX;
	cfg->cfg_RenderMouseY = cfg->cfg_CurMouseY;

	// --

	doCursor = FALSE;
	tiles = NewBuffer_Update_Tile_Count( cfg, un );
	total = tiles;

	// --
	// are we Using Hardware Cursor ..
	hardcursor = (( cfg->cfg_Active_Settings.RichCursor ) && ( cfg->cfg_ServerSupportsCursor ) && ( cfg->NetSend_Encodings1[ ENCODE_RichCursor ] ));

	if (( hardcursor ) && ( cfg->cfg_UpdateCursor ))
	{
		doCursor = TRUE;
		total++;
	}

	// --
	if ( cfg->cfg_LastRect )
	{
		lastrect = TRUE;
		total++;
	}

	// --
	// Send Tile Header

	header = (PTR) cfg->NetSend_SendBuffer;
	header->bm_Type		= 0;	// Framebuffer Update
	header->bm_Pad		= 0;
	header->bm_Rects	= total;

	if ( Tile_SendBuffer_Send( cfg, header, sizeof( header )))
	{
		stat = US_Error;
		goto bailout;
	}

	// --
	// Update tiles needed updates?

	if ( total == 0 )
	{
		// lets return okay, so it can check queue
		// and if queue is empty, it will call delay
		stat = US_Okay;
		goto bailout;
	}

	// --
	// Print Cursor info

	if ( cfg->cfg_ServerCursorStat != hardcursor )
	{
		cfg->cfg_ServerCursorStat = hardcursor;

		if ( hardcursor )
		{
			Log_PrintF( cfg, LOGTYPE_Info, "Using Rich cursor" );
		}
		else
		{
			Log_PrintF( cfg, LOGTYPE_Info, "Using Soft cursor" );
		}
	}

	// --
	// Update Graphics Tiles

	if ( tiles )
	{
		stat = NewBuffer_Update_Tile_Check( cfg, un, tiles, hardcursor, & tilesend );

		if ( stat != US_Okay )
		{
			goto bailout;
		}
	}

	// --
	// Update Cursor Tile

	if ( doCursor )
	{
		stat = NewBuffer_Cursor_Rich( cfg );

		if ( stat != US_Okay )
		{
			goto bailout;
		}

		tilesend++;
		cfg->cfg_UpdateCursor = FALSE;
	}

	// -- All Done

	stat = US_Okay;		// No wait
//	stat = US_Wait;		// Do a Delay(2)

bailout:

	if ( lastrect )
	{
		stat = NewBuffer_LastRect( cfg );

		if ( stat != US_Okay )
		{
			goto bailout;
		}
	}

	Tile_SendBuffer_Flush( cfg );

	return( stat );
}

// --
