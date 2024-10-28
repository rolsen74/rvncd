
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

#if 0

struct GfxRectBuffer
{
	uint16	br_XPos;
	uint16	br_YPos;
	uint16	br_Width;
	uint16	br_Height;
	uint32	br_Encoding;
};

#endif

// --

static int myUpdate_PointerBuffer( struct Config *cfg )
{
uint8 maskdata[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
uint32 *data32;
uint32 col32;
uint16 *data16;
uint16 col16;
uint8 *chunky;
uint8 *data8;
uint8 *mask;
uint8 col8;
int bytewidth;
int bytesize;
int maskpos;
int bufpos;
int err;
int pos;
int rs;
int gs;
int bs;
int pw;
int ph;
int x;
int y;
int r;
int g;
int b;

	// --

	if ( ! memcmp( & cfg->GfxRead_Enocde_ActivePixel, & cfg->cfg_PointerFormat, sizeof( struct PixelMessage )))
	{
		err = FALSE;
		goto bailout;
	}

	err = TRUE;

	// -- Clear Buffers

	memset( cfg->cfg_PointerBuffer, 0, 64*64*4 );
	memset( cfg->cfg_PointerMask,	0, 64*64*1/8 );

	// --

	chunky	= (APTR) cfg->cfg_PointerChunky;
	data32	= (APTR) cfg->cfg_PointerBuffer;
	data16	= (APTR) cfg->cfg_PointerBuffer;
	data8	= (APTR) cfg->cfg_PointerBuffer;
	mask	= (APTR) cfg->cfg_PointerMask;

	pw		= cfg->cfg_PointerWidth;
	ph		= cfg->cfg_PointerHeight;

	bytewidth = ( pw + 7 ) / 8;
	bytesize  = ( cfg->GfxRead_Enocde_ActivePixel.pm_BitsPerPixel + 7 ) / 8;

	// --

	rs = 8;
	y = 1;

	while( y < cfg->GfxRead_Enocde_ActivePixel.pm_RedMax )
	{
		y = y << 1;
		rs--;
	}

	// --

	gs = 8;
	y = 1;

	while( y < cfg->GfxRead_Enocde_ActivePixel.pm_GreenMax )
	{
		y = y << 1;
		gs--;
	}

	// --

	bs = 8;
	y = 1;

	while( y < cfg->GfxRead_Enocde_ActivePixel.pm_BlueMax )
	{
		y = y << 1;
		bs--;
	}

	// --

	bufpos = 0;

	for( y=0 ; y<ph ; y++ )
	{
		for( x=0 ; x<pw ; x++ )
		{
			pos = (( y * 64 * 4 ) + ( x * 4 ));

			//-- 

			if ( chunky[ pos + 0 ] > 25 ) // Check Alpha Channel
			{
				maskpos = ( bytewidth * y ) + ( x / 8 );

				mask[maskpos] |= maskdata[ x & 7 ];
			}

			// --

			r = chunky[ pos + 1 ] >> rs;
			g = chunky[ pos + 2 ] >> gs;
			b = chunky[ pos + 3 ] >> bs;

			/**/ if ( bytesize == 1 )
			{
				col8  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift );
				col8 |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
				col8 |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift );

				*data8++ = col8;
				bufpos += 1;
			}
			else if ( bytesize == 2 )
			{
				col16  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift );
				col16 |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
				col16 |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift );

				if ( cfg->GfxRead_Enocde_ActivePixel.pm_BigEndian )
				{
					*data16++ = col16;
				}
				else
				{
					*data16++ = SWAP16( col16 );
				}

				bufpos += 2;
			}
			else if ( bytesize == 4 )
			{
				col32  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift );
				col32 |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
				col32 |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift );

				if ( cfg->GfxRead_Enocde_ActivePixel.pm_BigEndian )
				{
					*data32++ = col32;
				}
				else
				{
					*data32++ = SWAP32( col32 );
				}

				bufpos += 4;
			}
		}
	}

	// --

	cfg->cfg_PointerBufferSize	= bufpos;
	cfg->cfg_PointerBufferMod	= bytewidth * bytesize;
	cfg->cfg_PointerMaskSize	= (( pw + 7 ) / 8 ) * ph;
	cfg->cfg_PointerMaskMod		= bytewidth;

	// --

	memcpy( & cfg->cfg_PointerFormat, & cfg->GfxRead_Enocde_ActivePixel, sizeof( struct PixelMessage ));

	err = FALSE;

bailout:

	if ( ! err )
	{
		cfg->GfxRead_Enocde_ActivePixelID = cfg->cfg_PointerFormatID;
	}

	return( err );
}

// --
// This function send a 'Tile' with the Pointer gfx
// gfx format should be the same as Client Screen

int NewBuffer_Cursor( struct Config *cfg )
{
struct GfxRectBuffer *rect;
uint8 *buffer;
uint8 *mask;
uint8 *gfx;
int masksize;
int gfxsize;
int size;
int stat;
int rc;
int pw;
int ph;
int bw;

	// -- 

	stat = UPDATESTAT_Error;

	if ( cfg->cfg_PointerFormatID != cfg->GfxRead_Enocde_ActivePixelID )
	{
		if ( myUpdate_PointerBuffer( cfg ))
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Pointer: Buffer error" );
			}
			goto bailout;
		}
	}

	// -- Calc Sizes

	pw = cfg->cfg_PointerWidth;		// Cursor Pixel Width
	ph = cfg->cfg_PointerHeight;	// Cursor Pixel Height
	bw = ( pw + 7 ) / 8;			// Byte Width

	gfxsize  = pw * ph * (( cfg->GfxRead_Enocde_ActivePixel.pm_BitsPerPixel + 7 ) / 8 );
	masksize = bw * ph;

	// -- Validate Buffer Size

	size = sizeof( struct GfxRectBuffer ) + gfxsize + masksize;

	if ( size > cfg->NetSend_SendBufferSize )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Pointer: Buffer overflow" );
		}
		goto bailout;
	}

	// --

	buffer	= cfg->NetSend_SendBuffer;
	rect	= (APTR) buffer;
	gfx		= & buffer[ sizeof( struct GfxRectBuffer ) ];
	mask	= & buffer[ sizeof( struct GfxRectBuffer ) + gfxsize ];

	// -- Rect
	rect->br_XPos		= 0;
	rect->br_YPos		= 0;
	rect->br_Width		= pw;
	rect->br_Height		= ph;
	rect->br_Encoding	= -239; // Cursor

	// -- Gfx
	memcpy( gfx, cfg->cfg_PointerBuffer, cfg->cfg_PointerBufferSize );

	// -- Mask
	memcpy( mask, cfg->cfg_PointerMask, cfg->cfg_PointerMaskSize );

	// --
	rc = myNetSend( cfg, cfg->NetSend_SendBuffer, size );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	// --

	stat = UPDATESTAT_Okay;			// No wait

bailout:

	return( stat );
}

// --

void NewBuffer_AddCursor( struct Config *cfg, uint8 *data, int tile )
{
struct TileInfo *ti;
uint8 *maskbuf;
int maskpos2;
int maskpos;
int delta;
int tpos2;
int tpos;
int mpos2;
int mpos;
int yy;
int xx;
int ww;
int hh;
int tx;
int ty;
int tw;
int th;
int mx;
int my;
int mw;
int mh;

	ti = & cfg->GfxRead_Screen_TileInfoBuffer[ tile ];
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

	// -- Update Gfx
	// Make sure Pointer is same format

	if ( cfg->cfg_PointerFormatID != cfg->GfxRead_Enocde_ActivePixelID )
	{
		if ( myUpdate_PointerBuffer( cfg ))
		{
			printf( "Pointer: Buffer error\n" );
			goto bailout;
		}
	}

	// --
	// Pointer Info

	maskbuf = cfg->cfg_PointerMask2;
	maskpos = 0;
	mpos	= 0;
	tpos	= 0;

	// -- Width

	/**/ if ( mx < tx )
	{
		delta	= tx - mx;
		mpos	+= delta;
		ww		= mw - delta;
		maskpos += delta;
	}
	else // ( mx > tx )
	{
		delta	= mx - tx;
		tpos	+= delta;
		ww		= mw;
	}

	if ( tx + tw < mx + ww )
	{
		ww = tx + tw - mx;
	}

	// -- Height

	/**/ if ( my < ty )
	{
		delta	= ty - my;
		mpos	+= delta * cfg->cfg_PointerWidth;
		hh		= mh - delta;
		maskpos += delta * 64;
	}
	else // ( my > ty )
	{
		delta	= my - ty;
		tpos	+= delta * cfg->GfxRead_Screen_TileSize;
		hh		= mh;
	}

	if ( ty + th < my + hh )
	{
		hh = ty + th - my;
	}

	// --

	/**/ if ( cfg->GfxRead_Encode_FormatSize == 1 )
	{
		uint8 *pb = (APTR) cfg->cfg_PointerBuffer;
		uint8 *tb = (APTR) data;		// Tile Buf

		for( yy=0 ; yy < hh ; yy++ )
		{
			maskpos2 = maskpos;
			tpos2 = tpos;
			mpos2 = mpos;

			for( xx=0 ; xx < ww ; xx++ )
			{
				if ( maskbuf[maskpos2] )
				{
					tb[tpos2] = pb[mpos2];
				}

				maskpos2++;
				tpos2++;
				mpos2++;
			}

			maskpos += 64;
			tpos += cfg->GfxRead_Screen_TileSize;
			mpos += cfg->cfg_PointerWidth;
		}
	}

	// --

	else if ( cfg->GfxRead_Encode_FormatSize == 2 )
	{
		uint16 *pb = (APTR) cfg->cfg_PointerBuffer;
		uint16 *tb = (APTR) data;		// Tile Buf

		for( yy=0 ; yy < hh ; yy++ )
		{
			maskpos2 = maskpos;
			tpos2 = tpos;
			mpos2 = mpos;

			for( xx=0 ; xx < ww ; xx++ )
			{
				if ( maskbuf[maskpos2] )
				{
					tb[tpos2] = pb[mpos2];
				}

				maskpos2++;
				tpos2++;
				mpos2++;
			}

			maskpos += 64;
			tpos += cfg->GfxRead_Screen_TileSize;
			mpos += cfg->cfg_PointerWidth;
		}
	}

	// --

	else if ( cfg->GfxRead_Encode_FormatSize == 4 )
	{
		uint32 *pb = (APTR) cfg->cfg_PointerBuffer;
		uint32 *tb = (APTR) data;		// Tile Buf

		for( yy=0 ; yy < hh ; yy++ )
		{
			maskpos2 = maskpos;
			tpos2 = tpos;
			mpos2 = mpos;

			for( xx=0 ; xx < ww ; xx++ )
			{
				if ( maskbuf[maskpos2] )
				{
					tb[tpos2] = pb[mpos2];
				}

				maskpos2++;
				tpos2++;
				mpos2++;
			}

			maskpos += 64;
			tpos += cfg->GfxRead_Screen_TileSize;
			mpos += cfg->cfg_PointerWidth;
		}
	}

	// --

	else
	{
		printf( "Pointer: FormatSize error\n" );
		goto bailout;
	}

	// --

bailout:

	return;
}

// --
