
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

STR							OrgTaskName			= NULL;
struct Task *				Program_Task		= NULL;

S32							Program_Running		= FALSE;
U32							Program_SigFlags	= 0;
U32							Program_SigMask		= 0;
S32							Program_Signal		= 0;

static const USED STR		MyVersion			= VERSTAG;

struct VNCList 				LogStringList;
struct VNCList 				KeyLoggerList;
struct CommandEncoding *	ActiveEncoding		= NULL;

// --


int main( S32 argc, STR *argv )
{
struct Config *cfg;
struct Task *task;
U32 mask;
BPTR olddir;
BPTR newdir;

	cfg = NULL;

	// -- Program Duplication Check

	Forbid();

	Program_Task = FindTask( NULL );
	OrgTaskName = Program_Task->tc_Node.ln_Name;
	Program_Task->tc_Node.ln_Name = "<NULL>";

	task = FindTask( "rVNCd" );

	if ( task )
	{
		Program_Task->tc_Node.ln_Name = OrgTaskName;
		Signal( task, SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E );
		Permit();
		// Can't use SHELLPRINT here
		printf( "rVNCd is Allready running\n" );
		return( 0 );
	}

	Program_Task->tc_Node.ln_Name = "rVNCd";

	Permit();

	// --
	// If running from CLI, DoVerbose defaults to 1
	// so WB don't get the 'Loading '%s' config..' message pr default

	if ( ((struct Process *)Program_Task)->pr_CLI )
	{
		DoVerbose = 1;
	}

	// --

	Program_Signal = AllocSignal( -1 );
	Program_SigMask = 1UL << Program_Signal;

	// --

	// Disable CTRL+C in c lib
	signal( SIGINT, SIG_IGN );

	// Init Random
	srand( time( NULL ));

	// Set Default Dir to progdir:
	newdir = Lock( "progdir:", ACCESS_READ );
	olddir = SETCURRENTDIR( newdir );

	List_Init( & KeyLoggerList );
	List_Init( & LogStringList );

	InitSemaphore( & ActionSema );

	// ----
	// Start Shell Buffer 
	if ( ! SHELLBUF_INIT())
	{
		goto bailout;
	}

	// ----
	// Parse Config

	cfg = Config_Init();

	if ( ! cfg )
	{
		SHELLBUF_PRINTF( "Error creating Config struct\n" );
		goto bailout;
	}

	if ( Config_Parse( cfg, argc, argv ))
	{
		SHELLBUF_PRINTF( "Error parsing Config file\n" );
		goto bailout;
	}

	// ----
	// Commands

	if ( ! Cmd_Init( cfg ))
	{
		goto bailout;
	}

	// ----
	// Basic System

	if ( ! System_Startup( cfg ))
	{
		SHELLBUF_PRINTF( "System_Startup() failed\n" );
		goto bailout;
	}

	if ( cfg->cfg_LogProgramStart )
	{
		Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Program : v%ld.%ld : Starting up .. ", VERSION, REVISION );
	}

	// ----
	// Start Shell Buffer Process
	if ( myStart_ShellBuf( cfg ))
	{
		SHELLBUF_PRINTF( "myStart_ShellBuf() failed\n" );
		goto bailout;
	}


	// ----
	// Application

	#ifdef HAVE_APPLIB

	if ( ! App_Init( cfg, ( argc == 0 ) ? (PTR) argv : NULL ))
	{
		SHELLBUF_PRINTF( "App_Init() failed\n" );
		goto bailout;
	}

	#endif // HAVE_APPLIB

	// ----
	// GUI

	if (( ! cfg->cfg_Program_DisableGUI ) && ( ActiveGUI.GUI_Init ))
	{
		if ( ! ActiveGUI.GUI_Init( cfg ))
		{
			SHELLBUF_PRINTF( "GUI_Init() failed\n" );
			goto bailout;
		}
	}

	#ifndef GUI_MUI
	// MUI have ARexx+Cx buildin

	// ----
	// ARexx
	#ifdef HAVE_AREXX

	if ( ! cfg->cfg_Program_DisableARexx )
	{
		if ( ! ARexx_Init( cfg ))
		{
			SHELLBUF_PRINTF( "ARexx_Init() failed\n" );
			goto bailout;
		}
	}

	#endif // HAVE_AREXX

	// ----
	// CxBroker
	#ifdef HAVE_CXBROKER

	if ( ! cfg->cfg_Program_DisableCxBroker )
	{
		if ( ! CxBroker_Init( cfg ))
		{
			SHELLBUF_PRINTF( "CxBroker_Init() failed\n" );
			goto bailout;
		}
	}

	#endif // HAVE_CXBROKER
	// --
	#endif // GUI_MUI

	// -- 
	// Patch Beep
	if ( 1 )
	{
		Beep_Patch_Install( cfg );
	}

	// ----
	// Start GUI

	if ( cfg->cfg_ActionsProgramStartEnable )
	{
		DoAction_ProgramStart( cfg );
	}

	#ifdef WIN_LAST
	if ( ! cfg->cfg_Program_DisableGUI )
	{

		#ifdef WIN_Main
		cfg->cfg_WinData[WIN_Main].Status = cfg->MainWinState;
		#endif
	
		for( S32 cnt=0 ; cnt<WIN_LAST ; cnt++ )
		{
			if (( cfg->cfg_WinData[cnt].OpenWin ) 
			&&	( cfg->cfg_WinData[cnt].Status != WINSTAT_Close ))
			{
				cfg->cfg_WinData[cnt].OpenWin( cfg );
			}
		}
	}
	#endif

	// ----
	// Start Server

	if ( cfg->AutoStart )
	{
		SHELLBUF_PRINTF( "Start Server\n" );
		StartServer( cfg );
	}
	else
	{
		SHELLBUF_PRINTF( "Skip Server AutoStart\n" );
	}

	// ----
	// Start Timer

	Timer_Start();


	// ----
	// Main Loop

	Program_Running = TRUE;

	SHELLBUF_PRINTF( "MainLoop\n" );

	while( Program_Running )
	{
		mask =	SIGBREAKF_CTRL_D |
				SIGBREAKF_CTRL_E |
				SIGBREAKF_CTRL_C |

				#ifdef HAVE_APPLIB
				AppBit | 
				#endif // HAVE_APPLIB

				#ifndef GUI_MUI
				#ifdef HAVE_AREXX
				ARexxBit | 
				#endif // HAVE_AREXX
				#ifdef HAVE_CXBROKER
				CxBrokerMsgPort.vmp_SignalBit |
				#endif // HAVE_CXBROKER
				#endif // GUI_MUI

				Program_SigMask |
				ActiveGUI.gui_SignalBits |
				CmdMsgPort.vmp_SignalBit |
				TimerMsgPort.vmp_SignalBit;

		mask = Wait( mask );

		#ifndef _AOS3_
		// VBCC's clib do not like we use CTRL+C
		// disabling it for now, use gui/cx too quit
		if ( mask & SIGBREAKF_CTRL_C )
		{
			Func_Quit( cfg );
		}
		#endif

		if (( mask & ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) == ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E ))
		{
			// Someone tryed to start us again.

			#ifdef WIN_Main
			cfg->cfg_WinData[WIN_Main].Status = WINSTAT_Open;

			if ( cfg->cfg_WinData[WIN_Main].OpenWin )
			{
				 cfg->cfg_WinData[WIN_Main].OpenWin( cfg );
			}
			#endif // WIN_Main
		}

		if ( mask & Program_SigMask )
		{
			U32 flags = Program_SigFlags;
			Program_SigFlags = 0;

			if (( flags & PRGFLAG_Refresh_Settings ) && ( ActiveGUI.Check_Settings )) 
			{
				ActiveGUI.Check_Settings( cfg );
			}

			if (( flags & PRGFLAG_Refresh_Status ) && ( ActiveGUI.Server_Status_Change ))
			{
				ActiveGUI.Server_Status_Change( cfg, cfg->cfg_ServerStatus );
			}

		}

		#ifdef HAVE_APPLIB
		if ( mask & AppBit )
		{
			App_Handle( cfg );
		}
		#endif // HAVE_APPLIB

		#ifndef GUI_MUI
		#ifdef HAVE_AREXX
		if ( mask & ARexxBit )
		{
			ARexx_Handle( cfg );
		}
		#endif // HAVE_AREXX
		#ifdef HAVE_CXBROKER
		if ( mask & CxBrokerMsgPort.vmp_SignalBit )
		{
			CxBroker_Handle( cfg );
		}
		#endif // HAVE_CXBROKER
		#endif // GUI_MUI

		if ( mask & ActiveGUI.gui_SignalBits )
		{
			if ( ActiveGUI.GUI_Handle )
			{
				ActiveGUI.GUI_Handle( cfg );
			}
		}

		if ( mask & CmdMsgPort.vmp_SignalBit )
		{
			Cmd_Handle( cfg );
		}

		if ( mask & TimerMsgPort.vmp_SignalBit )
		{
			Timer_Handle( cfg );
		}
	}

	// --

	SHELLBUF_PRINTF( "Stop 0\n" );
	SHELLBUF_FLUSH();

	Timer_Stop();

	// -- --

	SHELLBUF_PRINTF( "Stop 1\n" );
	SHELLBUF_FLUSH();

	StopServer( cfg );

	// -- --

bailout:

	SHELLBUF_PRINTF( "Stop 2\n" );
	SHELLBUF_FLUSH();

	// --

	if ( cfg )
	{
		Beep_Patch_Remove( cfg );

		if ( cfg->cfg_ActionsProgramStopEnable )
		{
			DoAction_ProgramStop( cfg );
		}

		if ( cfg->cfg_LogProgramStop )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Program : Shutting down .." );
		}

		for( S32 cnt=0 ; cnt<WIN_LAST ; cnt++ )
		{
			if (( cfg->cfg_WinData[cnt].CloseWin )
			&&	( cfg->cfg_WinData[cnt].Status != WINSTAT_Close ))
			{
				cfg->cfg_WinData[cnt].CloseWin( cfg );
			}
		}

		#ifndef GUI_MUI
		#ifdef HAVE_CXBROKER
		CxBroker_Free( cfg );
		#endif // HAVE_CXBROKER
		#ifdef HAVE_AREXX
		ARexx_Free( cfg );
		#endif // HAVE_AREXX
		#endif // GUI_MUI

		if ( ActiveEncoding )
		{
			mem_Free( ActiveEncoding );
			ActiveEncoding = NULL;
		}

		#ifdef HAVE_APPLIB
		App_Free();
		#endif // HAVE_APPLIB
	
		Cmd_Free();

		if ( ActiveGUI.GUI_Free )
		{
			ActiveGUI.GUI_Free( cfg );
		}

		// ----
		// Stop Shell Buffer Process
		myStop_ShellBuf( cfg );

		// Also Free's config
		System_Shutdown( cfg );
	}

	// ----
	// Release Buffer
	SHELLBUF_FREE();

	// --

	FreeSignal( Program_Signal );

	// --

	SETCURRENTDIR( olddir );
	UnLock( newdir );

	Program_Task->tc_Node.ln_Name = OrgTaskName;

	return( 0 );
}

// --
