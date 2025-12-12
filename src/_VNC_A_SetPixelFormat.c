
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if 0

struct PixelMessage
{
	U8	pm_Type;
	U8	pm_Pad;
	U8	pm_Pad2;
	U8	pm_Pad3;
	U8	pm_BitsPerPixel;
	U8	pm_Depth;
	U8	pm_BigEndian;
	U8	pm_TrueColor;
	U16	pm_RedMax;
	U16	pm_GreenMax;
	U16	pm_BlueMax;
	U8	pm_RedShift;
	U8	pm_GreenShift;
	U8	pm_BlueShift;
	U8	pm_Pad4;
	U8	pm_Pad5;
	U8	pm_Pad6;
};

#endif

// --

S32 VNC_SetPixelFormat( struct Config *cfg )
{
struct PixelMessage *buf;
//struct PixelMessage *pm;
struct UpdateNode *un;
S32 error;
S32 size;
S32 rc;

	error = TRUE;

	buf = cfg->NetRead_ReadBuffer;

	size = sizeof( struct PixelMessage );

	rc = Func_NetRead( cfg, buf, size, MSG_WAITALL );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	if (( buf->pm_Type != 0 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%ld != %ld)", rc, size );
		goto bailout;
	}

	ObtainSemaphore( & cfg->Server_UpdateSema );

	un = List_RemHead( & cfg->Server_UpdateFree );

	if ( ! un )
	{
		un = mem_AllocClr( sizeof( struct UpdateNode ) );

		if ( ! un )
		{
			SHELLBUF_PRINTF( "Yikes Calloc failed\n" );
		}
	}

	if ( un )
	{
//		SHELLBUF_PRINTF( "New Pixel Msg\n" );
		un->un_Type = UT_PixelMessage;
		memcpy( & un->un_Data.pixel, buf, sizeof( struct PixelMessage ));
		List_AddTail( & cfg->Server_UpdateList, & un->un_Node );
	}

	ReleaseSemaphore( & cfg->Server_UpdateSema );

	// --

	error = FALSE;

bailout:

	return( error );
}

// --
