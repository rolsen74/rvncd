 
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

#pragma pack(1)

struct BufferRect
{
	uint16	br_XPos;
	uint16	br_YPos;
	uint16	br_Width;
	uint16	br_Height;
	uint32	br_Encoding;
};

#pragma pack(0)

// --

int myEnc_Raw( struct Config *cfg, int tile )
{
struct BufferRect *rect;
struct TileInfo *ti;
uint8 *data;
int datalen;
int len;

	len = -1;

	ti = & cfg->GfxRead_Screen_TileInfoBuffer[tile];

	rect = cfg->NetSend_SendBuffer;
	data = cfg->NetSend_SendBuffer;

// IExec->DebugPrintF( "myEnc_Raw - %p - %dx%d %dx%d\n", data, ti->X, ti->Y, ti->W, ti->H );

	rect->br_XPos		= ti->X;
	rect->br_YPos		= ti->Y;
	rect->br_Width		= ti->W;
	rect->br_Height		= ti->H;
	rect->br_Encoding	= 0; // Raw

	if ( cfg->GfxRead_Encode_RenderTile )
	{
		IExec->ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

		datalen = cfg->GfxRead_Encode_RenderTile( cfg, & data[ sizeof( struct BufferRect ) ], tile );

		if ( datalen <= 0 )
		{
			printf( "RAW: GfxRead_Encode_RenderTile error\n" );
		}

		IExec->ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );
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

//	if ( cfg->GfxRead_Encode_RenderTile )
//	{
//		len = cfg->GfxRead_Encode_RenderTile( cfg, data, tile );
//	}
//	else
//	{
//		len = cfg->Enc_BufferRender( cfg, data, ti->X, ti->Y, ti->W, ti->H );
//	}

	if ( datalen <= 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Internal: Data Length error" );
		}

		printf( "raw data error\n" );
		goto bailout;
	}

	len  = datalen;
	len += sizeof( struct BufferRect ); 

	cfg->SessionStatus.si_Tiles_Raw++;
	cfg->SessionStatus.si_Tiles_Total++;

bailout:

// printf( "Raw: DataLen: %d, Header %d, Total %d\n", datalen, sizeof( struct BufferRect ), len );

// IExec->DebugPrintF( "myEnc_Raw 2 - %ld\n", len );

	return( len );
}

// --
