
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --
// This function draws on top of a'Tile'

void NewBuffer_Cursor_Soft( struct Config *cfg, U8 *tilebuf, U32 tilenr )
{
struct TileInfo *ti;
U8 *mbuf;
U32 ppos2;
U32 ppos;
U32 tpos2;
U32 tpos;
U32 mpos2;
U32 mpos;
U32 bytes;
S32 delta;
//
S32 yy;
S32 xx;
// Tile
S32 tx;
S32 ty;
S32 tw;
S32 th;
// Mouse
S32 mx;
S32 my;
S32 mw;
S32 mh;
// Area
S32 ax;
S32 ay;
S32 aw;
S32 ah;

	ti = & cfg->GfxRead_Screen_TileInfoBuffer[ tilenr ];
	tx = ti->X;
	ty = ti->Y;
	tw = ti->W;
	th = ti->H;

	mx = cfg->cfg_RenderMouseX;
	my = cfg->cfg_RenderMouseY;
	mw = cfg->cfg_PointerWidth;
	mh = cfg->cfg_PointerHeight;

	// -- Check if Mouse overlap Tile

	if (( mx + mw <= tx )
	||	( tx + tw <= mx )
	||	( my + mh <= ty )
	||	( ty + th <= my ))
	{
		// Nope, nothing to do
		goto bailout;
	}

//	DebugPrintF( "NewBuffer_Cursor_Soft : TileNr #%lu\n", tilenr );
//	DebugPrintF( "Tx: %04lu, Ty %04lu, Tw %02lu, Th %02lu\n", tx, ty, tw, th );
//	DebugPrintF( "Mx: %04lu, My %04lu, Mw %02lu, Mh %02lu\n", mx, my, mw, mh );

	ax = MAX( tx, mx );
	ay = MAX( ty, my );
	aw = MIN( tx + tw, mx + mw ) - ax;
	ah = MIN( ty + th, my + mh ) - ay;

//	DebugPrintF( "Ax: %04lu, Ay %04lu, Aw %02lu, Ah %02lu\n", ax, ay, aw, ah );
//	DebugPrintF( "GfxRead FormatSize %lu\n", cfg->GfxRead_Encode_FormatSize );
	bytes = ( cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel + 7 ) / 8 ;
//	DebugPrintF( "GfxRead BitsPerPixel %lu\n", bytes );

	// -- Update Gfx

	// Check: Make sure Pointer is same format
	if ( NewBuffer_Cursor_Update( cfg ))
	{
		SHELLBUF_PRINTF( "Pointer: Buffer error\n" );
		goto bailout;
	}

	mbuf = cfg->cfg_PointerMask2;
	ppos = 0;			// Pointer Pos
	mpos = 0;			// Mask Pos
	tpos = 0;			// Tile Pos

	/**/ if ( mx < tx )
	{
		delta	= tx - mx;
		ppos	+= delta;
		mpos	+= delta;
	}
	else // ( mx > tx )
	{
		delta	= mx - tx;
		tpos	+= delta;
	}

	/**/ if ( my < ty )
	{
		delta	= ty - my;
		ppos	+= delta * cfg->cfg_PointerWidth;
		mpos	+= delta * 64;
	}
	else // ( my > ty )
	{
		delta	= my - ty;
		tpos	+= delta * cfg->GfxRead_Screen_TileSize;
	}

//	DebugPrintF( "mpos %04lu, tpos %04lu, ppos %04lu\n", mpos, tpos, ppos );

	/**/ if ( bytes == 1 )
	{
		U8 *pbuf = (PTR) cfg->cfg_PointerBuffer;
		U8 *tbuf = (PTR) tilebuf;

		for( yy=0 ; yy < ah ; yy++ )
		{
			mpos2 = mpos;
			tpos2 = tpos;
			ppos2 = ppos;

			for( xx=0 ; xx < aw ; xx++ )
			{
				if ( mbuf[mpos2] )
				{
					tbuf[tpos2] = pbuf[ppos2];
				}

				mpos2++;
				tpos2++;
				ppos2++;
			}

			mpos += 64;		// Pointer Mask width is fixed 64 Bytes
			ppos += cfg->cfg_PointerWidth;
			tpos += cfg->GfxRead_Screen_TileSize;
		}
	}
	else if ( bytes == 2 )
	{
		U16 *pbuf = (PTR) cfg->cfg_PointerBuffer;
		U16 *tbuf = (PTR) tilebuf;

		for( yy=0 ; yy < ah ; yy++ )
		{
			mpos2 = mpos;
			tpos2 = tpos;
			ppos2 = ppos;

			for( xx=0 ; xx < aw ; xx++ )
			{
				if ( mbuf[mpos2] )
				{
					tbuf[tpos2] = pbuf[ppos2];
				}

				mpos2++;
				tpos2++;
				ppos2++;
			}

			mpos += 64;		// Pointer Mask width is fixed 64 Bytes
			ppos += cfg->cfg_PointerWidth;
			tpos += cfg->GfxRead_Screen_TileSize;
		}
	}
	else if ( bytes == 4 )
	{
		U32 *pbuf = (PTR) cfg->cfg_PointerBuffer;
		U32 *tbuf = (PTR) tilebuf;

		for( yy=0 ; yy < ah ; yy++ )
		{
			mpos2 = mpos;
			tpos2 = tpos;
			ppos2 = ppos;

			for( xx=0 ; xx < aw ; xx++ )
			{
				if ( mbuf[mpos2] )
				{
					tbuf[tpos2] = pbuf[ppos2];
				}

				mpos2++;
				tpos2++;
				ppos2++;
			}

			mpos += 64;		// Pointer Mask width is fixed 64 Bytes
			ppos += cfg->cfg_PointerWidth;
			tpos += cfg->GfxRead_Screen_TileSize;
		}
	}
	else
	{
		SHELLBUF_PRINTF( "Pointer: FormatSize error (%ld)\n", bytes );
	}

	// --

bailout:

	return;
}

// --
