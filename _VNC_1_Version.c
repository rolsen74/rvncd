 
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

/*
** Purpose:
** - Exchange protocol version to operate with
**
** Returns:
** - True and the socket will be closed
** - False and we continue
*/

int VNC_Version( struct Config *cfg )
{
struct SocketIFace *ISocket;
char *buf;
int rejected;
int ver;
int rev;
int rc;

	ISocket = cfg->NetSend_ISocket;

	buf = cfg->NetSend_SendBuffer;

	rejected = TRUE;

	if ( DoVerbose )
	{
		Log_PrintF( cfg, LOGTYPE_Info, "Negociating protocol version" );
	}

	// --- Version Exchange

	ver = 3;
	rev = 3;

	// "RFB 003.003\n" - 12 bytes exactly
	sprintf( buf, "RFB %03d.%03d\n", ver, rev );

	if ( DoVerbose )
	{
		Log_PrintF( cfg, LOGTYPE_Info, ". Maximum version supported by server v%ld.%ld", ver, rev );
	}

	rc = ISocket->send( cfg->NetSend_ClientSocket, buf, 12, 0 );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to send data (%d)", ISocket->Errno() );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "Failed to send data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Send_Bytes += rc;

	rc = ISocket->recv( cfg->NetSend_ClientSocket, buf, 12, MSG_WAITALL );

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

	buf[12] = 0;

	ver = 0;
	rev = 0;

	sscanf( buf, "RFB %d.%d\n", &ver, &rev );

	if ( DoVerbose )
	{
		Log_PrintF( cfg, LOGTYPE_Info, ". Client requested version v%ld.%ld", ver, rev );
	}

	if (( ver == 3 ) && ( rev == 3 ))
	{
		if ( DoVerbose )
		{
			Log_PrintF( cfg, LOGTYPE_Info, ". Protocol version v%ld.%ld selected", ver, rev );
		}

		cfg->NetSend_ClientProtocol = VNCProtocol_33;
	}
	else
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Unsupported version v%ld.%ld", ver, rev );
		goto bailout;
	}

	rejected = FALSE;

bailout:

// IExec->DebugPrintF( "VNC_Version (%d)\n", rejected );

	return( rejected );
}

// --
