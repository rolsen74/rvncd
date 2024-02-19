 
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
** - Server handles a Mouse event from Client
**
** Returns:
** - True and the socket will be closed
** - False and we continue
*/

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

int VNC_UpdateRequest( struct Config *cfg )
{
struct UpdateRequestMessage *buf;
struct SocketIFace *ISocket;
int error;
int size;
int rc;

	ISocket = cfg->NetRead_ISocket;

	buf = cfg->NetRead_ReadBuffer;

	error = TRUE;

	size = sizeof( struct UpdateRequestMessage );

	rc = ISocket->recv( cfg->NetRead_ClientSocket, buf, size, MSG_WAITALL );

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

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Read_Bytes += rc;

	if (( buf->urm_Type != 3 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%d != %d)", rc, size );
		goto bailout;
	}

	// --

	if ( ! buf->urm_Incremental )
	{
		cfg->cfg_ServerDoFullUpdate = TRUE;
	}

	// Set after Full Update
	IExec->Forbid();
	cfg->cfg_ServerGotBufferUpdateRequest++;
	IExec->Permit();

	// --

	#if 0
	// Log_PrintF( "\nVNC_UpdateRequest\n" );
	// Log_PrintF( "Type ...... : %d\n", buf->urm_Type );
	// Log_PrintF( "Incremental : %d\n", buf->urm_Incremental );
	// Log_PrintF( "XPos ...... : %d\n", buf->urm_XPos );
	// Log_PrintF( "YPos ...... : %d\n", buf->urm_YPos );
	// Log_PrintF( "Width ..... : %d\n", buf->urm_Width );
	// Log_PrintF( "Height .... : %d\n", buf->urm_Height );
	#endif

	// --

	error = FALSE;

bailout:

	return( error );
}

// --
