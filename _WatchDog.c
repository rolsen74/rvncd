
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

static void myProcess_Main( struct Config *cfg )
{
uint32 mask;

	while( TRUE )
	{
		// Note: CTRL+C used by Timer, it should be ignored
		mask = IExec->Wait( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E | cfg->WatchDog_ReplyPortBit );

		if (( mask & ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) == ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E ))
		{
			break;
		}

		if ( mask & cfg->WatchDog_ReplyPortBit )
		{
			// Note 1: Stopping the timer, triggers a SetSignal() so we need to watch for Active
			// Note 2: Not anymore, its being redirected to CTRL+C, so Active should not be needed.
			// Note 3: But i'm leaving it in, as it should no do any harm

		 	if ( cfg->WatchDog_TimerActive )
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = myASPrintF( "WatchDog: Kicking User" );
				}

				cfg->cfg_KickUser = 1;

				IExec->Signal( cfg->NetSend_Task, SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E );
			}
		}
	}
}

// --

static int myProcess_Init( struct Config *cfg )
{
int retval;

	retval = FALSE;

	// --

	cfg->WatchDog_ReplyPort = IExec->AllocSysObjectTags( ASOT_PORT,
		TAG_END
	);

	if ( ! cfg->WatchDog_ReplyPort )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Window AppPort" );
		goto bailout;
	}

	cfg->WatchDog_ReplyPortBit = 1UL << cfg->WatchDog_ReplyPort->mp_SigBit;

	// -- Init TimeRequest

	memcpy( & cfg->WatchDog_TimerIOReq, TimerIOReq, sizeof( struct TimeRequest ));

	cfg->WatchDog_TimerIOReq.Request.io_Message.mn_ReplyPort = cfg->WatchDog_ReplyPort;

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --

static void myProcess_Free( struct Config *cfg )
{
	// --

	if ( cfg->WatchDog_ReplyPort )
	{
		IExec->FreeSysObject( ASOT_PORT, cfg->WatchDog_ReplyPort );
		cfg->WatchDog_ReplyPortBit = 0;
		cfg->WatchDog_ReplyPort = NULL;
	}

	// --
}

// --

static void myServerProcess( void )
{
struct StartMessage *sm;
struct Config *Config;
struct Task *Parent;
struct Task *Self;
int stat;

	//--------

	Self = IExec->FindTask( NULL );

	sm = Self->tc_UserData;

	Parent = sm->Parent;
	Config = sm->Config;

	//--------

	Config->cfg_WatchDogStatus = PROCESS_Starting;

	stat = myProcess_Init( Config );

	if ( stat )
	{
		Config->WatchDog_Task = Self;

		Config->cfg_WatchDogStatus = PROCESS_Running;

		// Set signal after Status
		IExec->Signal( Parent, SIGBREAKF_CTRL_E );

		// --

		myProcess_Main( Config );

		// --

		Config->cfg_WatchDogStatus = PROCESS_Stopping;

		Parent = Config->WatchDog_Exit;

		Config->WatchDog_Task = NULL;
	}

	myProcess_Free( Config );

	Config->cfg_WatchDogStatus = PROCESS_Stopped;

	//--------

	if ( Parent )
	{
		IExec->Forbid();
		IExec->Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

int myStart_WatchDog( struct Config *cfg )
{
struct StartMessage msg;
uint32 mask;
int error;

	error = TRUE;

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_WatchDogStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_WatchDogStatus == PROCESS_Stopping ))
	{
		IDOS->Delay( 2 );
	}

	if ( cfg->cfg_WatchDogStatus == PROCESS_Stopped )
	{
		msg.Parent = IExec->FindTask( NULL );
		msg.Config = cfg;

		if ( IDOS->CreateNewProcTags(
			NP_Name,		"WatchDog Process",
			NP_Priority,	1,
			NP_Entry,		myServerProcess,
			NP_Child,		TRUE,
			NP_UserData,	& msg,
			TAG_END ))
		{
			while( TRUE )
			{
				mask = IExec->Wait( SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_F );

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
uint32 mask;
int cnt;

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	if ( cfg->cfg_WatchDogStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_WatchDogStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_WatchDogStatus == PROCESS_Stopping ))
		{
			IDOS->Delay( 2 );
		}

		if ( cfg->cfg_WatchDogStatus == PROCESS_Running )
		{
			cfg->WatchDog_Exit = IExec->FindTask( NULL );

			IExec->Signal( cfg->WatchDog_Task, SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E );

			cnt = 0;

			while( TRUE )
			{
				mask = IDOS->CheckSignal( SIGBREAKF_CTRL_F );

				if ( mask & SIGBREAKF_CTRL_F )
				{
					break;
				}
				else
				{
					IDOS->Delay( 5 );

					if ( ! ( ++cnt % 50 ))
					{
						printf( "\rmyStop_WatchDog still waiting : %d", cnt );
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
		printf( "Error: WatchDog allready running\n" );
		goto bailout;
	}

	if ( ! cfg->cfg_Active_Settings.SendWatchDog )
	{
		printf( "Error: WatchDog do not Zero Seconds\n" );
		goto bailout;
	}

	cfg->WatchDog_TimerActive = TRUE;

	// -- Add Request

	cfg->WatchDog_TimerIOReq.Request.io_Command = TR_ADDREQUEST;
	cfg->WatchDog_TimerIOReq.Time.Seconds		= cfg->cfg_Active_Settings.SendWatchDog;
	cfg->WatchDog_TimerIOReq.Time.Microseconds	= 0;

	IExec->SendIO( (APTR) & cfg->WatchDog_TimerIOReq );

bailout:

	return;
}

// --

void WatchDog_StopTimer( struct Config *cfg )
{
uint8 bitnr;

	if ( ! cfg->WatchDog_TimerActive )
	{
		printf( "Error: WatchDog not running\n" );
		goto bailout;
	}

	if ( ! cfg->cfg_Active_Settings.SendWatchDog )
	{
		printf( "Error: WatchDog do not Zero Seconds\n" );
		goto bailout;
	}

	cfg->WatchDog_TimerActive = FALSE;

	// -- Remove Request
	// Note: Stopping the Timer, triggers a set Signal()
	// Redirect Timer signal to CTRL+C

	bitnr = cfg->WatchDog_ReplyPort->mp_SigBit;
	/* - */ cfg->WatchDog_ReplyPort->mp_SigBit = SIGBREAKB_CTRL_C;

	IExec->AbortIO( (APTR) & cfg->WatchDog_TimerIOReq );
	IExec->WaitIO(  (APTR) & cfg->WatchDog_TimerIOReq );

	cfg->WatchDog_ReplyPort->mp_SigBit = bitnr;

bailout:

	return;
}

// --
