
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if 0

struct EncodingMessage
{
	U8	em_Type;
	U8	em_Pad;
	U16	em_Encodings;
};

#endif

// --

S32 VNC_SetEncoding( struct Config *cfg )
{
struct EncodingMessage *buf;
struct UpdateNode *un;
U32 *enc;
S32 error;
S32 size;
S32 rc;

	// --

	error = TRUE;

	buf = cfg->NetRead_ReadBuffer;

	size = sizeof( struct EncodingMessage );

	rc = Func_NetRead( cfg, buf, size, MSG_WAITALL );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	if ( buf->em_Type != 2 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data type (%ld != %ld)", buf->em_Type, 2 );
		goto bailout;
	}

	// --

	if ( buf->em_Encodings )
	{
		// todo reuse readbuffer if under 4k

		enc = mem_Alloc( buf->em_Encodings * sizeof( U32 ));

		if ( ! enc )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory (%ld Bytes)", buf->em_Encodings * sizeof( U32 ));
			goto bailout;
		}

		rc = Func_NetRead( cfg, enc, buf->em_Encodings * sizeof( U32 ), MSG_WAITALL );

		if ( rc <= 0 )
		{
			goto bailout;
		}
	}
	else
	{
		enc = NULL;
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
//		SHELLBUF_PRINTF( "New Encoding Msg\n" );
		un->un_Type = UT_EncodingMsg;
		un->un_Buffer = enc;
		memcpy( & un->un_Data.encoding, buf, sizeof( struct EncodingMessage ));
		List_AddTail( & cfg->Server_UpdateList, & un->un_Node );
	}

	ReleaseSemaphore( & cfg->Server_UpdateSema );

	// --

	error = FALSE;

bailout:

	return( error );
}

// --
