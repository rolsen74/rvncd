
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

enum UpdateStat NewBuffer_Update_Tile_Check( struct Config *cfg, struct UpdateNode *un, U32 tiles_left, S32 hardcursor, U32 *tilesend )
{
struct TileInfo *ti;
S32 datalen;
U32 sendcnt;
U32 tilenr;
S32 stat;
//S32 rc;
S32 x1;
S32 y1;
S32 x2;
S32 y2;

	stat = US_Error;

	sendcnt = 0;

	tilenr = ( un->un_Data.update.urm_Incremental ) ? cfg->NetSend_ScreenCurrentTile : 0 ;

	x1 = un->un_Data.update.urm_XPos;
	y1 = un->un_Data.update.urm_YPos;
	x2 = un->un_Data.update.urm_XPos + un->un_Data.update.urm_Width;
	y2 = un->un_Data.update.urm_YPos + un->un_Data.update.urm_Height;

	while( tiles_left )
	{
		tilenr--;

		if ( tilenr > cfg->GfxRead_Screen_Tiles )
		{
			 tilenr = cfg->GfxRead_Screen_Tiles - 1;
		}

		ti = & cfg->GfxRead_Screen_TileInfoBuffer[tilenr];

		if ( ti->X > x2 )
		{
			continue;
		}

		if ( ti->X + ti->W < x1 )
		{
			continue;
		}

		if ( ti->Y > y2 )
		{
			continue;
		}

		if ( ti->Y + ti->H < y1 )
		{
			continue;
		}

		// --

		if (( un->un_Data.update.urm_Incremental ) && ( cfg->GfxRead_Screen_TileArrayBuffer[tilenr] >= 0 ))
		{
			// Count up untell it flips over to Negative
			cfg->GfxRead_Screen_TileArrayBuffer[tilenr]++;
			continue;
		}

		cfg->GfxRead_Screen_TileArrayBuffer[tilenr] = rand() % 20;

		// --
		// Get Graphics Data
	
		datalen = NewBuffer_Update_Tile_Fill( cfg, un, tilenr, hardcursor );

		if (( datalen <= 0 )
		||	( datalen > cfg->NetSend_SendBufferSize ))
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = mem_ASPrintF( "Internal: myNewFill failed" );
			}

			Log_PrintF( cfg, LOGTYPE_Error, "Buffer error" );
			goto bailout;
		}

		sendcnt++;

		if ( Tile_SendBuffer_Send( cfg, cfg->NetSend_SendBuffer, datalen ))
		{
			stat = US_Error;
			goto bailout;
		}

		tiles_left--;
	}

	cfg->NetSend_ScreenCurrentTile = tilenr;

	// --

	stat = US_Okay;

bailout:

	if ( tilesend )
	{
		*tilesend = sendcnt;
	}

	return( stat );
}

// --
