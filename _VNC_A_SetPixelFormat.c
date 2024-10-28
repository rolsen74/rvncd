
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

#if 0

struct PixelMessage
{
	uint8	pm_Type;
	uint8	pm_Pad;
	uint8	pm_Pad2;
	uint8	pm_Pad3;
	uint8	pm_BitsPerPixel;
	uint8	pm_Depth;
	uint8	pm_BigEndian;
	uint8	pm_TrueColor;
	uint16	pm_RedMax;
	uint16	pm_GreenMax;
	uint16	pm_BlueMax;
	uint8	pm_RedShift;
	uint8	pm_GreenShift;
	uint8	pm_BlueShift;
	uint8	pm_Pad4;
	uint8	pm_Pad5;
	uint8	pm_Pad6;
};

#endif

// --

int VNC_SetPixelFormat( struct Config *cfg )
{
struct PixelMessage *buf;
struct PixelMessage *pm;
int error;
int size;
int rc;

	// Don't Change PixelFormat while a UpdateBuffer, is being processed
	IExec->ObtainSemaphore( & cfg->Server_UpdateSema );

	error = TRUE;

	buf = cfg->NetRead_ReadBuffer;

	size = sizeof( struct PixelMessage );

	rc = myNetRead( cfg, buf, size, MSG_WAITALL );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	if (( buf->pm_Type != 0 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%ld != %ld)", rc, size );
		goto bailout;
	}

	//
	// Ultra VNC client, like to send SetPixelFormat and SetEncoding over
	// and over at random times .. so we only reactive if something change
	//

	if ( DoVerbose )
	{
		printf( "Client Request PixelFormat\n" );
		printf( "\n" );
		printf( " BitsPerPixel.: %d\n", buf->pm_BitsPerPixel );
		printf( " Pad..........: %d\n", buf->pm_Pad );
		printf( " Pad2.........: %d\n", buf->pm_Pad2 );
		printf( " Pad3.........: %d\n", buf->pm_Pad3 );
		printf( " Depth........: %d\n", buf->pm_Depth );
		printf( " BigEndian....: %d\n", buf->pm_BigEndian );
		printf( " TrueColor....: %d\n", buf->pm_TrueColor );
		printf( " RedMax.......: %d\n", buf->pm_RedMax );
		printf( " GreenMax.....: %d\n", buf->pm_GreenMax );
		printf( " BlueMax......: %d\n", buf->pm_BlueMax );
		printf( " RedShift.....: %d\n", buf->pm_RedShift );
		printf( " GreenShift...: %d\n", buf->pm_GreenShift );
		printf( " BlueShift....: %d\n", buf->pm_BlueShift );
		printf( " Pad4.........: %d\n", buf->pm_Pad4 );
		printf( " Pad5.........: %d\n", buf->pm_Pad5 );
		printf( " Pad6.........: %d\n", buf->pm_Pad6 );
		printf( "\n" );
	}

	buf->pm_Pad		= 0;
	buf->pm_Pad2	= 0;
	buf->pm_Pad3	= 0;
	buf->pm_Pad4	= 0;
	buf->pm_Pad5	= 0;
	buf->pm_Pad6	= 0;

	pm = & cfg->GfxRead_Enocde_ActivePixel;

	if ( memcmp( pm, buf, sizeof( struct PixelMessage )))
	{
		if ( DoVerbose )
		{
			printf( "Updating PixelFormat\n\n" );
		}

		mySetEncoding_Message( cfg, buf, TRUE );
	}
	else
	{
		if ( DoVerbose )
		{
			printf( "Keeping PixelFormat (Nothing changed)\n\n" );
		}
	}

	error = FALSE;

bailout:

	IExec->ReleaseSemaphore( & cfg->Server_UpdateSema );

	return( error );
}

// --
