
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#pragma pack(1)

struct RREHeader
{
	// -- Rect Header
	U16	br_XPos;
	U16	br_YPos;
	U16	br_Width;
	U16	br_Height;
	U32	br_Encoding;
	// -- RRE Header
	U32	Rects;
	U16 BGColor;
};

struct RRERect
{
	U16 color;
	U16 x;
	U16 y;
	U16 w;
	U16	h;
};

#pragma pack(0)

// --

static U16 find_background_color( struct Config *cfg ) 
{
U16 *data;
U16 col;

	/* Simple choice: top-left pixel. 
		Optional: use a small histogram to find dominant color */

	data = (PTR) cfg->NetSend_EncBuffer;

	col = data[0];

	return( col );
}

// --

S32 myEnc_RRE_16( struct Config *cfg, struct UpdateNode *un, U32 tilenr )
{
struct RREHeader *header;
struct RRERect *rects;
struct RRERect *rect;
struct TileInfo *ti;
U16 *colbuf;
S32 datalen;
U32 rectcnt;
U32 rectmax;
U16 bgcol;
U16 col;
S32 run;
S32 rx;
S32 ry;
S32 x;
S32 y;
S32 w;
S32 h;

//	DebugPrintF( "myEnc_RRE_16\n" );

	// Default error
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

	// Setup output buffers
	header	= (PTR) & cfg->NetSend_SendBuffer[ 0 ];
	rects	= (PTR) & cfg->NetSend_SendBuffer[ sizeof( struct RREHeader ) ];

	// 
	colbuf	= (PTR) cfg->NetSend_EncBuffer;
	bgcol	= find_background_color( cfg );

	//  calc size when it becomes bigger than RAW Data
	rectmax	= ((( w * h * sizeof( U16 )) - sizeof( struct RREHeader )) / sizeof( struct RRERect ));
	rectcnt = 0;

	for( ry=0 ; ry<h ; ry++ )
	{
		for( rx=0 ; rx<w ; )
		{
			col = colbuf[ ry * ti->W + rx ];

			if ( col != bgcol )
			{
	 			if ( rectcnt >= rectmax )
	 			{
	 				// Not an error, just return -1 and try next Encoding
	 				goto bailout;
	 			}

				run = 1;

				while(( rx + run < w ) && ( colbuf[ ry * ti->W + rx + run ] == col ))
				{
					run++;
				}

				rect = & rects[ rectcnt++ ];
				rect->color = col;
				rect->x = rx;
				rect->y = ry;
				rect->w = run;
				rect->h = 1;
				rx += run;
			}
			else
			{
				rx++;
			}
		}
	}

	// Zero rects is valid, if all colors is the same a BG color
	header->br_XPos		= x;
	header->br_YPos		= y;
	header->br_Width	= w;
	header->br_Height	= h;
	header->br_Encoding	= 2; // RRE
	header->BGColor		= bgcol;
	header->Rects		= rectcnt;

	// --

	datalen  = sizeof( struct RREHeader );
	datalen += sizeof( struct RRERect ) * rectcnt;

	cfg->SessionStatus.si_Pixels_Send += w*h;
	cfg->SessionStatus.si_Tiles_Total++;
	cfg->SessionStatus.si_Tiles_RRE++;

	// --

bailout:

	return( datalen );
}

// --
