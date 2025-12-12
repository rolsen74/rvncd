
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#pragma pack(1)

struct RichHeader
{
	// -- Rect Header
	U16	br_XPos;
	U16	br_YPos;
	U16	br_Width;
	U16	br_Height;
	U32	br_Encoding;
};

#pragma pack(0)

// --
// This function send a 'Tile' with the Pointer gfx

S32 NewBuffer_Cursor_Rich( struct Config *cfg )
{
struct RichHeader *header;
U8 *buffer;
U8 *mask;
U8 *gfx;
S32 masksize;
S32 gfxsize;
S32 size;
S32 stat;
// S32 rc;
S32 pw;
S32 ph;
S32 bw;

	// -- 

	stat = US_Error;

	// Check: gfx format should be the same as Client Screen
	if ( NewBuffer_Cursor_Update( cfg ))
	{
		SHELLBUF_PRINTF( "Pointer: Buffer error\n" );
		goto bailout;
	}

	// -- Calc Sizes

	pw = cfg->cfg_PointerWidth;		// Cursor Pixel Width
	ph = cfg->cfg_PointerHeight;	// Cursor Pixel Height
	bw = ( pw + 7 ) / 8;			// Byte Width

	gfxsize  = pw * ph * (( cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel + 7 ) / 8 );
	masksize = bw * ph;

	// -- Validate Buffer Size

	size = sizeof( struct RichHeader ) + gfxsize + masksize;

	if ( size > cfg->NetSend_SendBufferSize )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = mem_ASPrintF( "Pointer: Buffer overflow" );
		}
		goto bailout;
	}

	// --

	buffer	= cfg->NetSend_SendBuffer;
	header	= (PTR) buffer;
	gfx		= & buffer[ sizeof( struct RichHeader ) ];
	mask	= & buffer[ sizeof( struct RichHeader ) + gfxsize ];

	// -- Rect
	header->br_XPos		= 0;
	header->br_YPos		= 0;
	header->br_Width	= pw;
	header->br_Height	= ph;
	header->br_Encoding	= -239; // Cursor

	// -- Gfx
	memcpy( gfx, cfg->cfg_PointerBuffer, cfg->cfg_PointerBufferSize );

	// -- Mask
	memcpy( mask, cfg->cfg_PointerMask, cfg->cfg_PointerMaskSize );

	// --

	if ( Tile_SendBuffer_Send( cfg, cfg->NetSend_SendBuffer, size ))
	{
		stat = US_Error;
		goto bailout;
	}

	// --
	// Not sure this should count
//	cfg->SessionStatus.si_Pixels_Send += pw*ph;

	cfg->SessionStatus.si_Tiles_Total++;
	cfg->SessionStatus.si_Tiles_Rich++;

	stat = US_Okay;			// No wait

bailout:

	return( stat );
}

// --
