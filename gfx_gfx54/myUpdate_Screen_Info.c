
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

void myUpdate_Screen_Info( struct Config *cfg, struct Screen *scr )
{
	cfg->GfxRead_Screen_Adr			= scr;
	cfg->GfxRead_Screen_PageWidth	= scr->Width;
	cfg->GfxRead_Screen_PageHeight	= scr->Height;

	// GFX v39
	U32 modeid = BestModeID( BIDTAG_ViewPort, & scr->ViewPort, TAG_END );

	// GFX : GetBitMapAttr (v39/v54)
	enum enPixelFormat pixfmt = GetBitMapAttr( scr->RastPort.BitMap, BMA_PIXELFORMAT );

	// GFX v36
	struct DimensionInfo di;
	GetDisplayInfoData( NULL, (PTR) & di, sizeof( di ), DTAG_DIMS, modeid );

	// GFX
	cfg->GfxRead_Screen_ModeID		= modeid;
	cfg->GfxRead_Screen_Format		= myConvert_GFX54_2_VNC_Mode( pixfmt );
	cfg->GfxRead_Screen_ViewWidth	= di.Nominal.MaxX - di.Nominal.MinX + 1;
	cfg->GfxRead_Screen_ViewHeight	= di.Nominal.MaxY - di.Nominal.MinY + 1;
}

// --
