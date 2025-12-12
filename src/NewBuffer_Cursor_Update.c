
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --
// We make sure the Pointer have same Format as Screen
// So simple Convert gfx into correct format

S32 NewBuffer_Cursor_Update( struct Config *cfg )
{
U8 maskdata[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
U32 *data32;
U32 col32;
U16 *data16;
U16 col16;
U8 *chunky;
U8 *data8;
U8 *mask;
U8 col8;
S32 bytewidth;
S32 bytesize;
S32 maskpos;
S32 bufpos;
S32 err;
S32 pos;
S32 rs;
S32 gs;
S32 bs;
S32 pw;
S32 ph;
S32 x;
S32 y;
S32 r;
S32 g;
S32 b;

//	DebugPrintF( "NewBuffer_Cursor_Update\n" );

	// --
	// 

	if (( cfg->cfg_PointerFormatID > 0 ) && ( cfg->cfg_PointerFormatID == cfg->GfxRead_Encode_ActivePixelID ))
	{
		err = FALSE;
		goto bailout;
	}

//	DebugPrintF( "Pointer ID %ld, Gfx ID %ld\n", cfg->cfg_PointerFormatID, cfg->GfxRead_Encode_ActivePixelID );

//	if ( ! memcmp( & cfg->GfxRead_Encode_ActivePixel, & cfg->cfg_PointerFormat, sizeof( struct PixelMessage )))
//	{
//		err = FALSE;
//		goto bailout;
//	}

	err = TRUE;

	// -- Clear Buffers

	memset( cfg->cfg_PointerBuffer, 0, 64*64*4 );
	memset( cfg->cfg_PointerMask,	0, 64*64*1/8 );

	// --

	chunky	= (PTR) cfg->cfg_PointerChunky;
	data32	= (PTR) cfg->cfg_PointerBuffer;
	data16	= (PTR) cfg->cfg_PointerBuffer;
	data8	= (PTR) cfg->cfg_PointerBuffer;
	mask	= (PTR) cfg->cfg_PointerMask;

	pw		= cfg->cfg_PointerWidth;
	ph		= cfg->cfg_PointerHeight;

	bytewidth = ( pw + 7 ) / 8;
	bytesize  = ( cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel + 7 ) / 8;

	// --

	rs = 8;
	y = 1;

	while( y < cfg->GfxRead_Encode_ActivePixel.pm_RedMax )
	{
		y = y << 1;
		rs--;
	}

	// --

	gs = 8;
	y = 1;

	while( y < cfg->GfxRead_Encode_ActivePixel.pm_GreenMax )
	{
		y = y << 1;
		gs--;
	}

	// --

	bs = 8;
	y = 1;

	while( y < cfg->GfxRead_Encode_ActivePixel.pm_BlueMax )
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
				col8  = ( r << cfg->GfxRead_Encode_ActivePixel.pm_RedShift );
				col8 |= ( g << cfg->GfxRead_Encode_ActivePixel.pm_GreenShift );
				col8 |= ( b << cfg->GfxRead_Encode_ActivePixel.pm_BlueShift );

				*data8++ = col8;
				bufpos += 1;
			}
			else if ( bytesize == 2 )
			{
				col16  = ( r << cfg->GfxRead_Encode_ActivePixel.pm_RedShift );
				col16 |= ( g << cfg->GfxRead_Encode_ActivePixel.pm_GreenShift );
				col16 |= ( b << cfg->GfxRead_Encode_ActivePixel.pm_BlueShift );

				if ( cfg->GfxRead_Encode_ActivePixel.pm_BigEndian )
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
				col32  = ( r << cfg->GfxRead_Encode_ActivePixel.pm_RedShift );
				col32 |= ( g << cfg->GfxRead_Encode_ActivePixel.pm_GreenShift );
				col32 |= ( b << cfg->GfxRead_Encode_ActivePixel.pm_BlueShift );

				if ( cfg->GfxRead_Encode_ActivePixel.pm_BigEndian )
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

	memcpy( & cfg->cfg_PointerFormat, & cfg->GfxRead_Encode_ActivePixel, sizeof( struct PixelMessage ));
	cfg->cfg_PointerFormatID = cfg->GfxRead_Encode_ActivePixelID;

	err = FALSE;

bailout:

	return( err );
}

// --
