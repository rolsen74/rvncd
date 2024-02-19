 
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

struct ClipboardMessage
{
	uint8	cm_Type;
	uint8	cm_Pad;
	uint8	cm_Pad2;
	uint8	cm_Pad3;
	uint32	cm_Length;
	// Text String
};

#endif

int VNC_Clipboard( struct Config *cfg )
{
struct ClipboardMessage *cm;
struct SocketIFace *ISocket;
struct IOClipReq *ioreq;
struct MsgPort *port;
struct Library *base;
uint32 header[5];
char *data;
char *buf;
uint32 datalen;
int error;
int size;
int len;
int rc;

	buf = NULL;
	data = NULL;
	base = NULL;
	port = NULL;
	ioreq = NULL;
	error = TRUE;

	ISocket = cfg->NetRead_ISocket;

	// --

	size = sizeof( struct ClipboardMessage );

	cm = cfg->NetRead_ReadBuffer;

	rc = ISocket->recv( cfg->NetRead_ClientSocket, cm, size, MSG_WAITALL );

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

	if (( cm->cm_Type != 6 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%d != %d)", rc, size );
		goto bailout;
	}

	// --

	datalen = cm->cm_Length;

	if ( datalen == 0 )
	{
		// I guess nothing to do
		error = FALSE;
		goto bailout;
	}

	// --

	if ( datalen > 4096 ) //cfg->NetRead_ReadBufferSize )
	{
		buf = myCalloc( datalen + 1 );

		if ( buf == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory (%ld Bytes)", datalen );
			goto bailout;
		}

		data = buf;
	}
	else
	{
		data = cfg->NetRead_ReadBuffer;
	}

	// --

	rc = ISocket->recv( cfg->NetRead_ClientSocket, data, datalen, 0 );

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

	// --
	// We dont stop just becoarse Clipboard fails

	error = FALSE;

	if ( cfg->cfg_Active_Settings.ReadClipboard )
	{
		port = IExec->AllocSysObjectTags( ASOT_PORT,
			TAG_END
		);

		if ( port == NULL )
		{
			goto bailout;
		}

		ioreq = IExec->AllocSysObjectTags( ASOT_IOREQUEST,
			ASOIOR_ReplyPort, port,
			ASOIOR_Size, sizeof( struct IOClipReq ),
			TAG_END
		);

		if ( ioreq == NULL )
		{
			goto bailout;
		}

		if ( IExec->OpenDevice( "clipboard.device", 0, (APTR) ioreq, 0 ))
		{
			goto bailout;
		}

		base = (APTR) ioreq->io_Device;

		// --

		len  = datalen;
		len +=  1;
		len &= ~1;

		header[0] = ID_FORM;
		header[1] = len + 12;
		header[2] = ID_FTXT;
		header[3] = ID_CHRS;
		header[4] = len;

		ioreq->io_Offset	= 0;
		ioreq->io_Error		= 0;
		ioreq->io_ClipID	= 0;
		ioreq->io_Data		= (APTR) header;
		ioreq->io_Length	= 20;
		ioreq->io_Command	= CMD_WRITE;

		if ( IExec->DoIO( (APTR) ioreq ))
		{
			goto bailout;
		}

		if ( ioreq->io_Actual != 20 )
		{
			goto bailout;
		}

		ioreq->io_Data		= data;
		ioreq->io_Length	= datalen;
		ioreq->io_Command	= CMD_WRITE;

		if ( IExec->DoIO( (APTR) ioreq ))
		{
			goto bailout;
		}

		if ( ioreq->io_Actual != datalen )
		{
			goto bailout;
		}

		cfg->NetSend_ClipBoardData.cd_Ignore = TRUE;

		ioreq->io_Command = CMD_UPDATE;

		if ( IExec->DoIO( (APTR) ioreq ))
		{
			goto bailout;
		}
	}

	// --

bailout:

	if ( buf )
	{
		myFree( buf );
	}

	if ( base )
	{
		IExec->CloseDevice(	(APTR) ioreq );
	}

	if ( ioreq )
	{
		IExec->FreeSysObject( ASOT_IOREQUEST, ioreq );
	}

	if ( port )
	{
		IExec->FreeSysObject( ASOT_PORT, port );
	}

// IExec->DebugPrintF( "myRead_Mouse (%d)\n", error );

	return( error );
}

// --
