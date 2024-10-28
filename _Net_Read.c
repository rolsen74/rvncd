
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

static int myOpen_Input( struct Config *cfg )
{
int error;

	error = TRUE;

	cfg->NetRead_InputMsgPort = IExec->AllocSysObjectTags( ASOT_PORT,
		TAG_END
	);

	if ( cfg->NetRead_InputMsgPort == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Input MsgPort" );
		goto bailout;
	}

	cfg->NetRead_InputIOReq = IExec->AllocSysObjectTags( ASOT_IOREQUEST,
		ASOIOR_ReplyPort, cfg->NetRead_InputMsgPort,
		ASOIOR_Size, sizeof( struct IOStdReq ),
		TAG_END
	);

	if ( cfg->NetRead_InputIOReq == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Input IORequest" );
		goto bailout;
	}

	if ( IExec->OpenDevice( "input.device", 0, (APTR) cfg->NetRead_InputIOReq, 0 ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening Input device" );
		goto bailout;
	}

	cfg->NetRead_InputBase = (APTR) cfg->NetRead_InputIOReq->io_Device;

	cfg->NetRead_IInput = (APTR) IExec->GetInterface( cfg->NetRead_InputBase, "main", 1, NULL );

	if ( cfg->NetRead_IInput == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining Input interface" );
		goto bailout;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static void myClose_Input( struct Config *cfg )
{
	if ( cfg->NetRead_IInput )
	{
		IExec->DropInterface( (APTR) cfg->NetRead_IInput );
		cfg->NetRead_IInput = NULL;
	}

	if ( cfg->NetRead_InputBase )
	{
		IExec->CloseDevice(	(APTR) cfg->NetRead_InputIOReq );
		cfg->NetRead_InputBase = NULL;
	}

	if ( cfg->NetRead_InputIOReq )
	{
		IExec->FreeSysObject( ASOT_IOREQUEST, cfg->NetRead_InputIOReq );
		cfg->NetRead_InputIOReq = NULL;
	}

	if ( cfg->NetRead_InputMsgPort )
	{
		IExec->FreeSysObject( ASOT_PORT, cfg->NetRead_InputMsgPort );
		cfg->NetRead_InputMsgPort = NULL;
	}
}

// --

static int myOpen_Socket( struct Config *cfg )
{
struct SocketIFace *ISocket;
struct timeval timeout;
int32 d;
int enable;
int error;
int s;

	error = TRUE;

	ISocket = cfg->NetRead_ISocket;

	// --

	d = cfg->Server_DuplicateReadSocket;
	s = ISocket->ObtainSocket( d, AF_INET, SOCK_STREAM, 0 );

	if ( s == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to obtain Socket (Sockets: %ld %ld)", s, d );
		goto bailout;
	}

	cfg->Server_DuplicateReadSocket = -1;
	cfg->NetRead_ClientSocket = s;

	// -- Enable Keep Alive

	enable = 1;

	ISocket->setsockopt( cfg->NetRead_ClientSocket, SOL_SOCKET, SO_KEEPALIVE, & enable, sizeof( int ));

	// -- Set 10s Timeout

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	ISocket->setsockopt( cfg->NetRead_ClientSocket, SOL_SOCKET, SO_RCVTIMEO, & timeout, sizeof( timeout ));

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

static void myClose_Socket( struct Config *cfg )
{
	if ( cfg->NetRead_ClientSocket != -1 )
	{
		cfg->NetRead_ISocket->CloseSocket( cfg->NetRead_ClientSocket );
		cfg->NetRead_ClientSocket = -1;
	}
}

// --

static int myOpen_Net( struct Config *cfg )
{
struct SocketIFace *ISocket;
int error;

	error = TRUE;

	cfg->NetRead_SocketBase = IExec->OpenLibrary( "bsdsocket.library", 4 );

	if ( cfg->NetRead_SocketBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening bsdsocket library v4" );
		goto bailout;
	}

	ISocket = (APTR) IExec->GetInterface( cfg->NetRead_SocketBase, "main", 1, NULL );

	if ( ISocket == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining bsdsocket interface" );
		goto bailout;
	}

	cfg->NetRead_ISocket = ISocket;

	ISocket->SocketBaseTags(
		SBTM_SETVAL(SBTC_BREAKMASK), SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E,
		TAG_END
	);

	error = FALSE;

bailout:

	return( error );
}

// --

static void myClose_Net( struct Config *cfg )
{
	if ( cfg->NetRead_ISocket )
	{
		IExec->DropInterface( (APTR) cfg->NetRead_ISocket );
		cfg->NetRead_ISocket = NULL;
	}

	if ( cfg->NetRead_SocketBase )
	{
		IExec->CloseLibrary( cfg->NetRead_SocketBase );
		cfg->NetRead_SocketBase = NULL;
	}
}

// --

static void myProcess_Main( struct Config *cfg )
{
struct SocketIFace *ISocket;
//struct timeval timeout;
fd_set readfd;
uint32 signal;
uint32 mask;
int gotsignal;
int stat;
int err;

	gotsignal = FALSE;

	ISocket = cfg->NetRead_ISocket;

	// --

	switch( cfg->NetSend_ClientProtocol )
	{
		case VNCProtocol_33:
		{
			printf( "VNC_HandleCmds_33\n" );
			break;
		}

		case VNCProtocol_37:
		{
			printf( "VNC_HandleCmds_37\n" );
			break;
		}

		default:
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Invalid Client Protocol type (%ld)", cfg->NetSend_ClientProtocol );
			break;
		}
	}

	// --

	while( TRUE )
	{
		if ( cfg->NetRead_Idle )
		{
			mask = IDOS->CheckSignal( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E );

			if ((( mask & ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) == ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) && ( cfg->NetRead_Exit ))
			{
				gotsignal = TRUE;
				break;
			}

			IDOS->Delay( 2 );
		}
		else
		{
			FD_ZERO( & readfd );
			FD_SET( cfg->NetRead_ClientSocket, & readfd );

			signal = SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E;

//			timeout.tv_sec	= 1;
//			timeout.tv_usec	= 0;

//			stat = ISocket->WaitSelect( 1, & readfd, NULL, NULL, & timeout, & signal );
			stat = ISocket->WaitSelect( 1, & readfd, NULL, NULL, NULL, & signal );

			/**/ if ( stat == -1 )
			{
				// Error

				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = myASPrintF( "WaitSelect failed (%d)", ISocket->Errno() );
				}

				break;
			}
			else if ( stat == 0 )
			{
				// Timeout or Signals

				if ( signal )
				{
					if ((( signal & ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) == ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) && ( cfg->NetRead_Exit ))
					{
						gotsignal = TRUE;
						break;
					}
				}
			}
			else
			{
				if ( FD_ISSET( cfg->NetRead_ClientSocket, & readfd ))
				{
					switch( cfg->NetSend_ClientProtocol )
					{
						case VNCProtocol_33:
						{
							err = VNC_HandleCmds_33( cfg );
							break;
						}

						case VNCProtocol_37:
						{
							err = VNC_HandleCmds_37( cfg );
							break;
						}

						default:
						{
							Log_PrintF( cfg, LOGTYPE_Error, "Invalid Client Protocol type (%ld)", cfg->NetSend_ClientProtocol );
							err = TRUE;
							break;
						}
					}

					if ( err )
					{
						break;
					}
				}
			}
		}
	}

	// --

	// Shutdown NetSend process
	cfg->cfg_ClientRunning = FALSE;

	// --

//	if ( cfg->NetRead_Exit == NULL )
	if ( ! gotsignal )
	{
		while( TRUE )
		{
			mask = IDOS->CheckSignal( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E );

			if ((( mask & ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) == ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) && ( cfg->NetRead_Exit ))
			{
				break;
			}

			IDOS->Delay( 2 );
		}
	}
}

// --

static int myProcess_Init( struct Config *cfg )
{
int retval;

	retval = FALSE;

	// --

	cfg->NetRead_ClientSocket = -1;
	cfg->NetRead_Idle = TRUE;

	// --

	cfg->NetRead_ReadBufferSize = 1024 * 4;
	cfg->NetRead_ReadBuffer = IExec->AllocVecTags( cfg->NetRead_ReadBufferSize,
		AVT_PhysicalAlignment, 4096,
		AVT_Contiguous, TRUE,
		AVT_Alignment, 4096,
		AVT_Lock, TRUE,
		TAG_END
	);

	if ( cfg->NetRead_ReadBuffer == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory (%ld Bytes)", cfg->NetRead_ReadBufferSize );
		goto bailout;
	}

	// --

	if ( myOpen_Net( cfg ))
	{
		goto bailout;
	}

	if ( myOpen_Input( cfg ))
	{
		goto bailout;
	}

	if ( myOpen_Socket( cfg ))
	{
		goto bailout;
	}

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --

static void myProcess_Free( struct Config *cfg )
{
	// --

	myClose_Socket( cfg );

	myClose_Input( cfg );

	myClose_Net( cfg );

	// --

	if ( cfg->NetRead_ReadBuffer )
	{
		IExec->UnlockMem( cfg->NetRead_ReadBuffer, cfg->NetRead_ReadBufferSize );
		IExec->FreeVec( cfg->NetRead_ReadBuffer );
		cfg->NetRead_ReadBuffer = NULL;
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

	Config->NetRead_Exit = NULL;
	Config->cfg_NetReadStatus = PROCESS_Starting;

	stat = myProcess_Init( Config );

	if ( stat )
	{
		Config->NetRead_Task = Self;

		Config->cfg_NetReadStatus = PROCESS_Running;

		// Set signal after Status
		IExec->Signal( Parent, SIGBREAKF_CTRL_E );

		// --

		myProcess_Main( Config );

		// --

		Config->cfg_NetReadStatus = PROCESS_Stopping;

		Parent = Config->NetRead_Exit;

		Config->NetRead_Task = NULL;
	}

	myProcess_Free( Config );

	Config->cfg_NetReadStatus = PROCESS_Stopped;

	//--------

	if ( Parent )
	{
		IExec->Forbid();
		IExec->Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

int myStart_Net_Read( struct Config *cfg )
{
struct StartMessage msg;
uint32 mask;
int error;

	error = TRUE;

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_NetReadStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_NetReadStatus == PROCESS_Stopping ))
	{
		IDOS->Delay( 2 );
	}

	if ( cfg->cfg_NetReadStatus == PROCESS_Stopped )
	{
		msg.Parent = IExec->FindTask( NULL );
		msg.Config = cfg;

		if ( IDOS->CreateNewProcTags(
			NP_Name,		"Net Read Process",
			NP_Priority,	5,
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

	if ( cfg->cfg_NetReadStatus == PROCESS_Running )
	{
		error = FALSE;
	}

	return( error );
}

// --

void myStop_Net_Read( struct Config *cfg )
{
uint32 mask;
int cnt;

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	if ( cfg->cfg_NetReadStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_NetReadStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_NetReadStatus == PROCESS_Stopping ))
		{
			IDOS->Delay( 2 );
		}

		if ( cfg->cfg_NetReadStatus == PROCESS_Running )
		{
			cfg->NetRead_Exit = IExec->FindTask( NULL );

			IExec->Signal( cfg->NetRead_Task, SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E );

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
						printf( "\rmyStop_Net_Read still waiting : %d", cnt );
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
