
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

static S32 myCheck_IP( struct Config *cfg UNUSED, struct IPNode *node, S16 a, S16 b, S16 c, S16 d )
{
S32 retval;

	retval = FALSE;

	// -1 is wild star .. '*'

	if (( node->ipn_A != -1 ) && ( node->ipn_A != a ))
	{
		goto bailout;
	}

	if (( node->ipn_B != -1 ) && ( node->ipn_B != b ))
	{
		goto bailout;
	}

	if (( node->ipn_C != -1 ) && ( node->ipn_C != c ))
	{
		goto bailout;
	}

	if (( node->ipn_D != -1 ) && ( node->ipn_D != d ))
	{
		goto bailout;
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --

static S32 myValidateIP( struct Config *cfg, STR addrbuf, S32 a, S32 b, S32 c, S32 d )
{
struct IPNode *node;
S32 rejected;
S32 users;

	rejected = TRUE;

	if ((( a < 0 ) || ( a > 255 ))
	||	(( b < 0 ) || ( b > 255 ))
	||	(( c < 0 ) || ( c > 255 ))
	||	(( d < 0 ) || ( d > 255 )))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Invalid IP format (%ld.%ld.%ld.%ld)", a, b, c, d );
		goto bailout;
	}

	// -- Check IP Allow/Block list

	ObtainSemaphore( & cfg->IPSema );
	node = List_GetHead( & cfg->IPList );

	while( node )
	{
		if ( node->ipn_Type == IPT_Allow )
		{
			if ( myCheck_IP( cfg, node, a, b, c, d ))
			{
				// IP is allowed

				// it still can be rejected for Max users
				cfg->Connect_Allowed_IPs++;
				break;
			}
		}
		else // IPT_Block
		{
			if ( myCheck_IP( cfg, node, a, b, c, d ))
			{
				// IP is blocked

				if ( cfg->cfg_LogUserConnect )
				{
					Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Connection from %s rejected. (Blocked)", addrbuf );
				}

				cfg->Connect_Rejected++;
				cfg->Connect_Blocked_IPs++;

				ReleaseSemaphore( & cfg->IPSema );
				goto bailout;
			}
		}

		node = List_GetNext(  node );
	}
	ReleaseSemaphore( & cfg->IPSema );

	// -- Passed .. Normal connections

	Forbid();
	users = cfg->UserCount;
	Permit();

	if ( users < MAX_USERS )
	{
		if ( cfg->cfg_LogUserConnect )
		{
			if ( node )
			{
				Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Connection from %s accepted. (Allowed)", addrbuf );
			}
			else
			{
				Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Connection from %s accepted.", addrbuf );
			}
		}

		cfg->Connect_Accecpted++;
	}
	else
	{
		Log_PrintF( cfg, LOGTYPE_Info, "Closing connection, maximum users reached." );

		cfg->Connect_Rejected++;
		goto bailout;
	}

	rejected = FALSE;

bailout:

	{
		struct CommandMessage *cmsg;

		cmsg = mem_AllocClr( sizeof( struct CommandMessage ) );

		if ( cmsg )
		{
			cmsg->cm_Command = CMD_ConnectStats;

			MsgPort_PutMsg( & CmdMsgPort, & cmsg->cm_Message );
		}
	}

	return( rejected );
}

// --

static S32 myOpen_Net( struct Config *cfg )
{
struct sockaddr_in serverAddress;
struct timeval timeout;
S32 enable;
S32 error;
S32 reuse;
S32 stat;

	error = TRUE;

	cfg->Server_SocketBase = OpenLibrary( "bsdsocket.library", 4 );

	if ( ! cfg->Server_SocketBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Error opening bsdsocket library v4" );
		goto bailout;
	}

	#ifdef _AOS4_

	cfg->Server_ISocket = (PTR) GetInterface( cfg->Server_SocketBase, "main", 1, NULL );

	if ( ! cfg->Server_ISocket )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Error obtaining bsdsocket interface" );
		goto bailout;
	}

	#endif

	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->Server_ISocket;
	#else
	struct Library *SocketBase = cfg->Server_SocketBase;
	#endif

	stat = SocketBaseTags(
		SBTM_SETVAL(SBTC_BREAKMASK), NET_EXIT_SIGNAL,
		TAG_END
	);

	if ( DoVerbose > 1 )
	{
		SHELLBUF_PRINTF( "Server: Set BreakMask returned %d\n", stat );
	}

	// --

	cfg->Server_ListenSocket = socket( AF_INET, SOCK_STREAM, 0 );

	if ( cfg->Server_ListenSocket == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Failed to create listen socket '%s' (%ld)", Func_StrError( Errno() ), Errno() );
		goto bailout;
	}

	// -- Enable Keep Alive

	enable = 1;

	setsockopt( cfg->Server_ListenSocket, SOL_SOCKET, SO_KEEPALIVE, & enable, sizeof( S32 ));

	// -- Set 10s Timeout

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	setsockopt( cfg->Server_ListenSocket, SOL_SOCKET, SO_RCVTIMEO, & timeout, sizeof( timeout ));

	// -- Enable Reuse of Port

	reuse = 1;

	setsockopt( cfg->Server_ListenSocket, SOL_SOCKET, SO_REUSEADDR, & reuse, sizeof( S32 ));

	// --

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons( cfg->cfg_Active_Settings.Port );

	if ( bind( cfg->Server_ListenSocket, (struct sockaddr *) & serverAddress, sizeof( serverAddress )) == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Failed to bind listen socket '%s' (%ld)", Func_StrError( Errno() ), Errno() );
		goto bailout;
	}

	if ( listen( cfg->Server_ListenSocket, MAX_SERVERS ) == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Failed to listen on socket '%s' (%ld)", Func_StrError( Errno() ), Errno() );
		goto bailout;
	}

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

static void myClose_Net( struct Config *cfg )
{
S32 s;

	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->Server_ISocket;
	#else
	struct Library *SocketBase = cfg->Server_SocketBase;
	#endif

	if ( cfg->Server_DuplicateSendSocket != -1 )
	{
		s = ObtainSocket( cfg->Server_DuplicateSendSocket, AF_INET, SOCK_STREAM, 0 );
		if ( s != -1 ) CloseSocket( s );
		cfg->Server_DuplicateSendSocket = -1;
	}

	if ( cfg->Server_DuplicateReadSocket != -1 )
	{
		s = ObtainSocket( cfg->Server_DuplicateReadSocket, AF_INET, SOCK_STREAM, 0 );
		if ( s != -1 ) CloseSocket( s );
		cfg->Server_DuplicateReadSocket = -1;
	}

	if ( cfg->Server_ListenSocket != -1 )
	{
		CloseSocket( cfg->Server_ListenSocket );
		cfg->Server_ListenSocket = -1;
	}

	#ifdef _AOS4_
	if ( cfg->Server_ISocket )
	{
		DropInterface( (PTR) cfg->Server_ISocket );
		cfg->Server_ISocket = NULL;
	}
	#endif

	if ( cfg->Server_SocketBase )
	{
		CloseLibrary( cfg->Server_SocketBase );
		cfg->Server_SocketBase = NULL;
	}
}

// --

STR my_inet_ntop( struct Config *cfg, S32 af, PTR src, STR dst, socklen_t size ) 
{
	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->Server_ISocket;
	#else
	struct Library *SocketBase = cfg->Server_SocketBase;
	#endif

	if ( af == AF_INET )
	{
		struct in_addr *addr = (struct in_addr *) src;
		STR res = Inet_NtoA( addr->s_addr );

		if ( res ) 
		{
			strncpy( dst, res, size );
			dst[size - 1] = '\0';
			return dst;
		}
	}

	return NULL;
}

// --

static void myProcess_Main( struct Config *cfg )
{
struct sockaddr_in clientAddress;
socklen_t clientAddressLength;
char addrbuf[ INET_ADDRSTRLEN ];
S32 a, b, c, d, s;

	clientAddressLength = sizeof( clientAddress );

	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->Server_ISocket;
	#else
	struct Library *SocketBase = cfg->Server_SocketBase;
	#endif

	Log_PrintF( cfg, LOGTYPE_Info, "Listening on port %ld.", cfg->cfg_Active_Settings.Port );

	#ifdef DEBUG
	DebugPrintF( "Server started\n" );
	#endif

	while( ! cfg->cfg_ServerShutdown )
	{
		if ( cfg->Server_DuplicateSendSocket != -1 )
		{
			s = ObtainSocket( cfg->Server_DuplicateSendSocket, AF_INET, SOCK_STREAM, 0 );
			if ( s != -1 ) CloseSocket( s );
			cfg->Server_DuplicateSendSocket = -1;
		}

		if ( cfg->Server_DuplicateReadSocket != -1 )
		{
			s = ObtainSocket( cfg->Server_DuplicateReadSocket, AF_INET, SOCK_STREAM, 0 );
			if ( s != -1 ) CloseSocket( s );
			cfg->Server_DuplicateReadSocket = -1;
		}

		if ( cfg->Server_ClientSocket != -1 )
		{
			CloseSocket( cfg->Server_ClientSocket );
			cfg->Server_ClientSocket = -1;
		}

		// -- Wait for next connection

		cfg->Server_ClientSocket = accept( cfg->Server_ListenSocket, (PTR) & clientAddress, & clientAddressLength );

		if ( cfg->cfg_ServerShutdown )
		{
			break;
		}

		if (( cfg->Server_ClientSocket == -1 )
		||	( cfg->Server_ClientSocket ==  0 ))
		{
			if ( ! cfg->Server_Exit )
			{
				Log_PrintF( cfg, LOGTYPE_Error, "Server: accept() failed with '%s' (%ld)", Func_StrError( Errno() ), Errno() );
			}
			break;
		}

		// -- Get IP Infomation

		if ( ! my_inet_ntop( cfg, AF_INET, & clientAddress.sin_addr, addrbuf, INET_ADDRSTRLEN ))
//		if ( ! inet_ntop( AF_INET, & clientAddress.sin_addr, addrbuf, INET_ADDRSTRLEN ))
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Server: inet_ntop() failed with '%s'. (%ld)", Func_StrError( Errno() ), Errno() );
			continue;
		}

		if ( sscanf( addrbuf, "%d.%d.%d.%d", & a, & b, & c, & d ) != 4 )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Server: Invalid IP format" );
			continue;
		}

		if ( myValidateIP( cfg, addrbuf, a, b, c, d ))
		{
			continue;
		}

		// We do not really support Multi users (yet)
		// But this is a start .. maybe someone will finish it

		myInitSessionInfo( cfg, a, b, c, d );

		cfg->Server_DuplicateSendSocket = ReleaseCopyOfSocket( cfg->Server_ClientSocket, UNIQUE_ID );

		if ( cfg->Server_DuplicateSendSocket == -1 )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Server: Failed to clone Socket. (%ld)", Func_StrError( Errno() ), Errno() );
			continue;
		}

		cfg->Server_DuplicateReadSocket = ReleaseCopyOfSocket( cfg->Server_ClientSocket, UNIQUE_ID );

		if ( cfg->Server_DuplicateReadSocket == -1 )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Server: Failed to clone Socket. (%ld)", Func_StrError( Errno() ), Errno() );
			continue;
		}

		cfg->cfg_ServerCursorStat = -1;

		myStart_Net_Send( cfg );
	}

	#ifdef DEBUG
	DebugPrintF( "Server stopping\n" );
	#endif
}

// --

static S32 myProcess_Init( struct Config *cfg )
{
S32 retval;
S32 ts;

	retval = FALSE;

	cfg->Server_ListenSocket = -1;
	cfg->Server_ClientSocket = -1;
	cfg->Server_DuplicateSendSocket = -1;
	cfg->Server_DuplicateReadSocket = -1;

	LogTime_Init( & cfg->Server_LogTime );

	// --

	if ( cfg->cfg_LogServerStart )
	{
		Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Server starting up .." );
	}

	// --

	// Copy Settings
	memcpy( & cfg->cfg_Active_Settings, & cfg->cfg_Disk_Settings, sizeof( struct Settings ));

	// --

	ts = MAX(  16, cfg->cfg_Active_Settings.TileSize );
	ts = MIN( 256, ts );
	cfg->GfxRead_Screen_TileSize = ts;

	if (( ! cfg->cfg_Active_Settings.Protocol33 )
	&&	( ! cfg->cfg_Active_Settings.Protocol37 ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "No VNC Protocols have been enabled" );
		goto bailout;
	}

	// --

	if ( myOpen_Net( cfg ))
	{
		goto bailout;
	}

	if ( Send_Load_Pointer( cfg ))
	{
		goto bailout;
	}

	if ( myStart_Gfx_Read( cfg ))
	{
		goto bailout;
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --

static void myProcess_Free( struct Config *cfg )
{
struct UpdateNode *un;

	// --

	LogTime_Stop( & cfg->Server_LogTime );

	if ( cfg->cfg_LogServerStop )
	{
		Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Server shutting down .." );
	}

	// --

	myStop_Net_Send( cfg );

	myStop_Gfx_Read( cfg );

	Send_Free_Pointer( cfg );

	myClose_Net( cfg );

	// -- Childs have stopped.. 
	// Make sure we can start again
	cfg->cfg_ServerShutdown = FALSE;

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

		mem_Free( un );
	}

	while( TRUE )
	{
		un = List_RemHead( & cfg->Server_UpdateFree );

		if ( ! un )
		{
			break;
		}

		if ( un->un_Buffer )
		{
			mem_Free( un->un_Buffer );
			un->un_Buffer = NULL;
		}

		mem_Free( un );
	}

	ReleaseSemaphore( & cfg->Server_UpdateSema );
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
	DebugPrintF( "Server starting 1/2\n" );
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
		DebugPrintF( "Server starting delay\n" );
		#endif

		Delay( 2 );
	}

	#ifdef DEBUG
	DebugPrintF( "Server starting 2/2\n" );
	#endif

	Parent = sm->Parent;
	Config = sm->Config;

	//--------

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	Config->Server_Exit = NULL;
	Config->cfg_ServerStatus = PROCESS_Starting;

	if ( ActiveGUI.Server_Status_Change )
	{
		#ifdef DEBUG
		ActiveGUI.Server_Status_Change( Config, Config->cfg_ServerStatus, __FILE__, __LINE__ );
		#else
		ActiveGUI.Server_Status_Change( Config, Config->cfg_ServerStatus );
		#endif
	}

	stat = myProcess_Init( Config );

	if ( stat )
	{
		Config->Server_Task = Self;
	
		// 0 = off, 1 = starting, 2 = running, 3 = shutting down
		Config->cfg_ServerStatus = PROCESS_Running;

		if ( ActiveGUI.Server_Status_Change )
		{
			#ifdef DEBUG
			ActiveGUI.Server_Status_Change( Config, Config->cfg_ServerStatus, __FILE__, __LINE__ );
			#else
			ActiveGUI.Server_Status_Change( Config, Config->cfg_ServerStatus );
			#endif
		}

		if ( ActiveGUI.Check_Settings )
		{
			#ifdef DEBUG
			ActiveGUI.Check_Settings( Config, __FILE__, __LINE__ );
			#else
			ActiveGUI.Check_Settings( Config );
			#endif
		}

		// Set signal after Status
		Signal( Parent, SIGBREAKF_CTRL_E );

		// --

		if ( Config->cfg_ActionsServerStartEnable )
		{
			DoAction_ServerStart( Config );
		}

		// --

		#ifdef DEBUG
		DebugPrintF( "Server entering main\n" );
		#endif

		SetTaskPri( Self, PRI_SERVER );
		myProcess_Main( Config );
		SetTaskPri( Self, PRI_SHUTDOWN );

		#ifdef DEBUG
		DebugPrintF( "Server exited main\n" );
		#endif

		// --

		if ( Config->cfg_ActionsServerStopEnable )
		{
			DoAction_ServerStop( Config );
		}

		// --

		// 0 = off, 1 = starting, 2 = running, 3 = shutting down
		Config->cfg_ServerStatus = PROCESS_Stopping;

		if ( ActiveGUI.Server_Status_Change )
		{
			#ifdef DEBUG
			ActiveGUI.Server_Status_Change( Config, Config->cfg_ServerStatus, __FILE__, __LINE__ );
			#else
			ActiveGUI.Server_Status_Change( Config, Config->cfg_ServerStatus );
			#endif
		}
//		GUIFunc_UpdateServerStatus( Config );

		// --

		Parent = Config->Server_Exit;

		Config->Server_Task = NULL;
	}

	myProcess_Free( Config );

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	Config->cfg_ServerStatus = PROCESS_Stopped;

	if ( ActiveGUI.Server_Status_Change )
	{
		#ifdef DEBUG
		ActiveGUI.Server_Status_Change( Config, Config->cfg_ServerStatus, __FILE__, __LINE__ );
		#else
		ActiveGUI.Server_Status_Change( Config, Config->cfg_ServerStatus );
		#endif
	}

	if ( ActiveGUI.Check_Settings )
	{
		#ifdef DEBUG
		ActiveGUI.Check_Settings( Config, __FILE__, __LINE__ );
		#else
		ActiveGUI.Check_Settings( Config );
		#endif
	}

	//	GUIFunc_UpdateServerStatus( Config );

	//--------

	#ifdef DEBUG
	DebugPrintF( "Server stopped\n" );
	#endif

	if ( Parent )
	{
		Forbid();
		Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

S32 myStart_Server( struct Config *cfg )
{
struct StartMessage msg;
struct Process *process;
S32 error;
U32 mask;
U32 wait;

	error = TRUE;

	#ifdef DEBUG
	DebugPrintF( "myStart_Server\n" );
	#endif

	if ( DoVerbose > 2 )
	{
		printf( "myStart_Server\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_ServerStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_ServerStatus == PROCESS_Stopping ))
	{
		Delay( 2 );
	}

	if ( cfg->cfg_ServerStatus == PROCESS_Stopped )
	{
		msg.Parent = FindTask( NULL );
		msg.Config = cfg;

		#ifdef _AOS4_

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : Server Process",
			NP_Priority,	PRI_STARTUP,
			NP_Entry,		myServerProcess,
			NP_Child,		TRUE,
			NP_UserData,	& msg,
			TAG_END
		);

		#else

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : Server Process",
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

	if ( cfg->cfg_ServerStatus == PROCESS_Running )
	{
		error = FALSE;
	}

	return( error );
}

// --

void myStop_Server( struct Config *cfg )
{
U32 mask;
S32 cnt;

	#ifdef DEBUG
	DebugPrintF( "myStop_Server\n" );
	#endif

	if ( DoVerbose > 2 )
	{
		printf( "myStop_Server\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	if ( cfg->cfg_ServerStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_ServerStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_ServerStatus == PROCESS_Stopping ))
		{
			Delay( 2 );
		}

		if ( cfg->cfg_ServerStatus == PROCESS_Running )
		{
			cfg->Server_Exit = FindTask( NULL );
			cfg->cfg_ServerShutdown = TRUE;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "Server: Sending CTRL+D+E\n" );
			}

			// Clear CTRL+F
			SetSignal( 0L, SIGBREAKF_CTRL_F );

			// Send Break Signal(s)
			Signal( cfg->Server_Task, NET_EXIT_SIGNAL );

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
						Signal( cfg->Server_Task, NET_EXIT_SIGNAL );
						printf( "myStop_Server still waiting : %d\n", cnt );
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
