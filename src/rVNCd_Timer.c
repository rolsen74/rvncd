
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

struct VNCMsgPort			TimerMsgPort;
struct Device *				TimerBase			= NULL;
struct VNCTimeRequest *		TimerIOReq			= NULL;
static U8					TimerActive			= FALSE;

#ifdef _AOS4_
struct TimerIFace *			ITimer				= NULL;
#endif

// --

void Timer_Start( void )
{
	if ( TimerActive )
	{
		SHELLBUF_PRINTF( "Timer allready active\n" );
		return;
	}

	TimerActive = TRUE;

	// --

	TimerIOReq->Request.io_Command = TR_ADDREQUEST;
	TimerIOReq->Time.Seconds = 1;
	TimerIOReq->Time.Microseconds = 0;

	SendIO( (PTR) TimerIOReq );
}

// --

void Timer_Stop( void )
{
	if ( ! TimerActive )
	{
		SHELLBUF_PRINTF( "Timer not active\n" );
		return;
	}

	if ( ! CheckIO( (PTR) TimerIOReq ))
	{
		AbortIO( (PTR) TimerIOReq );
	}

	WaitIO( (PTR) TimerIOReq );

	// --

	TimerActive = FALSE;
}

// --

S32 Timer_Init( void )
{
S32 retval;

	retval = FALSE;

	if ( ! MsgPort_Init( & TimerMsgPort ))
	{
		goto bailout;
	}

	#ifdef _AOS4_

	TimerIOReq = AllocSysObjectTags( ASOT_IOREQUEST,
		ASOIOR_Size, sizeof( struct VNCTimeRequest ),
		ASOIOR_ReplyPort, & TimerMsgPort.vmp_MsgPort,
		TAG_END
	);

	#else // _AOS4_

	TimerIOReq = CreateIORequest( & TimerMsgPort.vmp_MsgPort, sizeof( struct VNCTimeRequest ));

	#endif

	if ( ! TimerIOReq )
	{
		goto bailout;
	}

	if ( OpenDevice( "timer.device", 0, (PTR) TimerIOReq, UNIT_MICROHZ ))
	{
		goto bailout;
	}

	TimerBase = (PTR) TimerIOReq->Request.io_Device;

	#ifdef _AOS4_

	ITimer = (PTR) GetInterface( (PTR) TimerBase, "main", 1, NULL );

	if ( ! ITimer )
	{
		goto bailout;
	}

	#endif // _AOS4_

	retval = TRUE;

bailout:

	return( retval );
}

// --

void Timer_Free( void )
{
	if ( TimerActive )
	{
		Timer_Stop();
	}

	#ifdef _AOS4_

	if ( ITimer )
	{
		DropInterface( (PTR) ITimer );
		ITimer = NULL;
	}

	#endif

	if ( TimerBase )
	{
		CloseDevice( (PTR) TimerIOReq );
		TimerBase = NULL;
	}

	if ( TimerIOReq )
	{
		#ifdef _AOS4_

		FreeSysObject( ASOT_IOREQUEST, TimerIOReq );

		#else // _AOS4_

		DeleteIORequest( TimerIOReq );

		#endif // _ASO4

		TimerIOReq = NULL;
	}

	MsgPort_Free( & TimerMsgPort );
}

// --

void Timer_Handle( struct Config *cfg )
{
U32 cnt;

	// -- Remove Request

	MsgPort_WaitPort( & TimerMsgPort );

	MsgPort_GetMsg( & TimerMsgPort );

	// -- Restart Timer

	TimerIOReq->Request.io_Command = TR_ADDREQUEST;
	TimerIOReq->Time.Seconds = 1;
	TimerIOReq->Time.Microseconds = 0;
//	TimerIOReq->Time.Seconds = 0;
//	TimerIOReq->Time.Microseconds = 250000;
	SendIO( (PTR) TimerIOReq );

	// -- 

	for( cnt=0 ; cnt<WIN_LAST ; cnt++ )
	{
		if ( cfg->cfg_WinData[cnt].TimerTick )
		{
			 cfg->cfg_WinData[cnt].TimerTick( cfg );
		}
	}

	// --
}

// --
