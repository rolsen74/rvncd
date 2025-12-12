
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 MsgPort_Init( struct VNCMsgPort *mp )
{
S32 retval;
S8 sig;

	retval = FALSE;

	while( mp )
	{
		memset( mp, 0, sizeof( struct VNCMsgPort ));

		sig = AllocSignal( -1 );

		if ( sig == -1 )
		{
			#ifdef DEBUG
			SHELLBUF_PRINTF( "Error allocating signal\n" );
			#endif
			break;
		}

		mp->vmp_MsgPort.mp_Node.ln_Type			= NT_MSGPORT;
		mp->vmp_MsgPort.mp_MsgList.lh_Head		= (PTR) & mp->vmp_MsgPort.mp_MsgList.lh_Tail;
		mp->vmp_MsgPort.mp_MsgList.lh_Tail		= (PTR) NULL;
		mp->vmp_MsgPort.mp_MsgList.lh_TailPred	= (PTR) & mp->vmp_MsgPort.mp_MsgList.lh_Head;
		mp->vmp_MsgPort.mp_SigTask				= FindTask( NULL );
		mp->vmp_MsgPort.mp_Flags				= PA_SIGNAL;
		mp->vmp_MsgPort.mp_SigBit				= sig;
		mp->vmp_SignalBit						= 1UL << sig;
		mp->vmp_StructID						= ID_VNCMPORT;

		if ( DoVerbose > 2 )
		{
			SHELLBUF_PRINTF( "Setup MsgPort %p, Signal %d\n", (PTR) mp, sig );
		}

		retval = TRUE;
		break;
	}

	return(	retval );
}

// --

void MsgPort_Free( struct VNCMsgPort *mp )
{
	if ( ! mp )
	{
		return;
	}

	if ( ! mp->vmp_StructID )
	{
		return;
	}

	if ( mp->vmp_StructID != ID_VNCMPORT )
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "Invalid VNC MsgPort (%p)\n", (PTR) mp );
		#endif
		return;
	}

	mp->vmp_StructID = 0;

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "Clear MsgPort %p, Signal %d\n", (PTR) mp, mp->vmp_MsgPort.mp_SigBit );
	}

	// should proberly free any message in mp_MsgList

	if ( mp->vmp_MsgPort.mp_SigBit != 0xFF )
	{
		FreeSignal( mp->vmp_MsgPort.mp_SigBit );
		mp->vmp_MsgPort.mp_SigBit = 0xFF;
	}
}

// --

PTR MsgPort_GetMsg( struct VNCMsgPort *mp )
{
PTR msg;

	if ( ! mp )
	{
		return( NULL );
	}

	if ( mp->vmp_StructID != ID_VNCMPORT )
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "Invalid VNC MsgPort (%p)\n", (PTR) mp );
		#endif
		return( NULL );
	}

	msg = GetMsg( & mp->vmp_MsgPort );

	return( msg );
}

// --

void MsgPort_WaitPort( struct VNCMsgPort *mp )
{
	if ( ! mp )
	{
		return;
	}

	if ( mp->vmp_StructID != ID_VNCMPORT )
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "Invalid VNC MsgPort (%p)\n", (PTR) mp );
		#endif
		return;
	}

	WaitPort( & mp->vmp_MsgPort );
}

// --

void MsgPort_PutMsg( struct VNCMsgPort *mp, PTR msg )
{
	if ( ! mp )
	{
		return;
	}

	if ( mp->vmp_StructID != ID_VNCMPORT )
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "Invalid VNC MsgPort (%p)\n", (PTR) mp );
		#endif
		return;
	}

	PutMsg( & mp->vmp_MsgPort, msg );
}

// --
