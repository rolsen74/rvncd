
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --
// Count the number of Tiles that need an Update

U32 NewBuffer_Update_Tile_Count( struct Config *cfg, struct UpdateNode *un )
{
struct TileInfo *ti;
U32 tiles;
S32 tmpx;
S32 tmpy;
U32 pos;
S32 x1;
S32 y1;
S32 x2;
S32 y2;

	/*
	** Check ALL tiles and see if they are inside Requested Rect 
	** and count how many need an update
	*/

	tiles = 0;

	x1 = un->un_Data.update.urm_XPos;
	y1 = un->un_Data.update.urm_YPos;
	x2 = un->un_Data.update.urm_XPos + un->un_Data.update.urm_Width;
	y2 = un->un_Data.update.urm_YPos + un->un_Data.update.urm_Height;

	if ( cfg->cfg_Active_Settings.ServerMode == SM_Strict )
	{
		// in Struct mode, we need atleast one Tile
		tmpx  = rand() % un->un_Data.update.urm_Width;
		tmpx += un->un_Data.update.urm_XPos;
		tmpy  = rand() % un->un_Data.update.urm_Height;
		tmpx += un->un_Data.update.urm_YPos;
		myMarkDirtyXY( cfg, tmpx, tmpy );
	}

	for( pos=0 ; pos < cfg->GfxRead_Screen_Tiles ; pos++ )
	{
		ti = & cfg->GfxRead_Screen_TileInfoBuffer[pos];

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

		if (( un->un_Data.update.urm_Incremental ) && ( cfg->GfxRead_Screen_TileArrayBuffer[pos] >= 0 ))
		{
			continue;
		}

		tiles++;
	}

	return( tiles );
}

// --
