
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

	// P96 
	RGBFTYPE pixfmt = p96GetBitMapAttr( scr->RastPort.BitMap, P96BMA_RGBFORMAT );

	// P96
	cfg->GfxRead_Screen_ModeID		= modeid;
	cfg->GfxRead_Screen_Format		= myConvert_P96_2_VNC_Mode( pixfmt );
	cfg->GfxRead_Screen_ViewWidth	= p96GetModeIDAttr( modeid, P96IDA_WIDTH );
	cfg->GfxRead_Screen_ViewHeight	= p96GetModeIDAttr( modeid, P96IDA_HEIGHT );
}

// --
