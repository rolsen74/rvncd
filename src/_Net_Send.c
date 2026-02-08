
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

static void myProcess_Main( struct Config *cfg )
{
struct UpdateNode *un;
enum UpdateStat stat;
U32 mask;

	// -- Exchange VNC Info

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF( "\nVNC Init [1/6]\n\n" );
	}

	if ( VNC_Version( cfg ))
	{
		goto bailout;
	}

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF( "\nVNC Init [2/6]\n\n" );
	}

	switch( cfg->NetSend_ClientProtocol )
	{
		case VNCProtocol_33:
		{
			if ( VNC_Authenticate_33( cfg ))
			{
				goto bailout;
			}
			break;
		}

		case VNCProtocol_37:
		{
			if ( VNC_Authenticate_37( cfg ))
			{
				goto bailout;
			}
			break;
		}

		default:
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Invalid Client Protocol type (%d)", cfg->NetSend_ClientProtocol );
			goto bailout;
		}
	}

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF( "\nVNC Init [3/6]\n\n" );
	}

	if ( VNC_ClientInit( cfg ))
	{
		goto bailout;
	}

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF( "VNC Init [4/6]\n\n" );
	}

	if ( VNC_ServerInit( cfg ))
	{
		goto bailout;
	}

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF( "VNC Init [5/6]\n\n" );
	}

	// -- NetRead can now Read Requests

	cfg->NetRead_Idle = FALSE;

	// --

	// Clear ClipBoard signal
	SetSignal( 0, cfg->NetSend_ClipBoardSignal );

	if ( DoVerbose > 0 )
	{
		Log_PrintF( cfg, LOGTYPE_Info, "Client Connected" );
		SHELLBUF_PRINTF( "VNC Init [6/6] -- Rock'n Roll\n\n" );
	}

	// --

	cfg->cfg_KickUser = 0;

	while(	( cfg->cfg_ClientRunning ) 
	&&		( ! cfg->cfg_ServerShutdown )
	&&		( ! cfg->cfg_KickUser ))
	{
		if ( cfg->cfg_Active_Settings.SendWatchDog )
		{
			WatchDog_StartTimer( cfg );
		}

		ObtainSemaphore( & cfg->Server_UpdateSema );

		un = List_GetHead( & cfg->Server_UpdateList );

		ReleaseSemaphore( & cfg->Server_UpdateSema );

		if ( un )
		{
			switch( un->un_Type )
			{
				case UT_UpdateRequest:
				{
					stat = NewUpdateReq_Msg( cfg, un );
					break;
				}

				case UT_PixelMessage:
				{
					stat = NewPixelFmt_Msg( cfg, un );
					break;
				}

				case UT_EncodingMsg:
				{
					stat = NewEncoding_Msg( cfg, un );
					break;
				}

				case UT_Beep:
				{
					stat = NewBeep_Msg( cfg, un );
					break;
				}

				default:
				{
					stat = US_Wait;
//					stat = US_Error;
					break;
				}
			}

			// --
			// Move UpdateNode to Free List

			if ( stat == US_Handled )
			{
				if ( un->un_Buffer )
				{
					mem_Free( un->un_Buffer );
					un->un_Buffer = NULL;
				}

				ObtainSemaphore( & cfg->Server_UpdateSema );

				Remove( & un->un_Node );
				List_AddTail( & cfg->Server_UpdateFree, & un->un_Node );

				ReleaseSemaphore( & cfg->Server_UpdateSema );
			}
		}
		else
		{
			stat = US_Wait;
		}

		if ( cfg->cfg_Active_Settings.SendWatchDog )
		{
			WatchDog_StopTimer(	cfg );
		}

		/**/ if ( stat == US_Error ) // Error
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = mem_ASPrintF( "Internal: NewBufferUpdate Failed" );
			}
			break;
		}
		else if ( stat == US_Wait ) // No Change
		{
			struct VNCTimeVal start;
			struct VNCTimeVal stop;

			GetSysTime( (PTR) & start );
			Delay( 2 );
			GetSysTime( (PTR) & stop );

			SubTime( (PTR) & stop, (PTR) & start );

			Disable();
			AddTime( (PTR) & cfg->SessionStatus.si_Pixel_Time_Wait, (PTR) & stop );
			Enable();
		}

		// --

		mask = CheckSignal( NET_EXIT_SIGNAL | cfg->NetSend_ClipBoardSignal );

		if ((( mask & ( NET_EXIT_SIGNAL )) == ( NET_EXIT_SIGNAL )) && ( cfg->NetSend_Exit ))
		{
			cfg->cfg_ServerShutdown = TRUE;
			break;
		}

		if ( mask & cfg->NetSend_ClipBoardSignal )
		{
			Send_Handle_ClipBoard( cfg );
		}
	}

bailout:

	// --

	ObtainSemaphore( & cfg->Server_UpdateSema );

	while( TRUE )
	{
		un = List_RemHead( & cfg->Server_UpdateList );

		if ( ! un )
		{
			break;
		}

		if ( un->un_Buffer )
		{
			mem_Free( un->un_Buffer );
			un->un_Buffer = NULL;
		}

		List_AddTail( & cfg->Server_UpdateFree, & un->un_Node );
	}

	ReleaseSemaphore( & cfg->Server_UpdateSema );

	// --

	myPrintSessionInfo( cfg );

	cfg->cfg_ClientRunning = FALSE;
}

// --

static S32 myProcess_Init( struct Config *cfg )
{
PTR buf;
S32 retval;
S32 size;
S32 cnt;

	retval = FALSE;

	// --

	cfg->cfg_ClientRunning = TRUE;
	cfg->NetSend_ClientSocket = -1;
	cfg->NetSend_ClipBoardSignalNr = -1;
	cfg->GfxRead_Screen_SupportChange = FALSE;

	// --

	Log_PrintF( cfg, LOGTYPE_Info, "Client Starting up .. " );

	// --

	// 1x Tile chunky .. max 256x256x4 + 4096
	size  = cfg->GfxRead_Screen_TileSize * cfg->GfxRead_Screen_TileSize * 4;
	size += ( 1 * 4096 ); // Space for Headers
	size +=  4095; // Round up to 4k page
	size &= ~4095;

	#ifdef _AOS4_

	buf = AllocVecTags( size,
		AVT_PhysicalAlignment, 4096,
		AVT_Contiguous, TRUE,
		AVT_Alignment, 4096,
		AVT_Lock, TRUE,
		TAG_END
	);

	#else

	buf = AllocVec( size, MEMF_ANY );

	#endif

	if ( ! buf )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory (%ld Bytes)", size );
		goto bailout;
	}

	cfg->NetSend_SendBuffer = buf;
	cfg->NetSend_SendBufferSize = size;

	// --

	// 1x Tile chunky .. max 256x256x4
	size  = cfg->GfxRead_Screen_TileSize * cfg->GfxRead_Screen_TileSize * 4;
	size +=  4095; // Round up to 4k page
	size &= ~4095;

	#ifdef _AOS4_

	buf = AllocVecTags( size,
		AVT_PhysicalAlignment, 4096,
		AVT_Contiguous, TRUE,
		AVT_Alignment, 4096,
		AVT_Lock, TRUE,
		TAG_END
	);

	#else

	buf = AllocVec( size, MEMF_ANY );

	#endif

	if ( ! buf )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory (%ld Bytes)", size );
		goto bailout;
	}

	cfg->NetSend_EncBuffer = buf;
	cfg->NetSend_EncBufferSize = size;

	// --

	if ( cfg->UserCount == 0 )
	{
		cfg->GfxRead_Encode_ActivePixelSet = FALSE;
		memset( & cfg->NetSend_Encodings1, 0, ENCODE_LAST * sizeof( U8 ) );
		memset( & cfg->NetSend_Encodings2, 0, ENCODE_LAST * sizeof( struct myEncNode ) );
		memset( & cfg->GfxRead_Encode_ActivePixel, 100, sizeof( struct PixelMessage ));

		// Always have Raw Enabled
		cfg->NetSend_Encodings1[ENCODE_Raw] = TRUE;
		cfg->NetSend_Encodings2[ENCODE_Raw].Pos = 0;
		cfg->NetSend_Encodings2[ENCODE_Raw].Enabled = TRUE;
		cfg->NetSend_Encodings2[ENCODE_Raw].EncType = TRUE;

		if ( cfg->cfg_Active_Settings.DisableBlanker )
		{
			Func_Blanker_Disable( cfg );
		}

		if ( cfg->cfg_Active_Settings.SendClipboard )
		{
			if ( Send_Install_ClipHook( cfg ))
			{
				goto bailout;
			}
		}
	}

	// -- Wakeup GfxRead

	Forbid();
	cfg->cfg_UserConnectSignal = TRUE;
	cfg->UserCount++;
	Permit();

	for( cnt=0 ; cnt<50 ; cnt++ )
	{
		if ( cfg->cfg_UserConnectSignal )
		{
			Delay( 2 );
		}
		else
		{
			break;
		}
	}

	// --

	if ( Tile_SendBuffer_Init( cfg ))
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "Tile_SendBuffer_Init() failed\n" );
		#endif
		goto bailout;
	}

	// --

	if ( Send_Open_Socket( cfg ))
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "Send_Open_Socket() failed\n" );
		#endif
		goto bailout;
	}

	// --

	if ( myStart_Net_Read( cfg ))
	{
		#ifdef DEBUG
		SHELLBUF_PRINTF( "myStart_Net_Read() failed\n" );
		#endif
		goto bailout;
	}

	if ( cfg->cfg_Active_Settings.SendWatchDog )
	{
		if ( myStart_WatchDog( cfg ))
		{
			#ifdef DEBUG
			SHELLBUF_PRINTF( "myStart_WatchDog() failed\n" );
			#endif
			goto bailout;
		}
	}

	// -- Setup Z Library

	#ifdef HAVE_ZLIB
	ZLib_Setup( cfg );
	#endif // HAVE_ZLIB

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --

static void myProcess_Free( struct Config *cfg )
{
S32 cnt;

	// --

	Log_PrintF( cfg, LOGTYPE_Info, "Shutting down .. " );

	// --

	#ifdef HAVE_ZLIB
	ZLib_Cleanup( cfg );
	#endif // HAVE_ZLIB

	myStop_WatchDog( cfg );

	myStop_Net_Read( cfg );

	Send_Close_Socket( cfg );

	Tile_SendBuffer_Free( cfg );

	// --

	if ( cfg->NetSend_EncBuffer )
	{
		#ifdef _AOS4_
		UnlockMem( cfg->NetSend_EncBuffer, cfg->NetSend_EncBufferSize );
		#endif
		FreeVec( cfg->NetSend_EncBuffer );
		cfg->NetSend_EncBuffer = NULL;
	}

	if ( cfg->NetSend_SendBuffer )
	{
		#ifdef _AOS4_
		UnlockMem( cfg->NetSend_SendBuffer, cfg->NetSend_SendBufferSize );
		#endif
		FreeVec( cfg->NetSend_SendBuffer );
		cfg->NetSend_SendBuffer = NULL;
	}

	// --

	if ( cfg->UserCount == 1 )
	{
		if ( cfg->cfg_Active_Settings.DisableBlanker )
		{
			Func_Blanker_Enable( cfg );
		}

		if ( cfg->cfg_Active_Settings.SendClipboard )
		{
			Send_Remove_ClipHook( cfg );
		}

		// --

		Forbid();
		cfg->cfg_UserDisconnectSignal = TRUE;
		cfg->UserCount--;
		Permit();

		for( cnt=0 ; cnt<50 ; cnt++ )
		{
			if ( cfg->cfg_UserDisconnectSignal )
			{
				Delay( 2 );
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		Forbid();
		cfg->UserCount--;
		Permit();
	}

	// --
}

// --

static void myServerProcess( void )
{
struct CommandSession *msg;
struct StartMessage *sm;
struct Config *Config;
struct Task *Parent;
struct Task *Self;
S32 stat;

	//--------

	Self = FindTask( NULL );

	while( TRUE )
	{
		sm = Self->tc_UserData;

		if ( sm )
		{
			break;
		}

		Delay( 2 );
	}

	Parent = sm->Parent;
	Config = sm->Config;

	//--------

	Config->NetSend_Exit = NULL;
	Config->cfg_NetSendStatus = PROCESS_Starting;

	stat = myProcess_Init( Config );

	if ( stat )
	{
		Config->NetSend_Task = Self;
		Config->cfg_NetSendStatus = PROCESS_Running;

		// Set signal after Status
		Signal( Parent, SIGBREAKF_CTRL_E );

		// --

		msg = mem_AllocClr( sizeof( struct CommandSession ) );

		if ( msg )
		{
			msg->cs_Command = CMD_Session;
			msg->cs_Starting = 1;
		
			MsgPort_PutMsg( & CmdMsgPort, & msg->cs_Message );
		}

		// --

		if ( Config->cfg_ActionsUserConnectEnable )
		{
			DoAction_UserConnect( Config );
		}

		if ( Config->cfg_NetReason )
		{
			mem_Free( Config->cfg_NetReason );
			Config->cfg_NetReason = NULL;
		}

		// --

		SetTaskPri( Self, PRI_NETSEND );
		myProcess_Main( Config );
		SetTaskPri( Self, PRI_SHUTDOWN );

		// --

		if ( Config->cfg_NetReason )
		{
			Log_PrintF( Config, LOGTYPE_Info, "Client Shutdown .. %s", Config->cfg_NetReason );
			mem_Free( Config->cfg_NetReason );
			Config->cfg_NetReason = NULL;
		}

		// --

		msg = mem_AllocClr( sizeof( struct CommandSession ) );

		if ( msg )
		{
			msg->cs_Command = CMD_Session;
			msg->cs_Starting = 0;
		
			MsgPort_PutMsg( & CmdMsgPort, & msg->cs_Message );
		}

		// --

		if ( Config->cfg_ActionsUserDisconnectEnable )
		{
			DoAction_UserDisconnect( Config );
		}

		// --

		Config->cfg_NetSendStatus = PROCESS_Stopping;

		Parent = Config->NetSend_Exit;

		Config->NetSend_Task = NULL;
	}

	myProcess_Free( Config );

	Config->cfg_NetSendStatus = PROCESS_Stopped;

	//--------

	if ( Parent )
	{
		Forbid();
		Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

S32 myStart_Net_Send( struct Config *cfg )
{
struct StartMessage msg;
struct Process *process;
S32 error;
U32 mask;
U32 wait;

	error = TRUE;

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "myStart_Net_Send\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_NetSendStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_NetSendStatus == PROCESS_Stopping ))
	{
		Delay( 2 );
	}

	if ( cfg->cfg_NetSendStatus == PROCESS_Stopped )
	{
		msg.Parent = FindTask( NULL );
		msg.Config = cfg;

		#ifdef _AOS4_

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : Net Send Process",
			NP_Priority,	PRI_STARTUP,
			NP_Entry,		myServerProcess,
			NP_Child,		TRUE,
			NP_UserData,	& msg,
			TAG_END 
		);

		#else

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : Net Send Process",
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
				#ifndef _AOS4_
				process->pr_Task.tc_UserData = & msg;
				#endif

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

	if ( cfg->cfg_NetSendStatus == PROCESS_Running )
	{
		error = FALSE;
	}

	return( error );
}

// --

void myStop_Net_Send( struct Config *cfg )
{
U32 mask;
S32 cnt;

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "myStop_NetSend\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	if ( cfg->cfg_NetSendStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_NetSendStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_NetSendStatus == PROCESS_Stopping ))
		{
			Delay( 2 );
		}

		if ( cfg->cfg_NetSendStatus == PROCESS_Running )
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = mem_ASPrintF( "Normal shutdown" );
			}

			cfg->NetSend_Exit = FindTask( NULL );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "NetSend: Sending CTRL+D+E\n" );
			}

			// Clear CTRL+F
			SetSignal( 0L, SIGBREAKF_CTRL_F );

			// Send Break Signal(s)
			Signal( cfg->NetSend_Task, NET_EXIT_SIGNAL );

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
						Signal( cfg->NetSend_Task, NET_EXIT_SIGNAL );
						SHELLBUF_PRINTF1( "myStop_Net_Send still waiting : %" PRId32 "\n", cnt );
					}
				}
			}

			if ( cnt >= 50 )
			{
				SHELLBUF_PRINTF( "\n" );
			}
		}
	}
}

// --
