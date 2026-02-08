
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

void myGet_Screen_Tile( struct Config *cfg, struct Screen *scr, U32 tilenr, U8 *buf1, S32 size )
{
struct TileInfo *ti;
S32 sx;
S32 sy;
S32 ts;

	ti = cfg->GfxRead_Screen_TileInfoBuffer;
	ts = cfg->GfxRead_Screen_TileSize;
	sx = ti[tilenr].X;
	sy = ti[tilenr].Y;

	if ( cfg->GfxRead_Screen_ViewMode == VIEWMODE_View )
	{
		if ( cfg->GfxRead_Screen_Adr->ViewPort.DxOffset < 0 )
		{
			sx -= cfg->GfxRead_Screen_Adr->ViewPort.DxOffset;
		}

		if ( cfg->GfxRead_Screen_Adr->ViewPort.DyOffset < 0 )
		{
			sy -= cfg->GfxRead_Screen_Adr->ViewPort.DyOffset;
		}
	}

	// We need to clear the tile first?
	if ( ti[tilenr].C )
	{
		memset( buf1, 0, size );
	}

	// P96
	struct RenderInfo ri;

	ri.Memory		= buf1;
	ri.BytesPerRow	= cfg->GfxRead_Encode_FormatSize * ts;
	ri.RGBFormat	= cfg->GfxRead_Encode_Format2;

	p96ReadPixelArray(
		& ri, 				// ri
		0,					// DestX, 
		0,					// DestY, 
		& scr->RastPort,	// rp 
		sx,					// SrcX, 
		sy,					// SrcY, 
		ti[tilenr].W,		// SizeX (Width)
		ti[tilenr].H		// SizeY (Height)
	);
}

// --
