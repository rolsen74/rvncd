 
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

static uint32 ClipBoard_CallBack( struct Hook *h, void *o UNUSED, struct ClipHookMsg *msg UNUSED )
{
struct ClipData *cd;
struct Config *cfg;

	cd = (APTR) h->h_Data;
	cfg = cd->cd_Config;

	if (( cd ) && ( cfg ))
	{
		if ( cfg->NetSend_ClipBoardData.cd_Ignore == FALSE )
		{
//			cd->cd_ID = msg->chm_ClipID;
			if ( cfg->cfg_Active_Settings.SendClipboard )
			{
				IExec->Signal( cd->cd_Task, cfg->NetSend_ClipBoardSignal );
			}
		}
		else
		{
			// We are just written to CB, so this can be ignored
			cfg->NetSend_ClipBoardData.cd_Ignore = FALSE;
		}
	}

	return( 0 );
}

// --

int Send_Install_ClipHook( struct Config *cfg )
{
struct IOClipReq *ioreq;
struct ClipData *cd;
struct Hook *hook;
int error;

	error = TRUE;

	Log_PrintF( cfg, LOGTYPE_Info, "Installing ClipBoard Hook" );

	cfg->NetSend_ClipBoardMsgPort = IExec->AllocSysObjectTags( ASOT_PORT,
		TAG_END
	);

	if ( cfg->NetSend_ClipBoardMsgPort == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating ClipBoard MsgPort" );
		goto bailout;
	}

	cfg->NetSend_ClipBoardIOReq = IExec->AllocSysObjectTags( ASOT_IOREQUEST,
		ASOIOR_ReplyPort, cfg->NetSend_ClipBoardMsgPort,
		ASOIOR_Size, sizeof( struct IOClipReq ),
		TAG_END
	);

	if ( cfg->NetSend_ClipBoardIOReq == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating ClipBoard IORequest" );
		goto bailout;
	}

	if ( IExec->OpenDevice( "clipboard.device", 0, (APTR) cfg->NetSend_ClipBoardIOReq, 0 ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening ClipBoard device" );
		goto bailout;
	}

	cfg->NetSend_ClipBoardBase = (APTR) cfg->NetSend_ClipBoardIOReq->io_Device;

	// --

	cfg->NetSend_ClipBoardSignalNr = IExec->AllocSignal( -1 );

	if ( cfg->NetSend_ClipBoardSignalNr == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating ClipBoard Signal" );
		goto bailout;
	}

	cfg->NetSend_ClipBoardSignal = 1UL << cfg->NetSend_ClipBoardSignalNr;

	// --

	cd					= & cfg->NetSend_ClipBoardData;
	cd->cd_Config		= cfg;
	cd->cd_Task			= IExec->FindTask( NULL );
	cd->cd_ID			= -1;

	hook				= & cfg->NetSend_ClipBoardHook;
    hook->h_Entry		= (HOOKFUNC) ClipBoard_CallBack;
    hook->h_Data		= cd;
 
	ioreq				= cfg->NetSend_ClipBoardIOReq;
	ioreq->io_Data		= (APTR) hook;
	ioreq->io_Length	= 1;
	ioreq->io_Command	= CBD_CHANGEHOOK;
 
	if ( IExec->DoIO( (APTR) ioreq ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error installing ClipBoard Hook" );
		goto bailout;
	}

	cfg->NetSend_ClipBoardHookInstalled = TRUE;

	// We do not use the MsgPort or IOReq untell we need 
	// to remove the Hook again, so sort of waste. 

	// But it will be really bad if freed resources 
	// and when we needed to Remove the hook again, we
	// failed to alloc the resources again, this
	// would leave us when a hanging hook and result
	// in a crash the next time clipboard is used.

	error = FALSE;

bailout:

	return( error );
}

// --

void Send_Remove_ClipHook( struct Config *cfg )
{
struct IOClipReq *ioreq;

	Log_PrintF( cfg, LOGTYPE_Info, "Removing ClipBoard Hook" );

	if ( cfg->NetSend_ClipBoardHookInstalled )
	{
		ioreq				= cfg->NetSend_ClipBoardIOReq;
		ioreq->io_Data		= (APTR) & cfg->NetSend_ClipBoardHook;
		ioreq->io_Length	= 0;
		ioreq->io_Command	= CBD_CHANGEHOOK;

		IExec->DoIO( (APTR) ioreq );

		cfg->NetSend_ClipBoardHookInstalled = FALSE;
	}

	if ( cfg->NetSend_ClipBoardSignalNr != -1 )
	{
		IExec->FreeSignal( cfg->NetSend_ClipBoardSignalNr );
		cfg->NetSend_ClipBoardSignalNr = -1;
	}

	if ( cfg->NetSend_ClipBoardBase )
	{
		IExec->CloseDevice(	(APTR) cfg->NetSend_ClipBoardIOReq );
		cfg->NetSend_ClipBoardBase = NULL;
	}

	if ( cfg->NetSend_ClipBoardIOReq )
	{
		IExec->FreeSysObject( ASOT_IOREQUEST, cfg->NetSend_ClipBoardIOReq );
		cfg->NetSend_ClipBoardIOReq = NULL;
	}

	if ( cfg->NetSend_ClipBoardMsgPort )
	{
		IExec->FreeSysObject( ASOT_PORT, cfg->NetSend_ClipBoardMsgPort );
		cfg->NetSend_ClipBoardMsgPort = NULL;
	}
}

// --

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

void Send_Handle_ClipBoard( struct Config *cfg )
{
struct ClipboardMessage header;
struct SocketIFace *ISocket;
struct IOClipReq *ioreq;
struct MsgPort *port;
struct Library *base;
uint32 data[3];
char *buf;
int len;
int rc;

	buf = NULL;
	base = NULL;
	port = NULL;
	ioreq = NULL;

	if ( cfg->cfg_Active_Settings.SendClipboard )
	{
		port = IExec->AllocSysObjectTags( ASOT_PORT,
			TAG_END
		);

		if ( port == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error creating MsgPort" );
			goto bailout;
		}

		ioreq = IExec->AllocSysObjectTags( ASOT_IOREQUEST,
			ASOIOR_ReplyPort, port,
			ASOIOR_Size, sizeof( struct IOClipReq ),
			TAG_END
		);

		if ( ioreq == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error creating IORequest" );
			goto bailout;
		}

		if ( IExec->OpenDevice( "clipboard.device", 0, (APTR) ioreq, 0 ))
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error opening ClipBoard" );
			goto bailout;
		}

		base = (APTR) ioreq->io_Device;

		// --

		ioreq->io_Offset	= 0;
		ioreq->io_Error		= 0;
		ioreq->io_ClipID	= 0;
		ioreq->io_Data		= (APTR) data;
		ioreq->io_Length	= 12;
		ioreq->io_Command	= CMD_READ;

		if ( IExec->DoIO( (APTR) ioreq ))
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error reading from ClipBoard" );
			goto bailout;
		}

		if ( ioreq->io_Actual != 12 )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error invalid length" );
			goto bailout;
		}

		if ( data[0] != ID_FORM )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error invalid data header" );
			goto bailout;
		}

		if ( data[2] != ID_FTXT )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Unsupported IFF type" );
			goto bailout;
		}

		// Parse IFF .. Find CHRS Chunk

		while( TRUE )
		{
			ioreq->io_Data		= (APTR) data;
			ioreq->io_Length	= 8;
			ioreq->io_Command	= CMD_READ;

			if ( IExec->DoIO( (APTR) ioreq ))
			{
				Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error reading data" );
				goto bailout;
			}

			if ( ioreq->io_Actual != 8 )
			{
				Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error invalid length" );
				goto bailout;
			}

			if ( data[0] == ID_CHRS )
			{
				break;
			}

			// -- Skip Chunk

			len  = data[1];
			len +=  1;
			len &= ~1;

			ioreq->io_Data		= NULL;
			ioreq->io_Length	= len;
			ioreq->io_Command	= CMD_READ;

			if ( IExec->DoIO( (APTR) ioreq ))
			{
				Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error skipping data" );
				goto bailout;
			}
		}

		buf = myMalloc( data[1] );

		if ( buf == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error allocating memory (%ld)", data[1] );
			goto bailout;
		}

		ioreq->io_Data		= buf;
		ioreq->io_Length	= data[1];
		ioreq->io_Command	= CMD_READ;

		if ( IExec->DoIO( (APTR) ioreq ))
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error reading data" );
			goto bailout;
		}

		// --

		header.cm_Type		= 3; // Server Cut Text
		header.cm_Pad		= 0;
		header.cm_Pad2		= 0;
		header.cm_Pad3		= 0;
		header.cm_Length	= data[1];

		ISocket = cfg->NetSend_ISocket;

		rc = ISocket->send( cfg->NetSend_ClientSocket, & header, sizeof( header ), 0 );

		if ( rc == -1 )
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Failed to send data (%d)", ISocket->Errno() );
			}

			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Failed to send data '%s' (%d)", myStrError( ISocket->Errno() ), ISocket->Errno() );
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

		rc = ISocket->send( cfg->NetSend_ClientSocket, buf, data[1], 0 );

		if ( rc == -1 )
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Failed to send data (%d)", ISocket->Errno() );
			}

			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Failed to send data '%s' (%d)", myStrError( ISocket->Errno() ), ISocket->Errno() );
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
	}

	// --

bailout:

	if ( buf )
	{
		myFree( buf );
	}

	if ( base )
	{
		// -- Reading past end to indicate we are done,
		// it's a ClipBoard thing.

		ioreq->io_Actual = 1;

		while( ioreq->io_Actual )
		{
			ioreq->io_Data		= NULL;
			ioreq->io_Actual	= 1;
			ioreq->io_Length	= 1024*1024;
			ioreq->io_Command	= CMD_READ;

			if ( IExec->DoIO( (APTR) ioreq ))
			{
				break;
			}
		}

		// --

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

	return;
}

// --
