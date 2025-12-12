
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --
// -- Misc

static void myGetColor( struct Config *cfg, U32 tilenr, S32 xx, S32 yy, S32 *r, S32 *g, S32 *b )
{
U8 *buf;
S32 col;
S32 ts;
S32 tb;

	ts  = cfg->GfxRead_Screen_TileSize;
	tb  = ts * ts * sizeof( U32 ) * tilenr;
	tb += yy * ts * sizeof( U32 );
	tb += xx * sizeof( U32 );

	buf = & cfg->GfxRead_Screen_ChunkyBuffer[ tb ];

	col = buf[1];
	col >>= cfg->GfxRead_Encode_RedShift;
	*r	= col;

	col = buf[2];
	col >>= cfg->GfxRead_Encode_GreenShift;
	*g	= col;

	col = buf[3];
	col >>= cfg->GfxRead_Encode_BlueShift;
	*b	= col;
}

// --
// -- Tile Render 1,2 or 4 Bytes same Screen/Client Format

S32 TileRender_Copy( struct Config *cfg, PTR dstbuf, U32 tilenr )
{
struct TileInfo *ti;
PTR scrbuf;
S32 len;
S32 cnt;
S32 ts;
S32 tw;
S32 w;

	// --

	ti = & cfg->GfxRead_Screen_TileInfoBuffer[tilenr];
	ts = cfg->GfxRead_Screen_TileSize;
	tw = cfg->GfxRead_Encode_FormatSize * ts;

	scrbuf	= (PTR) ( (U32) cfg->GfxRead_Screen_ChunkyBuffer + tilenr * ts * tw );

	if ( ti->W != ts )
	{
		// Limited Tile Width

		w = ti->W * cfg->GfxRead_Encode_FormatSize;

		for( cnt=0 ; cnt < ti->H ; cnt++ )
		{
			memcpy( dstbuf, scrbuf, w );
			scrbuf = (PTR) ( (U32) scrbuf + tw );
			dstbuf = (PTR) ( (U32) dstbuf +  w );
		}

		len = w * ti->H;
	}
	else
	{
		// Full Tile Width

		len = ti->W * ti->H * cfg->GfxRead_Encode_FormatSize;

		memcpy( dstbuf, scrbuf, len );
	}

	// --

	return( len );
}

// --
// -- 8 Bit

S32 TileRender_Generic_8( struct Config *cfg, PTR buf, U32 tilenr )
{
struct TileInfo *ti;
U8 *dst;
U8 col;
S32 len;
S32 x;
S32 y;
S32 r;
S32 g;
S32 b;

	// --

	len	= 0;
	dst	= buf;
	ti	= & cfg->GfxRead_Screen_TileInfoBuffer[tilenr];

	for( y=0 ; y<ti->H ; y++ )
	{
		for( x=0 ; x<ti->W ; x++ )
		{
			myGetColor( cfg, tilenr, x, y, & r, & g, & b );

			col  = ( r << cfg->GfxRead_Encode_ActivePixel.pm_RedShift   );
			col |= ( g << cfg->GfxRead_Encode_ActivePixel.pm_GreenShift );
			col |= ( b << cfg->GfxRead_Encode_ActivePixel.pm_BlueShift  );

			dst[len++] = col;
		}
	}

	// --

	return( len );
}

// --
// -- 16 Bit

S32 TileRender_Generic_16BE( struct Config *cfg, PTR buf, U32 tilenr )
{
struct TileInfo *ti;
U16 *dst;
U16 col;
S32 len;
S32 x;
S32 y;
S32 r;
S32 g;
S32 b;

	// --

	len	= 0;
	dst	= buf;
	ti	= & cfg->GfxRead_Screen_TileInfoBuffer[tilenr];

	for( y=0 ; y<ti->H ; y++ )
	{
		for( x=0 ; x<ti->W ; x++ )
		{
			myGetColor( cfg, tilenr, x, y, & r, & g, & b );

			col  = ( r << cfg->GfxRead_Encode_ActivePixel.pm_RedShift   );
			col |= ( g << cfg->GfxRead_Encode_ActivePixel.pm_GreenShift );
			col |= ( b << cfg->GfxRead_Encode_ActivePixel.pm_BlueShift  );

			dst[len++] = col;
		}
	}

	len = len * 2;

	// --

	return( len );
}

// --
// -- 16 Bit Little Endian

S32 TileRender_Generic_16LE( struct Config *cfg, PTR buf, U32 tilenr )
{
struct TileInfo *ti;
U16 *dst;
U16 col;
S32 len;
S32 x;
S32 y;
S32 r;
S32 g;
S32 b;

	// --

	len	= 0;
	dst	= buf;
	ti	= & cfg->GfxRead_Screen_TileInfoBuffer[tilenr];

	for( y=0 ; y<ti->H ; y++ )
	{
		for( x=0 ; x<ti->W ; x++ )
		{
			myGetColor( cfg, tilenr, x, y, & r, & g, & b );

			col  = ( r << cfg->GfxRead_Encode_ActivePixel.pm_RedShift   );
			col |= ( g << cfg->GfxRead_Encode_ActivePixel.pm_GreenShift );
			col |= ( b << cfg->GfxRead_Encode_ActivePixel.pm_BlueShift  );

			dst[len++] = SWAP16( col );
		}
	}

	len = len * 2;

	// --

	return( len );
}

// --
// -- 32 Bit

S32 TileRender_Generic_32BE( struct Config *cfg, PTR buf, U32 tilenr )
{
struct TileInfo *ti;
U32 *dst;
U32 col;
S32 len;
S32 x;
S32 y;
S32 r;
S32 g;
S32 b;

	// --

	len	= 0;
	dst	= buf;
	ti	= & cfg->GfxRead_Screen_TileInfoBuffer[tilenr];

	for( y=0 ; y<ti->H ; y++ )
	{
		for( x=0 ; x<ti->W ; x++ )
		{
			myGetColor( cfg, tilenr, x, y, & r, & g, & b );

			col  = ( r << cfg->GfxRead_Encode_ActivePixel.pm_RedShift   );
			col |= ( g << cfg->GfxRead_Encode_ActivePixel.pm_GreenShift );
			col |= ( b << cfg->GfxRead_Encode_ActivePixel.pm_BlueShift  );

			dst[len++] = col;
		}
	}

	len = len * 4;

	// --

	return( len );
}

// --
// -- 32 Bit Little Endian

S32 TileRender_Generic_32LE( struct Config *cfg, PTR buf, U32 tilenr )
{
struct TileInfo *ti;
U32 *dst;
U32 col;
S32 len;
S32 x;
S32 y;
S32 r;
S32 g;
S32 b;

	// --

	len	= 0;
	dst	= buf;
	ti	= & cfg->GfxRead_Screen_TileInfoBuffer[tilenr];

	for( y=0 ; y<ti->H ; y++ )
	{
		for( x=0 ; x<ti->W ; x++ )
		{
			myGetColor( cfg, tilenr, x, y, & r, & g, & b );

			col  = ( r << cfg->GfxRead_Encode_ActivePixel.pm_RedShift   );
			col |= ( g << cfg->GfxRead_Encode_ActivePixel.pm_GreenShift );
			col |= ( b << cfg->GfxRead_Encode_ActivePixel.pm_BlueShift  );

			dst[len++] = SWAP32( col );
		}
	}

	len = len * 4;

	// --

	return( len );
}

// --
