
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

static S32 myOpen_Input( struct Config *cfg )
{
S32 error;

	error = TRUE;

	if ( ! MsgPort_Init( & cfg->NetRead_InputMsgPort ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Input MsgPort" );
		goto bailout;
	}

//	cfg->NetRead_InputMsgPort = AllocSysObjectTags( ASOT_PORT,
//		TAG_END
//	);
//
//	if ( cfg->NetRead_InputMsgPort == NULL )
//	{
//		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Input MsgPort" );
//		goto bailout;
//	}

	#ifdef _AOS4_

	cfg->NetRead_InputIOReq = AllocSysObjectTags( ASOT_IOREQUEST,
		ASOIOR_ReplyPort, cfg->NetRead_InputMsgPort,
		ASOIOR_Size, sizeof( struct IOStdReq ),
		TAG_END
	);

	#else

	cfg->NetRead_InputIOReq = (PTR) CreateIORequest( & cfg->NetRead_InputMsgPort.vmp_MsgPort, sizeof( struct IOStdReq ));

	#endif

	if ( ! cfg->NetRead_InputIOReq )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Input IORequest" );
		goto bailout;
	}

	if ( OpenDevice( "input.device", 0, (PTR) cfg->NetRead_InputIOReq, 0 ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening Input device" );
		goto bailout;
	}

	cfg->NetRead_InputBase = (PTR) cfg->NetRead_InputIOReq->io_Device;

	#ifdef _AOS4_
	cfg->NetRead_IInput = (PTR) GetInterface( cfg->NetRead_InputBase, "main", 1, NULL );

	if ( ! cfg->NetRead_IInput )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining Input interface" );
		goto bailout;
	}
	#endif

	error = FALSE;

bailout:

	return( error );
}

// --

static void myClose_Input( struct Config *cfg )
{
	#ifdef _AOS4_
	if ( cfg->NetRead_IInput )
	{
		DropInterface( (PTR) cfg->NetRead_IInput );
		cfg->NetRead_IInput = NULL;
	}
	#endif

	if ( cfg->NetRead_InputBase )
	{
		CloseDevice(	(PTR) cfg->NetRead_InputIOReq );
		cfg->NetRead_InputBase = NULL;
	}

	if ( cfg->NetRead_InputIOReq )
	{
		#ifdef _AOS4_

		FreeSysObject( ASOT_IOREQUEST, cfg->NetRead_InputIOReq );

		#else

		DeleteIORequest( cfg->NetRead_InputIOReq );

		#endif

		cfg->NetRead_InputIOReq = NULL;
	}

	MsgPort_Free( & cfg->NetRead_InputMsgPort );

//	if ( cfg->NetRead_InputMsgPort )
//	{
//		FreeSysObject( ASOT_PORT, cfg->NetRead_InputMsgPort );
//		cfg->NetRead_InputMsgPort = NULL;
//	}
}

// --

static S32 myOpen_Socket( struct Config *cfg )
{
//struct SocketIFace *ISocket;
struct timeval timeout;
S32 d;
S32 enable;
S32 error;
S32 s;

	error = TRUE;

	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->NetRead_ISocket;
	#else
	struct Library *SocketBase = cfg->NetRead_SocketBase;
	#endif

//	ISocket = cfg->NetRead_ISocket;

	// --

	d = cfg->Server_DuplicateReadSocket;
	s = ObtainSocket( d, AF_INET, SOCK_STREAM, 0 );

	if ( s == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to obtain Socket (Sockets: %ld %ld)", s, d );
		goto bailout;
	}

	cfg->Server_DuplicateReadSocket = -1;
	cfg->NetRead_ClientSocket = s;

	// -- Enable Keep Alive

	enable = 1;

	setsockopt( cfg->NetRead_ClientSocket, SOL_SOCKET, SO_KEEPALIVE, & enable, sizeof( S32 ));

	// -- Set 10s Timeout

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	setsockopt( cfg->NetRead_ClientSocket, SOL_SOCKET, SO_RCVTIMEO, & timeout, sizeof( timeout ));

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

static void myClose_Socket( struct Config *cfg )
{
	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->NetRead_ISocket;
	#else
	struct Library *SocketBase = cfg->NetRead_SocketBase;
	#endif

	if ( cfg->NetRead_ClientSocket != -1 )
	{
		CloseSocket( cfg->NetRead_ClientSocket );
		cfg->NetRead_ClientSocket = -1;
	}
}

// --

static S32 myOpen_Net( struct Config *cfg )
{
//struct SocketIFace *ISocket;
S32 error;
S32 stat;

	error = TRUE;

	cfg->NetRead_SocketBase = OpenLibrary( "bsdsocket.library", 4 );

	if ( ! cfg->NetRead_SocketBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening bsdsocket library v4" );
		goto bailout;
	}

	#ifdef _AOS4_

	cfg->NetRead_ISocket = (PTR) GetInterface( cfg->NetRead_SocketBase, "main", 1, NULL );

	if ( ! cfg->NetRead_ISocket )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining bsdsocket interface" );
		goto bailout;
	}

	#endif

	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->NetRead_ISocket;
	#else
	struct Library *SocketBase = cfg->NetRead_SocketBase;
	#endif

	stat = SocketBaseTags(
		SBTM_SETVAL(SBTC_BREAKMASK), NET_EXIT_SIGNAL,
		TAG_END
	);

	if ( DoVerbose > 1 )
	{
		SHELLBUF_PRINTF( "NetRead: Set BreakMask returned %d\n", stat );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static void myClose_Net( struct Config *cfg )
{
	#ifdef _AOS4_
	if ( cfg->NetRead_ISocket )
	{
		DropInterface( (PTR) cfg->NetRead_ISocket );
		cfg->NetRead_ISocket = NULL;
	}
	#endif

	if ( cfg->NetRead_SocketBase )
	{
		CloseLibrary( cfg->NetRead_SocketBase );
		cfg->NetRead_SocketBase = NULL;
	}
}

// --

static void myProcess_Main( struct Config *cfg )
{
//struct SocketIFace *ISocket;
//struct timeval timeout;
fd_set readfd;
ULONG signal;
U32 mask;
S32 gotsignal;
S32 stat;
S32 err;

	gotsignal = FALSE;

//	ISocket = cfg->NetRead_ISocket;

	#ifdef DEBUG
	DebugPrintF( "net read start\n" );
	#endif

	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->NetRead_ISocket;
	#else
	struct Library *SocketBase = cfg->NetRead_SocketBase;
	#endif

	// --

	#if 0
	switch( cfg->NetSend_ClientProtocol )
	{
		case VNCProtocol_33:
		{
			SHELLBUF_PRINTF( "VNC_HandleCmds_33\n" );
			break;
		}

		case VNCProtocol_37:
		{
			SHELLBUF_PRINTF( "VNC_HandleCmds_37\n" );
			break;
		}

		default:
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Invalid Client Protocol type (%d)", cfg->NetSend_ClientProtocol );
			break;
		}
	}
	#endif

	// --

	while( TRUE )
	{
		if ( cfg->NetRead_Idle )
		{
			mask = CheckSignal( NET_EXIT_SIGNAL );

			if ((( mask & ( NET_EXIT_SIGNAL )) == ( NET_EXIT_SIGNAL )) && ( cfg->NetRead_Exit ))
			{
				gotsignal = TRUE;
				break;
			}

			Delay( 2 );
		}
		else
		{
			FD_ZERO( & readfd );
			FD_SET( cfg->NetRead_ClientSocket, & readfd );

			signal = NET_EXIT_SIGNAL;

//			timeout.tv_sec	= 1;
//			timeout.tv_usec	= 0;

//			stat = WaitSelect( 1, & readfd, NULL, NULL, & timeout, & signal );
			stat = WaitSelect( 1, & readfd, NULL, NULL, NULL, & signal );

			/**/ if ( stat == -1 )
			{
				// Error

				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = mem_ASPrintF( "WaitSelect failed (%d)", Errno() );
				}

				break;
			}
			else if ( stat == 0 )
			{
				// Timeout or Signals

				if ( signal )
				{
					if ((( signal & ( NET_EXIT_SIGNAL )) == ( NET_EXIT_SIGNAL )) && ( cfg->NetRead_Exit ))
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

	#ifdef DEBUG
	DebugPrintF( "net read 1 stopping\n" );
	#endif

	// --

	// Shutdown NetSend process
	cfg->cfg_ClientRunning = FALSE;

	// --

//	if ( cfg->NetRead_Exit == NULL )
	if ( ! gotsignal )
	{
		while( TRUE )
		{
			mask = CheckSignal( NET_EXIT_SIGNAL );

			if ((( mask & ( NET_EXIT_SIGNAL )) == ( NET_EXIT_SIGNAL )) && ( cfg->NetRead_Exit ))
			{
				break;
			}

			Delay( 2 );
		}
	}

	#ifdef DEBUG
	DebugPrintF( "net read 2 stopping\n" );
	#endif
}

// --

static S32 myProcess_Init( struct Config *cfg )
{
S32 retval;

	retval = FALSE;

	// --

	cfg->NetRead_ClientSocket = -1;
	cfg->NetRead_Idle = TRUE;

	// --

	cfg->NetRead_ReadBufferSize = 1024 * 4;

	#ifdef _AOS4_

	cfg->NetRead_ReadBuffer = AllocVecTags( cfg->NetRead_ReadBufferSize,
		AVT_PhysicalAlignment, 4096,
		AVT_Contiguous, TRUE,
		AVT_Alignment, 4096,
		AVT_Lock, TRUE,
		TAG_END
	);

	#else

	cfg->NetRead_ReadBuffer = AllocVec( cfg->NetRead_ReadBufferSize, MEMF_ANY );

	#endif

	if ( ! cfg->NetRead_ReadBuffer )
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
		#ifdef _AOS4_
		UnlockMem( cfg->NetRead_ReadBuffer, cfg->NetRead_ReadBufferSize );
		#endif
		FreeVec( cfg->NetRead_ReadBuffer );
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

	Config->NetRead_Exit = NULL;
	Config->cfg_NetReadStatus = PROCESS_Starting;

	stat = myProcess_Init( Config );

	if ( stat )
	{
		Config->NetRead_Task = Self;

		Config->cfg_NetReadStatus = PROCESS_Running;

		// Set signal after Status
		Signal( Parent, SIGBREAKF_CTRL_E );

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

	#ifdef DEBUG
	DebugPrintF( "net read stopped\n" );
	#endif

	if ( Parent )
	{
		Forbid();
		Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

S32 myStart_Net_Read( struct Config *cfg )
{
struct StartMessage msg;
struct Process *process;
S32 error;
U32 mask;
U32 wait;

	error = TRUE;

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "myStart_Net_Read\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_NetReadStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_NetReadStatus == PROCESS_Stopping ))
	{
		Delay( 2 );
	}

	if ( cfg->cfg_NetReadStatus == PROCESS_Stopped )
	{
		msg.Parent = FindTask( NULL );
		msg.Config = cfg;

		#ifdef _AOS4_

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : Net Read Process",
			NP_Priority,	5,
			NP_Entry,		myServerProcess,
			NP_Child,		TRUE,
			NP_UserData,	& msg,
			TAG_END 
		);

		#else

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : Net Read Process",
			NP_Priority,	5,
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

	if ( cfg->cfg_NetReadStatus == PROCESS_Running )
	{
		error = FALSE;
	}

	return( error );
}

// --

void myStop_Net_Read( struct Config *cfg )
{
U32 mask;
S32 cnt;

	if ( DoVerbose > 2 )
	{
		printf( "myStop_Net_Read\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	if ( cfg->cfg_NetReadStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_NetReadStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_NetReadStatus == PROCESS_Stopping ))
		{
			Delay( 2 );
		}

		if ( cfg->cfg_NetReadStatus == PROCESS_Running )
		{
			cfg->NetRead_Exit = FindTask( NULL );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "NetRead: Sending CTRL+D+E\n" );
			}

			// Clear CTRL+F
			SetSignal( 0L, SIGBREAKF_CTRL_F );
			// Send Break Signal(s)
			Signal( cfg->NetRead_Task, NET_EXIT_SIGNAL );

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
						Signal( cfg->NetRead_Task, NET_EXIT_SIGNAL );
						printf( "myStop_Net_Read still waiting : %d\n", cnt );
//						SHELLBUF_PRINTF( "\rmyStop_Net_Read still waiting : %d", cnt );
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
