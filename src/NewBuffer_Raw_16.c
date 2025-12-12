
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#pragma pack(1)

struct RAWHeader
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

S32 myEnc_Raw_16( struct Config *cfg, struct UpdateNode *un, U32 tilenr )
{
struct RAWHeader *header;
struct TileInfo *ti;
S32 datalen;
U16 *ibuf;
U16 *obuf;
U32 pos;
S32 rx;
S32 ry;
S32 x;
S32 y;
S32 w;
S32 h;

//	SHELLBUF_PRINTF( "myEnc_Raw_16 : TileNr #%ld\n", tilenr );

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

	header = (PTR) cfg->NetSend_SendBuffer;
	ibuf = (PTR) cfg->NetSend_EncBuffer;
	obuf = (PTR) & cfg->NetSend_SendBuffer[ sizeof( struct RAWHeader ) ];
	pos  = 0;

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

	header->br_XPos		= x;
	header->br_YPos		= y;
	header->br_Width	= w;
	header->br_Height	= h;
	header->br_Encoding	= 0; // Raw

	datalen  = sizeof( struct RAWHeader );
	datalen += pos * sizeof( U16 );

	cfg->SessionStatus.si_Pixels_Send += w*h;
	cfg->SessionStatus.si_Tiles_Total++;
	cfg->SessionStatus.si_Tiles_Raw++;

	// --

	#ifdef DEBUG
bailout:
	#endif

	return( datalen );
}

// --
