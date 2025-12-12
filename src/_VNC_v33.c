 
/*
 * Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
 *
 * This software is released under the GNU General Public License, version 3.
 * For the full text of the license, please visit:
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * You can also find a copy of the license in the LICENSE file included with this software.
 */

// --

#include "rVNCd.h"

// --

static S32 myRead_Unknown( struct Config *cfg )
{
S32 error;
S32 rc;

	error = TRUE;

	rc = Func_NetRead( cfg, cfg->NetRead_ReadBuffer, cfg->NetRead_ReadBufferSize, 0 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	#ifdef DEBUG
	SHELLBUF_PRINTF( "Skipping packet data (%" PRId32 ")", rc );
	#endif

	error = FALSE;

bailout:

	return( error );
}

// --

S32 VNC_HandleCmds_33( struct Config *cfg )
{
char type;
S32 error;
S32 stat;
S32 rc;

	// --

	error = TRUE;

	rc = Func_NetRead( cfg, & type, 1, MSG_WAITALL|MSG_PEEK );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	// --

	switch( type )
	{
		// Client -> Server Commands
		// v3.3 Protocol

		case 0: // Set Pixel Format
		{
//			DebugPrintF( "VNC_SetPixelFormat\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_SetPixelFormat" );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "VNC_SetPixelFormat\n" );
			}

			stat = VNC_SetPixelFormat( cfg );
			break;
		}

		//   1 - Not used

		case 2: // Set Encoding
		{
//			DebugPrintF( "VNC_SetEncoding\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_SetEncoding" );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "VNC_SetEncoding\n" );
			}

			stat = VNC_SetEncoding( cfg );
			break;
		}

		case 3: // Update Request
		{
//			DebugPrintF( "VNC_UpdateRequest\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_UpdateRequest" );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "VNC_UpdateRequest\n" );
			}

			stat = VNC_UpdateRequest( cfg );
			break;
		}

		case 4:	// Key
		{
//			DebugPrintF( "VNC_Keyboard\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_Keyboard" );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "VNC_Keyboard\n" );
			}

			stat = VNC_Keyboard( cfg );
			break;
		}

		case 5:	// Mouse
		{
//			DebugPrintF( "VNC_Mouse\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_Mouse" );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "VNC_Mouse\n" );
			}

			stat = VNC_Mouse( cfg );
			break;
		}

		case 6: // Clipboard
		{
//			DebugPrintF( "VNC_Clipboard\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_Clipboard" );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "VNC_Clipboard\n" );
			}

			stat = VNC_Clipboard( cfg );
			break;
		}

		default:
		{
			stat = myRead_Unknown( cfg );
			break;
		}
	}

	if ( stat )
	{
//	DebugPrintF( "Stat error %d\n", stat );
		error = TRUE;
		Log_PrintF( cfg, LOGTYPE_Error, "Error Stat : %ld\n", stat );
		goto bailout;
	}

	// --

	error = FALSE;

bailout:

	return( error );
}
