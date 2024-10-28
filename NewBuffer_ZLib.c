
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

#if 0

struct GfxRectZLib
{
	// -- Rect Header
	uint16	br_XPos;
	uint16	br_YPos;
	uint16	br_Width;
	uint16	br_Height;
	uint32	br_Encoding;
	// -- ZLib Header
	uint32	bz_Length;
	// -- ZLib Data
};

#endif

// --

int myEnc_ZLib( struct Config *cfg, struct UpdateNode *un UNUSED, int tilenr, int hardcursor )
{
struct GfxRectZLib *rect;
struct TileInfo *ti;
z_stream *zStream;
uint8 *data;
int bufsize;
int datalen;
int stat;
int len;

	// --

	len  = -1; // Error
	rect = cfg->NetSend_SendBuffer;
	data = cfg->NetSend_SendBuffer;

	memset( data, 0xac, cfg->NetSend_SendBufferSize );

	// -- Get Raw Graphics Data

	if ( cfg->GfxRead_Encode_RenderTile )
	{
		IExec->ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

		// The SendBuffer have 4096 extra bytes .. so placing it 1000 in, is no problem
		datalen = cfg->GfxRead_Encode_RenderTile( cfg, & data[ 1000 ], tilenr );

		if ( datalen <= 0 )
		{
			printf( "ZLIB: GfxRead_Encode_RenderTile error\n" );
		}

		IExec->ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );

		if ( ! hardcursor )
		{
			NewBuffer_AddCursor( cfg, & data[ 1000 ], tilenr );
		}
	}
	else
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Internal: TileRender not set" );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "TileRender not set" );
		datalen = -1;
	}

	if ( datalen <= 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Internal: zlib Data error" );
		}

		printf( "zlib Data error\n" );
		goto bailout;
	}

	// -- ZLib - Pack data

	bufsize = cfg->NetSend_SendBufferSize - 1000;

	zStream = & cfg->cfg_zStream;

	zStream->next_in	= & data[ 1000 ];
	zStream->next_out	= & data[ sizeof( struct GfxRectZLib ) ];
	zStream->avail_in	= datalen;
	zStream->avail_out	= bufsize;

	stat = IZ->Deflate( zStream, Z_SYNC_FLUSH );

	if ( stat != Z_OK )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Internal: zlib deflate error" );
		}

		printf( "zlib deflate error\n" );
		goto bailout;
	}

	// --

	ti = & cfg->GfxRead_Screen_TileInfoBuffer[ tilenr ];

	rect->br_XPos		= ti->X;
	rect->br_YPos		= ti->Y;
	rect->br_Width		= ti->W;
	rect->br_Height		= ti->H;
	rect->br_Encoding	= 6; // ZLib
	rect->bz_Length		= bufsize - zStream->avail_out;

	// --

	len  = sizeof( struct GfxRectZLib ); 
	len += rect->bz_Length;

	cfg->SessionStatus.si_Tiles_ZLib++;
	cfg->SessionStatus.si_Tiles_Total++;

bailout:

	return( len );
}

// --
