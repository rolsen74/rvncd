 
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
struct SocketIFace *ISocket;
int error;
int rc;

	ISocket = cfg->NetRead_ISocket;

	error = TRUE;

	rc = ISocket->recv( cfg->NetRead_ClientSocket, cfg->NetRead_ReadBuffer, 2048, 0 );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to read data (%d)", ISocket->Errno() );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "Failed to read data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		cfg->cfg_ServerRunning = FALSE;

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Read_Bytes += rc;

	Log_PrintF( cfg, LOGTYPE_Info, "Skipping packet data (%ld)", rc );

	error = FALSE;

bailout:

	return( error );
}

// --

int VNC_HandleCmds_33( struct Config *cfg )
{
struct SocketIFace *ISocket;
char type;
int error;
int stat;
int rc;

// IExec->DebugPrintF( "VNC_HandleCmds_33\n" );

	// --

	ISocket = cfg->NetRead_ISocket;

	rc = ISocket->recv( cfg->NetRead_ClientSocket, & type, 1, MSG_PEEK );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to peek data (%d)", ISocket->Errno() );
		}

		error = TRUE;
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to peek data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		error = TRUE;

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	// --

	switch( type )
	{
		// Client -> Server Commands
		// v3.3 Protocol

		case 0: // Set Pixel Format
		{
//			IExec->DebugPrintF( "VNC_SetPixelFormat\n" );
//			Log_PrintF( cfg, LOGTYPE_Error, "VNC_SetPixelFormat" );
			stat = VNC_SetPixelFormat( cfg );
			break;
		}

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

// IExec->DebugPrintF( "VNC_HandleCmds_33 (%d)\n", error );

	return( error );
}
