
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#pragma pack(1)

struct LastRectHeader
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

S32 NewBuffer_LastRect( struct Config *cfg )
{
struct LastRectHeader *header;
S32 stat;

	stat = US_Error;

	// -- Rect
	header = (PTR) cfg->NetSend_SendBuffer;
	header->br_XPos		= 0;
	header->br_YPos		= 0;
	header->br_Width	= 0;
	header->br_Height	= 0;
	header->br_Encoding	= -224; // Last Rect

	// --

	if ( Tile_SendBuffer_Send( cfg, cfg->NetSend_SendBuffer, sizeof( struct LastRectHeader )))
	{
		stat = US_Error;
		goto bailout;
	}

	stat = US_Okay;

bailout:

	return( stat );
}

// --
