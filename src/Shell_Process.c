
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#define MAX_TEMP 32

static void myProcess_Main( struct Config *cfg UNUSED )
{
char buf[MAX_TEMP];
U32 mask;
U32 wait;
S32 len;

	ShellBuf_Enable = TRUE;

	wait = NET_EXIT_SIGNAL;

	#ifdef DEBUG
	DebugPrintF( "Shell started\n" );
	#endif

	while( TRUE )
	{
		mask = CheckSignal( wait );

		if (( mask & ( NET_EXIT_SIGNAL )) == ( NET_EXIT_SIGNAL ))
		{
			break;
		}

		len = SHELLBUF_GETBYTES( buf, MAX_TEMP );

		if ( len > 0 )
		{
			printf( "%s", buf );
		}
		else
		{
			Delay( 15 );
		}
	}

	#ifdef DEBUG
	DebugPrintF( "Shell stopping\n" );
	#endif

	ShellBuf_Enable = FALSE;
}

// --

static S32 myProcess_Init( struct Config *cfg UNUSED )
{
S32 retval;

	retval = FALSE;


	retval = TRUE;

// bailout:

	return( retval );
}

// --

static void myProcess_Free( struct Config *cfg UNUSED )
{

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
	DebugPrintF( "Shell starting 1/2\n" );
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
		DebugPrintF( "Shell starting delay\n" );
		#endif

		Delay( 2 );
	}

	#ifdef DEBUG
	DebugPrintF( "Shell starting 2/2\n" );
	#endif

	Parent = sm->Parent;
	Config = sm->Config;

	//--------

	Config->cfg_ShellBufStatus = PROCESS_Starting;

	stat = myProcess_Init( Config );

	if ( stat )
	{
		Config->ShellBuf_Task = Self;

		Config->cfg_ShellBufStatus = PROCESS_Running;

		// Set signal after Status
		Signal( Parent, SIGBREAKF_CTRL_E );

		// --

		#ifdef DEBUG
		DebugPrintF( "Shell entering main\n" );
		#endif

		SetTaskPri( Self, PRI_SHELL );
		myProcess_Main( Config );
		SetTaskPri( Self, PRI_SHUTDOWN );

		#ifdef DEBUG
		DebugPrintF( "Shell exited main\n" );
		#endif

		// --

		Config->cfg_ShellBufStatus = PROCESS_Stopping;

		Parent = Config->ShellBuf_Exit;

		Config->ShellBuf_Task = NULL;
	}

	myProcess_Free( Config );

	Config->cfg_ShellBufStatus = PROCESS_Stopped;

	//--------

	#ifdef DEBUG
	DebugPrintF( "Shell stopped\n" );
	#endif

	if ( Parent )
	{
		Forbid();
		Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

S32 myStart_ShellBuf( struct Config *cfg )
{
struct StartMessage msg;
struct Process *process;
S32 error;
U32 mask;
U32 wait;

	error = TRUE;

	#ifdef DEBUG
	DebugPrintF( "myStart_ShellBuf\n" );
	#endif

	if ( DoVerbose > 2 )
	{
		printf( "myStart_ShellBuf\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_ShellBufStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_ShellBufStatus == PROCESS_Stopping ))
	{
		Delay( 2 );
	}

	if ( cfg->cfg_ShellBufStatus == PROCESS_Stopped )
	{
		msg.Parent = FindTask( NULL );
		msg.Config = cfg;

		#ifdef _AOS4_

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : ShellBuf Process",
			NP_Priority,	PRI_STARTUP,
			NP_Entry,		myServerProcess,
			NP_Child,		TRUE,
			NP_UserData,	& msg,
			TAG_END
		);

		#else

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : ShellBuf Process",
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

	if ( cfg->cfg_ShellBufStatus == PROCESS_Running )
	{
		error = FALSE;
	}

	return( error );
}

// --

void myStop_ShellBuf( struct Config *cfg )
{
U32 mask;
S32 cnt;

	#ifdef DEBUG
	DebugPrintF( "myStop_ShellBuf\n" );
	#endif

	if ( DoVerbose > 2 )
	{
		printf( "myStop_ShellBuf\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	if ( cfg->cfg_ShellBufStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_ShellBufStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_ShellBufStatus == PROCESS_Stopping ))
		{
			Delay( 2 );
		}

		if ( cfg->cfg_ShellBufStatus == PROCESS_Running )
		{
			cfg->ShellBuf_Exit = FindTask( NULL );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "ShellBuf: Sending CTRL+D+E\n" );
			}

			// Clear CTRL+F
			SetSignal( 0L, SIGBREAKF_CTRL_F );

			// Send Break Signal(s)
			Signal( cfg->ShellBuf_Task, NET_EXIT_SIGNAL );

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
						Signal( cfg->ShellBuf_Task, NET_EXIT_SIGNAL );
						printf( "ShellBuf still waiting : %d\n", cnt );
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
