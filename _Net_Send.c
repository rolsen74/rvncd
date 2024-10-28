
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

static void myProcess_Main( struct Config *cfg )
{
struct UpdateNode *un;
uint32 mask;
//int delay;
int stat;

	// -- Exchange VNC Info

	if ( DoVerbose )
	{
		printf( "\nVNC Init [1/6]\n\n" );
	}

	if ( VNC_Version( cfg ))
	{
		goto bailout;
	}

	if ( DoVerbose )
	{
		printf( "\nVNC Init [2/6]\n\n" );
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

	if ( DoVerbose )
	{
		printf( "\nVNC Init [3/6]\n\n" );
	}

	if ( VNC_ClientInit( cfg ))
	{
		goto bailout;
	}

	if ( DoVerbose )
	{
		printf( "VNC Init [4/6]\n\n" );
	}

	if ( VNC_ServerInit( cfg ))
	{
		goto bailout;
	}

	if ( DoVerbose )
	{
		printf( "VNC Init [5/6]\n\n" );
	}

	// -- NetRead can now Read Requests

	cfg->NetRead_Idle = FALSE;

	// --

	// Clear ClipBoard signal
	IExec->SetSignal( 0, cfg->NetSend_ClipBoardSignal );

	if ( DoVerbose )
	{
		Log_PrintF( cfg, LOGTYPE_Info, "Clinet Connected" );
		printf( "VNC Init [6/6] -- Rock'n Roll\n\n" );
	}

	// --

	cfg->cfg_KickUser = 0;

	while(( cfg->cfg_ClientRunning ) && ( ! cfg->cfg_KickUser ))
	{
		if ( cfg->cfg_Active_Settings.SendWatchDog )
		{
			WatchDog_StartTimer( cfg );
		}

		stat = NewBufferUpdate( cfg );

		if ( cfg->cfg_Active_Settings.SendWatchDog )
		{
			WatchDog_StopTimer(	cfg );
		}

		/**/ if ( stat == UPDATESTAT_Error ) // Error
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Internal: NewBufferUpdate Failed" );
			}
			break;
		}
		else if ( stat == UPDATESTAT_NoChange ) // No Change
		{
			IDOS->Delay( 2 );
		}

		// --

		mask = IDOS->CheckSignal( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E | cfg->NetSend_ClipBoardSignal );

		if ((( mask & ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) == ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) && ( cfg->NetSend_Exit ))
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

	IExec->ObtainSemaphore( & cfg->Server_UpdateSema );

	while( TRUE )
	{
		un = (APTR) IExec->RemHead( & cfg->Server_UpdateList );

		if ( un == NULL )
		{
			break;
		}

		IExec->AddTail( & cfg->Server_UpdateFree, & un->un_Node );
	}

	IExec->ReleaseSemaphore( & cfg->Server_UpdateSema );

	// --

	myPrintSessionInfo( cfg );

	cfg->cfg_ClientRunning = FALSE;
}

// --

static int myProcess_Init( struct Config *cfg )
{
APTR buf;
int retval;
int size;
int cnt;

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

	buf = IExec->AllocVecTags( size,
		AVT_PhysicalAlignment, 4096,
		AVT_Contiguous, TRUE,
		AVT_Alignment, 4096,
		AVT_Lock, TRUE,
		TAG_END
	);

	if ( buf == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory (%ld Bytes)", size );
		goto bailout;
	}

	cfg->NetSend_SendBuffer = buf;
	cfg->NetSend_SendBufferSize = size;

	// --

	if ( cfg->UserCount == 0 )
	{
		cfg->GfxRead_Enocde_ActivePixelSet = FALSE;
		memset( & cfg->NetSend_Encodings1, 0, ENCODE_LAST * sizeof( uint8 ) );
		memset( & cfg->NetSend_Encodings2, 0, ENCODE_LAST * sizeof( struct myEncNode ) );
		memset( & cfg->GfxRead_Enocde_ActivePixel, 100, sizeof( struct PixelMessage ));

		// Always have Raw Enabled
		cfg->NetSend_Encodings1[ENCODE_Raw] = TRUE;
		cfg->NetSend_Encodings2[ENCODE_Raw].Pos = 0;
		cfg->NetSend_Encodings2[ENCODE_Raw].Enabled = TRUE;
		cfg->NetSend_Encodings2[ENCODE_Raw].EncType = TRUE;

		if ( cfg->cfg_Active_Settings.DisableBlanker )
		{
			Send_Blanker_Disable( cfg );
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

	IExec->Forbid();
	cfg->cfg_UserConnectSignal = TRUE;
	cfg->UserCount++;
	IExec->Permit();

	for( cnt=0 ; cnt<50 ; cnt++ )
	{
		if ( cfg->cfg_UserConnectSignal )
		{
			IDOS->Delay( 2 );
		}
		else
		{
			break;
		}
	}

	// --

	if ( Send_Open_Socket( cfg ))
	{
		goto bailout;
	}

	// --

	if ( myStart_Net_Read( cfg ))
	{
		goto bailout;
	}

	if ( cfg->cfg_Active_Settings.SendWatchDog )
	{
		if ( myStart_WatchDog( cfg ))
		{
			goto bailout;
		}
	}

	// -- Setup Z Library
	
	myZLib_Setup( cfg );

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --

static void myProcess_Free( struct Config *cfg )
{
int cnt;

	// --

	Log_PrintF( cfg, LOGTYPE_Info, "Shutting down .. " );

	// --

	myZLib_Cleanup( cfg );

	myStop_WatchDog( cfg );

	myStop_Net_Read( cfg );

	Send_Close_Socket( cfg );

	// --

	if ( cfg->NetSend_SendBuffer )
	{
		IExec->UnlockMem( cfg->NetSend_SendBuffer, cfg->NetSend_SendBufferSize );
		IExec->FreeVec( cfg->NetSend_SendBuffer );
		cfg->NetSend_SendBuffer = NULL;
	}

	// --

	if ( cfg->UserCount == 1 )
	{
		if ( cfg->cfg_Active_Settings.DisableBlanker )
		{
			Send_Blanker_Enable( cfg );
		}

		if ( cfg->cfg_Active_Settings.SendClipboard )
		{
			Send_Remove_ClipHook( cfg );
		}

		// --

		IExec->Forbid();
		cfg->cfg_UserDisconnectSignal = TRUE;
		cfg->UserCount--;
		IExec->Permit();

		for( cnt=0 ; cnt<50 ; cnt++ )
		{
			if ( cfg->cfg_UserDisconnectSignal )
			{
				IDOS->Delay( 2 );
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		IExec->Forbid();
		cfg->UserCount--;
		IExec->Permit();
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
int stat;

	//--------

	Self = IExec->FindTask( NULL );

	sm = Self->tc_UserData;

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
		IExec->Signal( Parent, SIGBREAKF_CTRL_E );

		// --

		msg = myCalloc( sizeof( struct CommandSession ) );

		if ( msg )
		{
			msg->cs_Command = CMD_Session;
			msg->cs_Starting = 1;
		
			IExec->PutMsg( CmdMsgPort, & msg->cs_Message );
		}

		// --

		if ( Config->cfg_ActionsUserConnectEnable )
		{
			DoAction_UserConnect( Config );
		}

		if ( Config->cfg_NetReason )
		{
			myFree( Config->cfg_NetReason );
			Config->cfg_NetReason = NULL;
		}

		myProcess_Main( Config );

		if ( Config->cfg_NetReason )
		{
			Log_PrintF( Config, LOGTYPE_Info, "Client Shutdown .. %s", Config->cfg_NetReason );
			myFree( Config->cfg_NetReason );
			Config->cfg_NetReason = NULL;
		}

		// --

		msg = myCalloc( sizeof( struct CommandSession ) );

		if ( msg )
		{
			msg->cs_Command = CMD_Session;
			msg->cs_Starting = 0;
		
			IExec->PutMsg( CmdMsgPort, & msg->cs_Message );
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

	IExec->Forbid();

	if ( Parent )
	{
		IExec->Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

int myStart_Net_Send( struct Config *cfg )
{
struct StartMessage msg;
uint32 mask;
int error;

	error = TRUE;

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_NetSendStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_NetSendStatus == PROCESS_Stopping ))
	{
		IDOS->Delay( 2 );
	}

	if ( cfg->cfg_NetSendStatus == PROCESS_Stopped )
	{
		msg.Parent = IExec->FindTask( NULL );
		msg.Config = cfg;

		if ( IDOS->CreateNewProcTags(
			NP_Name,	    "Net Send Process",
			NP_Priority,	1,
			NP_Entry,	    myServerProcess,
			NP_Child,	    TRUE,
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

	if ( cfg->cfg_NetSendStatus == PROCESS_Running )
	{
		error = FALSE;
	}

	return( error );
}

// --

void myStop_Net_Send( struct Config *cfg )
{
uint32 mask;
int cnt;

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	if ( cfg->cfg_NetSendStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_NetSendStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_NetSendStatus == PROCESS_Stopping ))
		{
			IDOS->Delay( 2 );
		}

		if ( cfg->cfg_NetSendStatus == PROCESS_Running )
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Normal shutdown" );
			}

			cfg->NetSend_Exit = IExec->FindTask( NULL );

			IExec->Signal( cfg->NetSend_Task, SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E );

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
						printf( "\rmyStop_Net_Send still waiting : %d", cnt );
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
