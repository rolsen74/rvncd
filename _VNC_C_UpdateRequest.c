
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

#if 0

struct UpdateRequestMessage
{
	uint8	urm_Type;
	uint8	urm_Incremental;
	uint16	urm_XPos;
	uint16	urm_YPos;
	uint16	urm_Width;
	uint16	urm_Height;
};

#endif

// --

int VNC_UpdateRequest( struct Config *cfg )
{
struct UpdateRequestMessage *buf;
struct UpdateNode *un;
int error;
int size;
int rc;

	buf = cfg->NetRead_ReadBuffer;

	error = TRUE;

	size = sizeof( struct UpdateRequestMessage );

	rc = myNetRead( cfg, buf, size, MSG_WAITALL );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	if (( buf->urm_Type != 3 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%ld != %ld)", rc, size );
		goto bailout;
	}

	// --

	IExec->ObtainSemaphore( & cfg->Server_UpdateSema );

	un = (APTR) IExec->RemHead( & cfg->Server_UpdateFree );

	if ( un == NULL )
	{
		un = myCalloc( sizeof( struct UpdateNode ) );

		if ( un == NULL )
		{
			printf( "Yikes Calloc failed\n" );
		}
	}

	if ( un )
	{
		un->un_Incremental = buf->urm_Incremental;
		un->un_XPos		= buf->urm_XPos;
		un->un_YPos		= buf->urm_YPos;
		un->un_Width	= buf->urm_Width;
		un->un_Height	= buf->urm_Height;

		IExec->AddTail( & cfg->Server_UpdateList, & un->un_Node );
	}

	IExec->ReleaseSemaphore( & cfg->Server_UpdateSema );

	// --

	error = FALSE;

bailout:

	return( error );
}

// --
