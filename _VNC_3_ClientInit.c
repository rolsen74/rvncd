 
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
** - Client send share connection to server
**
** Returns:
** - True and the socket will be closed
** - False and we continue
*/

#pragma pack(1)

struct ClientInitMessage
{
	uint8	cim_Shared;
};

#pragma pack(0)

int VNC_ClientInit( struct Config *cfg )
{
struct ClientInitMessage *cim;
struct SocketIFace *ISocket;
int rejected;
int rc;

// IExec->DebugPrintF( "VNC_ClientInit\n" );

	ISocket = cfg->NetSend_ISocket;

	rejected = TRUE;

	cim = cfg->NetSend_SendBuffer;

	// -- Get Shared Flag
	// We dont use this

	rc = ISocket->recv( cfg->NetSend_ClientSocket, cim, sizeof( struct ClientInitMessage ), MSG_WAITALL );

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

	#if 0
	IExec->DebugPrintF( "\nVNC_ClientInit\n" );
	IExec->DebugPrintF( "Shared ....... : %d\n",	cim.cim_Shared );
	#endif

	// --

	rejected = FALSE;

bailout:

// IExec->DebugPrintF( "VNC_ClientInit (%d)\n", rejected );

	return( rejected );
}

// --
