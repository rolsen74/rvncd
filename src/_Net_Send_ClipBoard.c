
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

U32 ClipBoard_CallBack( struct Hook *h, PTR o UNUSED, PTR msgptr UNUSED )
{
//struct ClipHookMsg *msg = (PTR) msgptr;
struct ClipData *cd;
struct Config *cfg;

	cd = (PTR) h->h_Data;
	cfg = cd->cd_Config;

	if (( cd ) && ( cfg ))
	{
		if ( cfg->NetSend_ClipBoardData.cd_Ignore == FALSE )
		{
//			cd->cd_ID = msg->chm_ClipID;
			if ( cfg->cfg_Active_Settings.SendClipboard )
			{
				Signal( cd->cd_Task, cfg->NetSend_ClipBoardSignal );
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

S32 Send_Install_ClipHook( struct Config *cfg )
{
struct IOClipReq *ioreq;
struct ClipData *cd;
struct Hook *hook;
S32 error;

	error = TRUE;

	Log_PrintF( cfg, LOGTYPE_Info, "Installing ClipBoard Hook" );

	if ( ! MsgPort_Init( & cfg->NetSend_ClipBoardMsgPort ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating ClipBoard MsgPort" );
		goto bailout;
	}

//	cfg->NetSend_ClipBoardMsgPort = AllocSysObjectTags( ASOT_PORT,
//		TAG_END
//	);
//
//	if ( cfg->NetSend_ClipBoardMsgPort == NULL )
//	{
//		Log_PrintF( cfg, LOGTYPE_Error, "Error creating ClipBoard MsgPort" );
//		goto bailout;
//	}

	#ifdef _AOS4_

	cfg->NetSend_ClipBoardIOReq = AllocSysObjectTags( ASOT_IOREQUEST,
		ASOIOR_ReplyPort, cfg->NetSend_ClipBoardMsgPort,
		ASOIOR_Size, sizeof( struct IOClipReq ),
		TAG_END
	);

	#else

	cfg->NetSend_ClipBoardIOReq = (PTR) CreateIORequest( & cfg->NetSend_ClipBoardMsgPort.vmp_MsgPort, sizeof( struct IOClipReq ));

	#endif

	if ( ! cfg->NetSend_ClipBoardIOReq )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating ClipBoard IORequest" );
		goto bailout;
	}

	if ( OpenDevice( "clipboard.device", 0, (PTR) cfg->NetSend_ClipBoardIOReq, 0 ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening ClipBoard device" );
		goto bailout;
	}

	cfg->NetSend_ClipBoardBase = (PTR) cfg->NetSend_ClipBoardIOReq->io_Device;

	// --

	cfg->NetSend_ClipBoardSignalNr = AllocSignal( -1 );

	if ( cfg->NetSend_ClipBoardSignalNr == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating ClipBoard Signal" );
		goto bailout;
	}

	cfg->NetSend_ClipBoardSignal = 1UL << cfg->NetSend_ClipBoardSignalNr;

	// --

	cd					= & cfg->NetSend_ClipBoardData;
	cd->cd_Config		= cfg;
	cd->cd_Task			= FindTask( NULL );
	cd->cd_ID			= -1;

	hook				= & cfg->NetSend_ClipBoardHook;
    hook->h_Entry		= (long unsigned int (*)()) ClipBoard_CallBack;
    hook->h_Data		= cd;
 
	ioreq				= cfg->NetSend_ClipBoardIOReq;
	ioreq->io_Data		= (PTR) hook;
	ioreq->io_Length	= 1;
	ioreq->io_Command	= CBD_CHANGEHOOK;
 
	if ( DoIO( (PTR) ioreq ))
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
		ioreq->io_Data		= (PTR) & cfg->NetSend_ClipBoardHook;
		ioreq->io_Length	= 0;
		ioreq->io_Command	= CBD_CHANGEHOOK;

		DoIO( (PTR) ioreq );

		cfg->NetSend_ClipBoardHookInstalled = FALSE;
	}

	if ( cfg->NetSend_ClipBoardSignalNr != -1 )
	{
		FreeSignal( cfg->NetSend_ClipBoardSignalNr );
		cfg->NetSend_ClipBoardSignalNr = -1;
	}

	if ( cfg->NetSend_ClipBoardBase )
	{
		CloseDevice(	(PTR) cfg->NetSend_ClipBoardIOReq );
		cfg->NetSend_ClipBoardBase = NULL;
	}

	if ( cfg->NetSend_ClipBoardIOReq )
	{
		#ifdef _AOS4_

		FreeSysObject( ASOT_IOREQUEST, cfg->NetSend_ClipBoardIOReq );

		#else

		DeleteIORequest( cfg->NetSend_ClipBoardIOReq );

		#endif

		cfg->NetSend_ClipBoardIOReq = NULL;
	}

	MsgPort_Free( & cfg->NetSend_ClipBoardMsgPort );

//	if ( cfg->NetSend_ClipBoardMsgPort )
//	{
//		FreeSysObject( ASOT_PORT, cfg->NetSend_ClipBoardMsgPort );
//		cfg->NetSend_ClipBoardMsgPort = NULL;
//	}
}

// --

#if 0

struct ClipboardMessage
{
	U8	cm_Type;
	U8	cm_Pad;
	U8	cm_Pad2;
	U8	cm_Pad3;
	U32	cm_Length;
	// Text String
};

#endif

void Send_Handle_ClipBoard( struct Config *cfg )
{
struct ClipboardMessage header;
struct VNCMsgPort port;
struct IOClipReq *ioreq;
struct Library *base;
U32 data[3];
STR buf;
S32 len;
S32 rc;

	buf = NULL;
	base = NULL;
//	port = NULL;
	ioreq = NULL;

	if ( cfg->cfg_Active_Settings.SendClipboard )
	{
		if ( ! MsgPort_Init( & port ))
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error creating MsgPort" );
			goto bailout;
		}

//		port = AllocSysObjectTags( ASOT_PORT,
//			TAG_END
//		);
//
//		if ( port == NULL )
//		{
//			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error creating MsgPort" );
//			goto bailout;
//		}

		#ifdef _AOS4_

		ioreq = AllocSysObjectTags( ASOT_IOREQUEST,
			ASOIOR_ReplyPort, port,
			ASOIOR_Size, sizeof( struct IOClipReq ),
			TAG_END
		);

		#else

		ioreq = (PTR) CreateIORequest( & port.vmp_MsgPort, sizeof( struct IOClipReq ));

		#endif

		if ( ! ioreq )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error creating IORequest" );
			goto bailout;
		}

		if ( OpenDevice( "clipboard.device", 0, (PTR) ioreq, 0 ))
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error opening ClipBoard" );
			goto bailout;
		}

		base = (PTR) ioreq->io_Device;

		// --

		ioreq->io_Offset	= 0;
		ioreq->io_Error		= 0;
		ioreq->io_ClipID	= 0;
		ioreq->io_Data		= (PTR) data;
		ioreq->io_Length	= 12;
		ioreq->io_Command	= CMD_READ;

		if ( DoIO( (PTR) ioreq ))
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
			ioreq->io_Data		= (PTR) data;
			ioreq->io_Length	= 8;
			ioreq->io_Command	= CMD_READ;

			if ( DoIO( (PTR) ioreq ))
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

			if ( DoIO( (PTR) ioreq ))
			{
				Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error skipping data" );
				goto bailout;
			}
		}

		buf = mem_Alloc( data[1] );

		if ( buf == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "ClipHandle - Error allocating memory (%ld)", data[1] );
			goto bailout;
		}

		ioreq->io_Data		= buf;
		ioreq->io_Length	= data[1];
		ioreq->io_Command	= CMD_READ;

		if ( DoIO( (PTR) ioreq ))
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

		rc = Func_NetSend( cfg, & header, sizeof( header ));

		if ( rc <= 0 )
		{
			goto bailout;
		}

		rc = Func_NetSend( cfg, buf, data[1] );

		if ( rc <= 0 )
		{
			goto bailout;
		}
	}

	// --

bailout:

	if ( buf )
	{
		mem_Free( buf );
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

			if ( DoIO( (PTR) ioreq ))
			{
				break;
			}
		}

		// --

		CloseDevice(	(PTR) ioreq );
	}

	if ( ioreq )
	{
		#ifdef _AOS4_

		FreeSysObject( ASOT_IOREQUEST, ioreq );

		#else

		DeleteIORequest( ioreq );

		#endif
	}

	MsgPort_Free( & port );

//	if ( port )
//	{
//		FreeSysObject( ASOT_PORT, port );
//	}

	return;
}

// --
