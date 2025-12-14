
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

static void myProcess_Main( struct Config *cfg )
{
U32 mask;

	#ifdef DEBUG
	DebugPrintF( "WatchDog started\n" );
	#endif

	while( TRUE )
	{
		// Note: CTRL+C used by Timer, it should be ignored
		mask = Wait( NET_EXIT_SIGNAL | cfg->WatchDog_ReplyPort.vmp_SignalBit );

		if (( mask & ( NET_EXIT_SIGNAL )) == ( NET_EXIT_SIGNAL ))
		{
			break;
		}

		if ( mask & cfg->WatchDog_ReplyPort.vmp_SignalBit )
		{
			// Note 1: Stopping the timer, triggers a SetSignal() so we need to watch for Active
			// Note 2: Not anymore, its being redirected to CTRL+C, so Active should not be needed.
			// Note 3: But i'm leaving it in, as it should no do any harm

		 	if ( cfg->WatchDog_TimerActive )
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = mem_ASPrintF( "WatchDog: Kicking User" );
				}

				cfg->cfg_KickUser = 1;

				Signal( cfg->NetSend_Task, NET_EXIT_SIGNAL );
			}
		}
	}

	#ifdef DEBUG
	DebugPrintF( "WatchDog stopping\n" );
	#endif
}

// --

static S32 myProcess_Init( struct Config *cfg )
{
S32 retval;

	retval = FALSE;

	// --

	if ( ! MsgPort_Init( & cfg->WatchDog_ReplyPort ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Window AppPort" );
		goto bailout;
	}

//	cfg->WatchDog_ReplyPort = AllocSysObjectTags( ASOT_PORT,
//		TAG_END
//	);
//
//	if ( ! cfg->WatchDog_ReplyPort )
//	{
//		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Window AppPort" );
//		goto bailout;
//	}
//
//	cfg->WatchDog_ReplyPort.vmp_SignalBit = 1UL << cfg->WatchDog_ReplyPort->mp_SigBit;

	// -- Init TimeRequest

	memcpy( & cfg->WatchDog_TimerIOReq, TimerIOReq, sizeof( struct VNCTimeRequest ));

	cfg->WatchDog_TimerIOReq.Request.io_Message.mn_ReplyPort = & cfg->WatchDog_ReplyPort.vmp_MsgPort;

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --

static void myProcess_Free( struct Config *cfg )
{
	// --

	MsgPort_Free( & cfg->WatchDog_ReplyPort );

//	if ( cfg->WatchDog_ReplyPort )
//	{
//		FreeSysObject( ASOT_PORT, cfg->WatchDog_ReplyPort );
//		cfg->WatchDog_ReplyPort.vmp_SignalBit = 0;
//		cfg->WatchDog_ReplyPort = NULL;
//	}

	// --
}

// --

static void myServerProcess( void )
{
struct StartMessage *sm;
struct Config *Config;
struct Task *Parent;
struct Task *Self;
S32 stat;

	#ifdef DEBUG
	DebugPrintF( "WatchDog starting 1/2\n" );
	#endif

	//--------

	Self = FindTask( NULL );

	while( TRUE )
	{
		sm = Self->tc_UserData;

		if ( sm )
		{
			break;
		}

		#ifdef DEBUG
		DebugPrintF( "WatchDog starting delay\n" );
		#endif

		Delay( 2 );
	}

	#ifdef DEBUG
	DebugPrintF( "WatchDog starting 2/2\n" );
	#endif

	Parent = sm->Parent;
	Config = sm->Config;

	//--------

	Config->WatchDog_Exit = NULL;
	Config->cfg_WatchDogStatus = PROCESS_Starting;

	stat = myProcess_Init( Config );

	if ( stat )
	{
		Config->WatchDog_Task = Self;

		Config->cfg_WatchDogStatus = PROCESS_Running;

		// Set signal after Status
		Signal( Parent, SIGBREAKF_CTRL_E );

		// --

		#ifdef DEBUG
		DebugPrintF( "WatchDog entering main\n" );
		#endif

		SetTaskPri( Self, PRI_WATCHDOG );
		myProcess_Main( Config );
		SetTaskPri( Self, PRI_SHUTDOWN );

		#ifdef DEBUG
		DebugPrintF( "WatchDog exited main\n" );
		#endif

		// --

		Config->cfg_WatchDogStatus = PROCESS_Stopping;

		Parent = Config->WatchDog_Exit;

		Config->WatchDog_Task = NULL;
	}

	myProcess_Free( Config );

	Config->cfg_WatchDogStatus = PROCESS_Stopped;

	//--------

	#ifdef DEBUG
	DebugPrintF( "WatchDog stopped\n" );
	#endif

	if ( Parent )
	{
		Forbid();
		Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

S32 myStart_WatchDog( struct Config *cfg )
{
struct StartMessage msg;
struct Process *process;
S32 error;
U32 mask;
U32 wait;

	error = TRUE;

	#ifdef DEBUG
	DebugPrintF( "myStart_WatchDog\n" );
	#endif

	if ( DoVerbose > 2 )
	{
		printf( "myStart_WatchDog\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_WatchDogStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_WatchDogStatus == PROCESS_Stopping ))
	{
		Delay( 2 );
	}

	if ( cfg->cfg_WatchDogStatus == PROCESS_Stopped )
	{
		msg.Parent = FindTask( NULL );
		msg.Config = cfg;

		#ifdef _AOS4_

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : WatchDog Process",
			NP_Priority,	PRI_STARTUP,
			NP_Entry,		myServerProcess,
			NP_Child,		TRUE,
			NP_UserData,	& msg,
			TAG_END 
		);

		#else

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : WatchDog Process",
			NP_Priority,	PRI_STARTUP,
			NP_Entry,		myServerProcess,
			TAG_END 
		);

		#endif

		if ( process )
		{
			#ifndef _AOS4_
			process->pr_Task.tc_UserData = & msg;
			#endif

			wait = SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_F ;

			while( TRUE )
			{
				mask = Wait( wait );

				// CTRL+E is meaning every thing is okey
				if ( mask & SIGBREAKF_CTRL_E )
				{
					break;
				}

				// CTRL+F is means failure
				if ( mask & SIGBREAKF_CTRL_F )
				{
					break;
				}
			}
		}
	}

	if ( cfg->cfg_WatchDogStatus == PROCESS_Running )
	{
		error = FALSE;
	}

	return( error );
}

// --

void myStop_WatchDog( struct Config *cfg )
{
U32 mask;
S32 cnt;

	#ifdef DEBUG
	DebugPrintF( "myStop_WatchDog\n" );
	#endif

	if ( DoVerbose > 2 )
	{
		printf( "myStop_WatchDog\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	if ( cfg->cfg_WatchDogStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_WatchDogStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_WatchDogStatus == PROCESS_Stopping ))
		{
			Delay( 2 );
		}

		if ( cfg->cfg_WatchDogStatus == PROCESS_Running )
		{
			cfg->WatchDog_Exit = FindTask( NULL );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "WatchDog: Sending CTRL+D+E\n" );
			}

			// Clear CTRL+F
			SetSignal( 0L, SIGBREAKF_CTRL_F );

			// Send Break Signal(s)
			Signal( cfg->WatchDog_Task, NET_EXIT_SIGNAL );

			cnt = 0;

			while( TRUE )
			{
				mask = CheckSignal( SIGBREAKF_CTRL_F );

				if ( mask & SIGBREAKF_CTRL_F )
				{
					break;
				}
				else
				{
					Delay( 5 );

					if ( ! ( ++cnt % 50 ))
					{
						Signal( cfg->WatchDog_Task, NET_EXIT_SIGNAL );
//						printf( "myStop_WatchDog still waiting : %d\n", cnt );
						SHELLBUF_PRINTF( "\rmyStop_WatchDog still waiting : %d", cnt );
//						fflush( stdout );
					}
				}
			}

			if ( cnt >= 50 )
			{
				printf( "\n" );
			}
		}
	}
}

// --

void WatchDog_StartTimer( struct Config *cfg )
{
	if ( cfg->WatchDog_TimerActive )
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "Error: WatchDog allready running\n" );
		#endif
		goto bailout;
	}

	if ( ! cfg->cfg_Active_Settings.SendWatchDog )
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "Error: WatchDog do not Zero Seconds\n" );
		#endif
		goto bailout;
	}

	cfg->WatchDog_TimerActive = TRUE;

	// -- Add Request

	cfg->WatchDog_TimerIOReq.Request.io_Command = TR_ADDREQUEST;
	cfg->WatchDog_TimerIOReq.Time.Seconds		= cfg->cfg_Active_Settings.SendWatchDog;
	cfg->WatchDog_TimerIOReq.Time.Microseconds	= 0;

	SendIO( (PTR) & cfg->WatchDog_TimerIOReq );

bailout:

	return;
}

// --

void WatchDog_StopTimer( struct Config *cfg )
{
U8 bitnr;

	if ( ! cfg->WatchDog_TimerActive )
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "Error: WatchDog not running\n" );
		#endif
		goto bailout;
	}

	if ( ! cfg->cfg_Active_Settings.SendWatchDog )
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "Error: WatchDog do not Zero Seconds\n" );
		#endif
		goto bailout;
	}

	cfg->WatchDog_TimerActive = FALSE;

	// -- Remove Request
	// Note: Stopping the Timer, triggers a set Signal()
	// Redirect Timer signal to CTRL+C

	bitnr = cfg->WatchDog_ReplyPort.vmp_MsgPort.mp_SigBit;
	/* - */ cfg->WatchDog_ReplyPort.vmp_MsgPort.mp_SigBit = SIGBREAKB_CTRL_C;

	AbortIO( (PTR) & cfg->WatchDog_TimerIOReq );
	WaitIO(  (PTR) & cfg->WatchDog_TimerIOReq );

	cfg->WatchDog_ReplyPort.vmp_MsgPort.mp_SigBit = bitnr;

bailout:

	return;
}

// --
