
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if 0

struct UpdateRequestMessage
{
	U8	urm_Type;
	U8	urm_Incremental;
	U16	urm_XPos;
	U16	urm_YPos;
	U16	urm_Width;
	U16	urm_Height;
};

#endif

// --

S32 VNC_UpdateRequest( struct Config *cfg )
{
struct UpdateRequestMessage *buf;
struct UpdateNode *un;
S32 error;
S32 size;
S32 rc;

	buf = cfg->NetRead_ReadBuffer;

	error = TRUE;

	size = sizeof( struct UpdateRequestMessage );

	rc = Func_NetRead( cfg, buf, size, MSG_WAITALL );

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
//		SHELLBUF_PRINTF( "New UpdateReq Msg : %s\n", buf->urm_Incremental ? "Only Changes" : "Full Update" );
		un->un_Type = UT_UpdateRequest;
		memcpy( & un->un_Data.update, buf, sizeof( struct UpdateRequestMessage ));
		List_AddTail( & cfg->Server_UpdateList, & un->un_Node );
	}

	ReleaseSemaphore( & cfg->Server_UpdateSema );

	// --

	error = FALSE;

bailout:

	return( error );
}

// --
