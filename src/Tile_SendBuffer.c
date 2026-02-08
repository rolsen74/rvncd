
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 Tile_SendBuffer_Init( struct Config *cfg )
{
S32 error;
S32 kbsize;
S32 size;

	error = TRUE;

	// --
	// Clamp 0-1024

	kbsize = MIN( cfg->cfg_Active_Settings.TileBufferKB, 1024 );
	kbsize = MAX( kbsize, 0 );

	if ( ! kbsize )
	{
		Log_PrintF( cfg, LOGTYPE_Info, "Tile send buffer disabled" );
		error = FALSE;
		goto bailout;
	}

	// Convert Bytes to KB .. so max 1 MB
	size = kbsize * 1024;

	#ifdef __amigaos4__

	cfg->NetSend_TileBuffer = AllocVecTags( size,
		AVT_PhysicalAlignment, 4096,
		AVT_Contiguous, FALSE,
		AVT_Alignment, 4096,
//		AVT_Lock, FALSE,
		TAG_END
	);

	#else

	cfg->NetSend_TileBuffer = AllocVec( size, MEMF_ANY );

	#endif

	if ( ! cfg->NetSend_TileBuffer )
	{
		goto bailout;
	}

	cfg->NetSend_TileBufferSize = size;

	Log_PrintF( cfg, LOGTYPE_Info, "Using %lu KB tile send buffer", kbsize );

	error = FALSE;

bailout:

	return( error );
}

// --

void Tile_SendBuffer_Free( struct Config *cfg )
{
	if ( cfg->NetSend_TileBuffer )
	{
		FreeVec( cfg->NetSend_TileBuffer );
		cfg->NetSend_TileBufferSize = 0;
		cfg->NetSend_TileBufferUsed = 0;
		cfg->NetSend_TileBuffer = NULL;
	}
}

// --

S32 Tile_SendBuffer_Send( struct Config *cfg, PTR data, U32 len )
{
S32 error;
S32 rc;

	error = TRUE;

	if (( ! cfg->NetSend_TileBuffer ) || ( ! cfg->NetSend_TileBufferSize ))
	{
		// No Buffering

		rc = Func_NetSend( cfg, data, len );

		if ( rc <= 0 )
		{
			#ifdef DEBUG
			SHELLBUF_PRINTF( "Tile_SendBuffer_Send : Error 1\n" );
			#endif
			goto bailout;
		}
	}
	else
	{
		// Buffering enabled

		/**/ if ( cfg->NetSend_TileBufferSize < len )
		{
			// Data larger than Buffer

			if ( Tile_SendBuffer_Flush( cfg ))
			{
				#ifdef DEBUG
				SHELLBUF_PRINTF( "Tile_SendBuffer_Send : Error 2\n" );
				#endif
				goto bailout;
			}

			rc = Func_NetSend( cfg, data, len );

			if ( rc <= 0 )
			{
				#ifdef DEBUG
				SHELLBUF_PRINTF( "Tile_SendBuffer_Send : Error 3\n" );
				#endif
				goto bailout;
			}
		}
		else if ( cfg->NetSend_TileBufferSize <= cfg->NetSend_TileBufferUsed + len )
		{
			// Buffer full

			if ( Tile_SendBuffer_Flush( cfg ))
			{
				#ifdef DEBUG
				SHELLBUF_PRINTF( "Tile_SendBuffer_Send : Error 4\n" );
				#endif
				goto bailout;
			}

			memcpy( cfg->NetSend_TileBuffer, data, len );
			cfg->NetSend_TileBufferUsed = len;
		}
		else
		{
			// Just insert

			memcpy( & cfg->NetSend_TileBuffer[ cfg->NetSend_TileBufferUsed ], data, len );
			cfg->NetSend_TileBufferUsed += len;
		}
	}

	error = FALSE;

bailout:

	return( error );
}

// --

S32 Tile_SendBuffer_Flush( struct Config *cfg )
{
S32 error;
S32 rc;

	error = TRUE;

	if ( cfg->NetSend_TileBufferUsed )
	{
		rc = Func_NetSend( cfg, cfg->NetSend_TileBuffer, cfg->NetSend_TileBufferUsed );

		if ( rc <= 0 )
		{
			SHELLBUF_PRINTF( "Tile_SendBuffer_Send : Error 5\n" );
			goto bailout;
		}

		cfg->NetSend_TileBufferUsed = 0;
	}	

	error = FALSE;

bailout:

	return( error );
}

// --
