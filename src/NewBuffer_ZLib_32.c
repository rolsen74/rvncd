
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#pragma pack(1)

struct ZLIBHeader
{
	// -- Rect Header
	U16	br_XPos;
	U16	br_YPos;
	U16	br_Width;
	U16	br_Height;
	U32	br_Encoding;
	// -- ZLib Header
	U32	bz_Length;
};

#pragma pack(0)

// --

#ifdef HAVE_ZLIB

S32 myEnc_ZLib_32( struct Config *cfg, struct UpdateNode *un, U32 tilenr )
{
struct ZLIBHeader *header;
struct TileInfo *ti;
z_stream *zStream;
S32 datalen;
U32 *ibuf;
U32 *obuf;
S32 size;
S32 stat;
U32 pos;
S32 rx;
S32 ry;
S32 x;
S32 y;
S32 w;
S32 h;

	// --

	datalen = -1;

	// --
	// Get Tile Info
	// adjust so we only update requested Rect

	ti = & cfg->GfxRead_Screen_TileInfoBuffer[tilenr];
	x = ti->X;
	y = ti->Y;
	w = ti->W;
	h = ti->H;

	if ( x < un->un_Data.update.urm_XPos )
	{
		x  = ( un->un_Data.update.urm_XPos );
		w -= ( un->un_Data.update.urm_XPos - ti->X );
	}

	if ( y < un->un_Data.update.urm_YPos )
	{
		y  = ( un->un_Data.update.urm_YPos );
		h -= ( un->un_Data.update.urm_YPos - ti->Y );
	}

	if (( un->un_Data.update.urm_XPos + un->un_Data.update.urm_Width ) < ( ti->X + ti->W ))
	{
		w -= (( ti->X + ti->W ) - ( un->un_Data.update.urm_XPos + un->un_Data.update.urm_Width ));
	}

	if (( un->un_Data.update.urm_YPos + un->un_Data.update.urm_Height ) < ( ti->Y + ti->H ))
	{
		h -= (( ti->Y + ti->H ) - ( un->un_Data.update.urm_YPos + un->un_Data.update.urm_Height ));
	}

	#ifdef DEBUG
	if (( x < 0 )
	||	( y < 0 )
	||	( w <= 0 )
	||	( h <= 0 )
	||	(( x + w ) > ( ti->X + ti->W ))
	||	(( y + h ) > ( ti->Y + ti->H )))
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = mem_ASPrintF( "Internal: Data Length error" );
		}

		SHELLBUF_PRINTF( "raw data error : x %ld : y %ld : w %ld : h %ld :\n", x, y, w, h );
		goto bailout;
	}
	#endif

	// --
	// Pack data, so we only send exactly what is requested
	// Note: NetSend_SendBuffer have 4k extra buffer, so we are okay here

	ibuf = (PTR) cfg->NetSend_EncBuffer;
	obuf = (PTR) & cfg->NetSend_SendBuffer[ 2048 ];
	pos = 0;

	S32 xoff = x - ti->X;
	S32 yoff = y - ti->Y;

	for( ry=0 ; ry<h ; ry++ )
	{
		for( rx=0 ; rx<w ; rx++ )
		{
			obuf[pos++] = ibuf[ (yoff + ry) * ti->W + (xoff + rx) ];
		}
	}

	// --

	zStream = & cfg->cfg_zStream;
	zStream->next_in	= (PTR) obuf;
	zStream->next_out	= (PTR) & cfg->NetSend_SendBuffer[ sizeof( struct ZLIBHeader ) ];
	zStream->avail_in	= pos * sizeof( U32 );
	zStream->avail_out	= cfg->NetSend_SendBufferSize;

	stat = Deflate( zStream, Z_SYNC_FLUSH );

	if ( stat != Z_OK )
	{
		SHELLBUF_PRINTF( "zlib deflate error\n" );
		goto bailout;
	}

	size = cfg->NetSend_SendBufferSize - zStream->avail_out;

	if ( cfg->NetSend_EncBufferUsed < size )
	{
		// Not an error, just return -1 and try next Encoding
		#ifdef DEBUG
		SHELLBUF_PRINTF( "zlib buffer became larger, than RAW data\n" );
		#endif
		goto bailout;
	}

	// --

	header = (PTR) cfg->NetSend_SendBuffer;
	header->br_XPos		= x;
	header->br_YPos		= y;
	header->br_Width	= w;
	header->br_Height	= h;
	header->br_Encoding	= 6; // ZLib
	header->bz_Length	= size;

	// --

	datalen	 = sizeof( struct ZLIBHeader );
	datalen += size;

	cfg->SessionStatus.si_Pixels_Send += w*h;
	cfg->SessionStatus.si_Tiles_Total++;
	cfg->SessionStatus.si_Tiles_ZLib++;

	// --

bailout:

	return( datalen );
}

#endif // HAVE_ZLIB

// --
