
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --
// -- Misc

static inline void myGetColor( struct Config *cfg, int tile, int xx, int yy, int *r, int *g, int *b )
{
uint8 *buf;
int col;
int ts;
int tb;

	ts  = cfg->GfxRead_Screen_TileSize;
	tb  = ts * ts * 4 * tile;
	tb += yy * ts * 4;
	tb += xx * 4;

	buf = & cfg->GfxRead_Screen_ChunkyBuffer[ tb ];

	col = buf[1];
	col >>= cfg->GfxRead_Enocde_RedShift;
	*r	= col;

	col = buf[2];
	col >>= cfg->GfxRead_Enocde_GreenShift;
	*g	= col;

	col = buf[3];
	col >>= cfg->GfxRead_Enocde_BlueShift;
	*b	= col;
}

// --
// -- Tile Render 1,2 or 4 Bytes same Screen/Client Format

int TileRender_Copy( struct Config *cfg, APTR dstbuf, int tile )
{
struct TileInfo *ti;
APTR scrbuf;
int len;
int cnt;
int ts;
int tw;
int w;

	// --

	ti = & cfg->GfxRead_Screen_TileInfoBuffer[tile];
	ts = cfg->GfxRead_Screen_TileSize;
	tw = cfg->GfxRead_Encode_FormatSize * ts;

	scrbuf	= cfg->GfxRead_Screen_ChunkyBuffer;
	scrbuf += tile * ts * tw;

	if ( ti->W != ts )
	{
		// Limited Tile Width

		w = ti->W * cfg->GfxRead_Encode_FormatSize;

		for( cnt=0 ; cnt < ti->H ; cnt++ )
		{
			memcpy( dstbuf, scrbuf, w );
			scrbuf += tw;
			dstbuf += w;
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

int TileRender_Generic_8( struct Config *cfg, APTR buf, int tile )
{
struct TileInfo *ti;
uint8 *dst;
uint8 col;
int len;
int x;
int y;
int r;
int g;
int b;

	// --

	len	= 0;
	dst	= buf;
	ti	= & cfg->GfxRead_Screen_TileInfoBuffer[tile];

	for( y=0 ; y<ti->H ; y++ )
	{
		for( x=0 ; x<ti->W ; x++ )
		{
			myGetColor( cfg, tile, x, y, & r, & g, & b );

			col  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift   );
			col |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
			col |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift  );

			dst[len++] = col;
		}
	}

	// --

	return( len );
}

// --
// -- 16 Bit

int TileRender_Generic_16BE( struct Config *cfg, APTR buf, int tile )
{
struct TileInfo *ti;
uint16 *dst;
uint16 col;
int len;
int x;
int y;
int r;
int g;
int b;

	// --

	len	= 0;
	dst	= buf;
	ti	= & cfg->GfxRead_Screen_TileInfoBuffer[tile];

	for( y=0 ; y<ti->H ; y++ )
	{
		for( x=0 ; x<ti->W ; x++ )
		{
			myGetColor( cfg, tile, x, y, & r, & g, & b );

			col  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift   );
			col |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
			col |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift  );

			dst[len++] = col;
		}
	}

	len = len * 2;

	// --

	return( len );
}

// --
// -- 16 Bit Little Endian

int TileRender_Generic_16LE( struct Config *cfg, APTR buf, int tile )
{
struct TileInfo *ti;
uint16 *dst;
uint16 col;
int len;
int x;
int y;
int r;
int g;
int b;

	// --

	len	= 0;
	dst	= buf;
	ti	= & cfg->GfxRead_Screen_TileInfoBuffer[tile];

	for( y=0 ; y<ti->H ; y++ )
	{
		for( x=0 ; x<ti->W ; x++ )
		{
			myGetColor( cfg, tile, x, y, & r, & g, & b );

			col  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift   );
			col |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
			col |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift  );

			dst[len++] = SWAP16( col );
		}
	}

	len = len * 2;

	// --

	return( len );
}

// --
// -- 32 Bit

int TileRender_Generic_32BE( struct Config *cfg, APTR buf, int tile )
{
struct TileInfo *ti;
uint32 *dst;
uint32 col;
int len;
int x;
int y;
int r;
int g;
int b;

	// --

	len	= 0;
	dst	= buf;
	ti	= & cfg->GfxRead_Screen_TileInfoBuffer[tile];

	for( y=0 ; y<ti->H ; y++ )
	{
		for( x=0 ; x<ti->W ; x++ )
		{
			myGetColor( cfg, tile, x, y, & r, & g, & b );

			col  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift   );
			col |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
			col |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift  );

			dst[len++] = col;
		}
	}

	len = len * 4;

	// --

	return( len );
}

// --
// -- 32 Bit Little Endian

int TileRender_Generic_32LE( struct Config *cfg, APTR buf, int tile )
{
struct TileInfo *ti;
uint32 *dst;
uint32 col;
int len;
int x;
int y;
int r;
int g;
int b;

	// --

	len	= 0;
	dst	= buf;
	ti	= & cfg->GfxRead_Screen_TileInfoBuffer[tile];

	for( y=0 ; y<ti->H ; y++ )
	{
		for( x=0 ; x<ti->W ; x++ )
		{
			myGetColor( cfg, tile, x, y, & r, & g, & b );

			col  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift   );
			col |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
			col |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift  );

			dst[len++] = SWAP32( col );
		}
	}

	len = len * 4;

	// --

	return( len );
}

// --
