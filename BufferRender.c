 
/*
 * Copyright (c) 2023-2024 Rene W. Olsen < renewolsen @ gmail . com >
 *
 * This software is released under the GNU General Public License, version 3.
 * For the full text of the license, please visit:
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * You can also find a copy of the license in the LICENSE file included with this software.
 */

// --

#include "RVNCd.h"

// --

/*
**
** This file contains functions that Render
** into Client's expected format.
**
*/

int BufferRender_Generic_8(		struct Config *cfg, APTR buf, int xx, int yy, int ww, int hh );
int BufferRender_Generic_16BE(	struct Config *cfg, APTR buf, int xx, int yy, int ww, int hh );
int BufferRender_Generic_16LE(	struct Config *cfg, APTR buf, int xx, int yy, int ww, int hh );
int BufferRender_Generic_32BE(	struct Config *cfg, APTR buf, int xx, int yy, int ww, int hh );
int BufferRender_Generic_32LE(	struct Config *cfg, APTR buf, int xx, int yy, int ww, int hh );
int BufferRender_RGB565PC(		struct Config *cfg, APTR buf, int xx, int yy, int ww, int hh );

// -- Misc

static void myGetColor( struct Config *cfg, int xx, int yy, int *r, int *g, int *b )
{
uint8 *buf;
int offset;
int tile;
int x1;
int x2;
int y1;
int y2;
int ts;
int tb;

	ts		= cfg->GfxRead_TileSize;
	tb		= ts * ts * 4; // We read from 32bit ARGB 

	x1		= ( xx % ts );
	y1		= ( yy % ts );

	x2		= ( xx / ts );
	y2		= ( yy / ts );

	tile	= ( y2 * cfg->GfxRead_TileWidth ) + x2;

	offset	= ( tile * tb );
	offset += ( y1 * ts * 4 );
	offset += ( x1 * 4 );

	buf		= & cfg->GfxRead_ScreenChunkyBuffer[ offset ];

	*r		= buf[1];
	*g		= buf[2];
	*b		= buf[3];
}

// --


// -- RGB 565 PC

#if 1
int BufferRender_RGB565PC( struct Config *cfg, APTR data, int xx, int yy, int ww, int hh )
{
uint16 *pxl;
uint16 *buf;
int len;
int x;
int y;
//int w;
//int h;

int offset;
int tile;
int x1;
int x2;
int y1;
int y2;
int ts;
int tb;

#define PixelSize	2

	len = 0;
//	buf = (APTR) cfg->Enc_Buffer;
	buf = data;

	ts	= cfg->GfxRead_TileSize;
	tb	= ts * ts * PixelSize;

	for( y=yy ; y<yy+hh ; y++ )
	{
		for( x=xx ; x<xx+ww ; x++ )
		{
			x1		= ( x % ts );
			y1		= ( y % ts );

			x2		= ( x / ts );
			y2		= ( y / ts );

			tile	= ( y2 * cfg->GfxRead_TileWidth ) + x2;

			offset	= ( tile * tb );
			offset += ( y1 * ts * PixelSize );
			offset += ( x1 * PixelSize );

			pxl = (APTR)( & cfg->GfxRead_ScreenChunkyBuffer[ offset ] );

			buf[len++] = *pxl;
		}
	}

	len = len * PixelSize;

#undef PixelSize

	return( len );
}
#endif

// -- RGB 888 PC

#if 1
int BufferRender_RGB888PC( struct Config *cfg, APTR data, int xx, int yy, int ww, int hh )
{
uint32 *pxl;
uint32 *buf;
int len;
int x;
int y;
//int w;
//int h;

int offset;
int tile;
int x1;
int x2;
int y1;
int y2;
int ts;
int tb;

#define PixelSize	4

	len = 0;
//	buf = (APTR) cfg->Enc_Buffer;
	buf = data;

	ts	= cfg->GfxRead_TileSize;
	tb	= ts * ts * PixelSize;

	for( y=yy ; y<yy+hh ; y++ )
	{
		for( x=xx ; x<xx+ww ; x++ )
		{
			x1		= ( x % ts );
			y1		= ( y % ts );

			x2		= ( x / ts );
			y2		= ( y / ts );

			tile	= ( y2 * cfg->GfxRead_TileWidth ) + x2;

			offset	= ( tile * tb );
			offset += ( y1 * ts * PixelSize );
			offset += ( x1 * PixelSize );

			pxl = (APTR)( & cfg->GfxRead_ScreenChunkyBuffer[ offset ] );

			buf[len++] = *pxl;
		}
	}

	len = len * PixelSize;

#undef PixelSize

	return( len );
}
#endif

int BufferRender_RGB888( struct Config *cfg, APTR data, int xx, int yy, int ww, int hh )
{
uint32 *pxl;
uint32 *buf;
int len;
int x;
int y;
//int w;
//int h;

int offset;
int tile;
int x1;
int x2;
int y1;
int y2;
int ts;
int tb;

#define PixelSize	4

	len = 0;
//	buf = (APTR) cfg->Enc_Buffer;
	buf = data;

	ts	= cfg->GfxRead_TileSize;
	tb	= ts * ts * PixelSize;

	for( y=yy ; y<yy+hh ; y++ )
	{
		for( x=xx ; x<xx+ww ; x++ )
		{
			x1		= ( x % ts );
			y1		= ( y % ts );

			x2		= ( x / ts );
			y2		= ( y / ts );

			tile	= ( y2 * cfg->GfxRead_TileWidth ) + x2;

			offset	= ( tile * tb );
			offset += ( y1 * ts * PixelSize );
			offset += ( x1 * PixelSize );

			pxl = (APTR)( & cfg->GfxRead_ScreenChunkyBuffer[ offset ] );

			buf[len++] = *pxl;
		}
	}

	len = len * PixelSize;

#undef PixelSize

	return( len );
}

// -- Generic 8 Bit ( 1 Byte pr. Pixel )

int BufferRender_Generic_8( struct Config *cfg, APTR data, int xx, int yy, int ww, int hh )
{
uint8 *buf;
uint8 col;
int len;
int x;
int y;
int r;
int g;
int b;

// IExec->DebugPrintF( "BufferRender_Generic_8 : %dx%d %dx%d\n", xx, yy, ww, hh );

	len = 0;

	if ( ! cfg->NetSend_ClientPixelFormatSet )
	{
printf( "%s%04d: Client Pixel Format not Set\n", __FILE__, __LINE__ );
		goto bailout;
	}

//	buf = (APTR) cfg->Enc_Buffer;
	buf = data;

	for( y=yy ; y<yy+hh ; y++ )
	{
		for( x=xx ; x<xx+ww ; x++ )
		{
			myGetColor( cfg, x, y, & r, & g, & b );

			r >>= cfg->Enc_RedShift;
			g >>= cfg->Enc_GreenShift;
			b >>= cfg->Enc_BlueShift;

			col  = ( r << cfg->NetSend_ClientPixelFormat.pm_RedShift );
			col |= ( g << cfg->NetSend_ClientPixelFormat.pm_GreenShift );
			col |= ( b << cfg->NetSend_ClientPixelFormat.pm_BlueShift );

			buf[len++] = col;
		}
	}

bailout:

	return( len );
}

// -- Generic 16 Bit - Big Endian ( 2 Bytes pr. Pixel )

int BufferRender_Generic_16BE( struct Config *cfg, APTR data, int xx, int yy, int ww, int hh )
{
uint16 *buf;
uint16 col;
int len;
int x;
int y;
int r;
int g;
int b;

// IExec->DebugPrintF( "BufferRender_Generic_16BE : %dx%d %dx%d\n", xx, yy, ww, hh );

	len = 0;

	if ( ! cfg->NetSend_ClientPixelFormatSet )
	{
printf( "%s%04d: Client Pixel Format not Set\n", __FILE__, __LINE__ );
		goto bailout;
	}

//	buf = (APTR) cfg->Enc_Buffer;
	buf = data;

	for( y=yy ; y<yy+hh ; y++ )
	{
		for( x=xx ; x<xx+ww ; x++ )
		{
			myGetColor( cfg, x, y, & r, & g, & b );

			r >>= cfg->Enc_RedShift;
			g >>= cfg->Enc_GreenShift;
			b >>= cfg->Enc_BlueShift;

			col  = ( r << cfg->NetSend_ClientPixelFormat.pm_RedShift );
			col |= ( g << cfg->NetSend_ClientPixelFormat.pm_GreenShift );
			col |= ( b << cfg->NetSend_ClientPixelFormat.pm_BlueShift );

			buf[len++] = col;
		}
	}

	len = len * 2;

bailout:

	return( len );
}

// -- Generic 16 Bit - Little Endian ( 2 Bytes pr. Pixel )

int BufferRender_Generic_16LE( struct Config *cfg, APTR data, int xx, int yy, int ww, int hh )
{
uint16 *buf;
uint16 col;
int len;
int x;
int y;
int r;
int g;
int b;

// IExec->DebugPrintF( "BufferRender_Generic_16LE : %dx%d %dx%d\n", xx, yy, ww, hh );

	len = 0;

	if ( ! cfg->NetSend_ClientPixelFormatSet )
	{
printf( "%s%04d: Client Pixel Format not Set\n", __FILE__, __LINE__ );
		goto bailout;
	}

//	buf = (APTR) cfg->Enc_Buffer;
	buf = data;

	for( y=yy ; y<yy+hh ; y++ )
	{
		for( x=xx ; x<xx+ww ; x++ )
		{
			myGetColor( cfg, x, y, & r, & g, & b );

			r >>= cfg->Enc_RedShift;
			g >>= cfg->Enc_GreenShift;
			b >>= cfg->Enc_BlueShift;

			col  = ( r << cfg->NetSend_ClientPixelFormat.pm_RedShift );
			col |= ( g << cfg->NetSend_ClientPixelFormat.pm_GreenShift );
			col |= ( b << cfg->NetSend_ClientPixelFormat.pm_BlueShift );

			buf[len++] = SWAP16( col );
		}
	}

	len = len * 2;

bailout:

	return( len );
}

// -- Generic 32 Bit - Big Endian ( 4 Bytes pr. Pixel )

int BufferRender_Generic_32BE( struct Config *cfg, APTR data, int xx, int yy, int ww, int hh )
{
uint32 *buf;
uint32 col;
int len;
int x;
int y;
int r;
int g;
int b;

// IExec->DebugPrintF( "BufferRender_Generic_32BE : %dx%d %dx%d\n", xx, yy, ww, hh );

	len = 0;

	if ( ! cfg->NetSend_ClientPixelFormatSet )
	{
printf( "%s%04d: Client Pixel Format not Set\n", __FILE__, __LINE__ );
		goto bailout;
	}

//	buf = (APTR) cfg->Enc_Buffer;
	buf = data;

	for( y=yy ; y<yy+hh ; y++ )
	{
		for( x=xx ; x<xx+ww ; x++ )
		{
			myGetColor( cfg, x, y, & r, & g, & b );

			r >>= cfg->Enc_RedShift;
			g >>= cfg->Enc_GreenShift;
			b >>= cfg->Enc_BlueShift;

			col  = ( r << cfg->NetSend_ClientPixelFormat.pm_RedShift );
			col |= ( g << cfg->NetSend_ClientPixelFormat.pm_GreenShift );
			col |= ( b << cfg->NetSend_ClientPixelFormat.pm_BlueShift );

			buf[len++] = col;
		}
	}

	len = len * 4;

bailout:

	return( len );
}

// -- Generic 32 Bit - Little Endian ( 4 Bytes pr. Pixel )

int BufferRender_Generic_32LE( struct Config *cfg, APTR data, int xx, int yy, int ww, int hh )
{
uint32 *buf;
uint32 col;
int len;
int x;
int y;
int r;
int g;
int b;

// IExec->DebugPrintF( "BufferRender_Generic_32LE : %dx%d %dx%d\n", xx, yy, ww, hh );

	len = 0;

	if ( ! cfg->NetSend_ClientPixelFormatSet )
	{
printf( "%s%04d: Client Pixel Format not Set\n", __FILE__, __LINE__ );
		goto bailout;
	}

//	buf = (APTR) cfg->Enc_Buffer;
	buf = data;

	for( y=yy ; y<yy+hh ; y++ )
	{
		for( x=xx ; x<xx+ww ; x++ )
		{
			myGetColor( cfg, x, y, & r, & g, & b );

			r >>= cfg->Enc_RedShift;
			g >>= cfg->Enc_GreenShift;
			b >>= cfg->Enc_BlueShift;

			col  = ( r << cfg->NetSend_ClientPixelFormat.pm_RedShift );
			col |= ( g << cfg->NetSend_ClientPixelFormat.pm_GreenShift );
			col |= ( b << cfg->NetSend_ClientPixelFormat.pm_BlueShift );

			buf[len++] = SWAP32( col );
		}
	}

	len = len * 4;

bailout:

	return( len );
}

// --
