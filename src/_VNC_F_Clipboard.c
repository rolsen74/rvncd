
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

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

// --

S32 VNC_Clipboard( struct Config *cfg )
{
struct ClipboardMessage *cm;
struct VNCMsgPort port;
struct IOClipReq *ioreq;
struct Library *base;
U32 header[5];
STR data;
STR buf;
U32 datalen;
S32 error;
S32 size;
S32 len;
S32 rc;

	buf = NULL;
	data = NULL;
	base = NULL;
//	port = NULL;
	ioreq = NULL;
	error = TRUE;

	// --

	size = sizeof( struct ClipboardMessage );

	cm = cfg->NetRead_ReadBuffer;

	rc = Func_NetRead( cfg, cm, size, MSG_WAITALL );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	if (( cm->cm_Type != 6 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%ld != %ld)", rc, size );
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
		buf = mem_AllocClr( datalen + 1 );

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

	rc = Func_NetRead( cfg, data, datalen, 0 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	// --
	// We dont stop just becoarse Clipboard fails

	error = FALSE;

	if ( cfg->cfg_Active_Settings.ReadClipboard )
	{
		if ( ! MsgPort_Init( & port ))
		{
			goto bailout;
		}

//		port = AllocSysObjectTags( ASOT_PORT,
//			TAG_END
//		);
//
//		if ( port == NULL )
//		{
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
			goto bailout;
		}

		if ( OpenDevice( "clipboard.device", 0, (PTR) ioreq, 0 ))
		{
			goto bailout;
		}

		base = (PTR) ioreq->io_Device;

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
		ioreq->io_Data		= (PTR) header;
		ioreq->io_Length	= 20;
		ioreq->io_Command	= CMD_WRITE;

		if ( DoIO( (PTR) ioreq ))
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

		if ( DoIO( (PTR) ioreq ))
		{
			goto bailout;
		}

		if ( ioreq->io_Actual != datalen )
		{
			goto bailout;
		}

		cfg->NetSend_ClipBoardData.cd_Ignore = TRUE;

		ioreq->io_Command = CMD_UPDATE;

		if ( DoIO( (PTR) ioreq ))
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

	return( error );
}

// --
