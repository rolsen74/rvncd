 
/*
 * Copyright (c) 2023-2024 Rene W. Olsen < renewolsen @ gmail . com >
 *
 * This software is released under the GNU General Public License, version 3.
 * For the full text of the license, please visit:
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * You can also find a copy of the license in the LICENSE file included with this software.
 */

// --

#include "RVNCd.h"

// --

static int myRead_Unknown( struct Config *cfg )
{
int error;
int rc;

	error = TRUE;

	rc = myNetRead( cfg, cfg->NetRead_ReadBuffer, cfg->NetRead_ReadBufferSize, 0 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	Log_PrintF( cfg, LOGTYPE_Info, "Skipping packet data (%ld)", rc );

	error = FALSE;

bailout:

	return( error );
}

// --

int VNC_HandleCmds_37( struct Config *cfg )
{
char type;
int error;
int stat;
int rc;

	// --

	error = TRUE;

	rc = myNetRead( cfg, & type, 1, MSG_WAITALL|MSG_PEEK );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	// --

	switch( type )
	{
		// Client -> Server Commands
		// v3.7 Protocol

		case 0: // Set Pixel Format
		{
//			IExec->DebugPrintF( "VNC_SetPixelFormat\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_SetPixelFormat" );
			stat = VNC_SetPixelFormat( cfg );
			break;
		}

		//   1 -  
		//   1 - Not used

		case 2: // Set Encoding
		{
//			IExec->DebugPrintF( "VNC_SetEncoding\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_SetEncoding" );
			stat = VNC_SetEncoding( cfg );
			break;
		}

		case 3: // Update Request
		{
//			IExec->DebugPrintF( "VNC_UpdateRequest\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_UpdateRequest" );
			stat = VNC_UpdateRequest( cfg );
			break;
		}

		case 4:	// Key
		{
//			IExec->DebugPrintF( "VNC_Keyboard\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_Keyboard" );
			stat = VNC_Keyboard( cfg );
			break;
		}

		case 5:	// Mouse
		{
//			IExec->DebugPrintF( "VNC_Mouse\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_Mouse" );
			stat = VNC_Mouse( cfg );
			break;
		}

		case 6: // Clipboard
		{
//			IExec->DebugPrintF( "VNC_Clipboard\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_Clipboard" );
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
//	IExec->DebugPrintF( "Stat error %d\n", stat );
		error = TRUE;
		Log_PrintF( cfg, LOGTYPE_Error, "Error Stat : %ld\n", stat );
		goto bailout;
	}

	// --

	error = FALSE;

bailout:

	return( error );
}
