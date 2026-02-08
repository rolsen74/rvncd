
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

	// GfxBase : GetVPModeID v36
	U32 modeid = GetVPModeID( & scr->ViewPort );

	// CyberGfx
	U32 pixfmt = GetCyberIDAttr( CYBRIDATTR_PIXFMT,	modeid );

	cfg->GfxRead_Screen_ModeID		= modeid;
	cfg->GfxRead_Screen_Format		= myConvert_CGFX_2_VNC_Mode( pixfmt );
	cfg->GfxRead_Screen_ViewWidth	= GetCyberIDAttr( CYBRIDATTR_WIDTH, modeid );
	cfg->GfxRead_Screen_ViewHeight	= GetCyberIDAttr( CYBRIDATTR_HEIGHT, modeid );
}

// --
