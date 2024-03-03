 
/*
 * Copyright (c) 2023-2024 Rene W. Olsen < renewolsen @ gmail . com >
 *
 * This software is released under the GNU General Public License, version 3.
 * For the full text of the license, please visit:
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * You can also find a copy of the license in the LICENSE file included with this software.
 */

// --

#include "RVNCd.h"

// --

static int myCheck_IP( struct Config *cfg UNUSED, struct IPNode *node, int a, int b, int c, int d )
{
int retval;

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

static void mySetIPConfig( struct Config *cfg UNUSED, int cfgnr UNUSED )
{

}

// --

static int myValidateIP( struct Config *cfg, char *addrbuf, int a, int b, int c, int d )
{
struct IPNode *node;
int rejected;
int users;

	rejected = TRUE;

	if ((( a < 0 ) || ( a > 255 ))
	||	(( b < 0 ) || ( b > 255 ))
	||	(( c < 0 ) || ( c > 255 ))
	||	(( d < 0 ) || ( d > 255 )))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Invalid IP format (%ld.%ld.%ld.%ld)", a, b, c, d );
		goto bailout;
	}

	// -- White List Check

	node = (APTR) IExec->GetHead( & cfg->WhiteList );

	while( node )
	{
		if ( myCheck_IP( cfg, node, a, b, c, d ))
		{
			break;
		}
		else
		{
			node = (APTR) IExec->GetSucc( (APTR) node );
		}
	}

	if ( node )
	{
		if ( cfg->cfg_LogUserConnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Connection from %s accepted. (Whitelisted)", addrbuf );
		}

		if ( node->ipn_CfgNr >= 0 )
		{
			mySetIPConfig( cfg, node->ipn_CfgNr );
		}

		cfg->Connect_Accecpted++;
		cfg->Connect_WhiteListed++;

// printf( "WhiteListed: %ld, Total Accecpted: %ld\n", cfg->Connect_WhiteListed, cfg->Connect_Accecpted );

		rejected = FALSE;
		goto bailout;
	}

	// -- Black List Check

	node = (APTR) IExec->GetHead( & cfg->BlackList );

	while( node )
	{
		if ( myCheck_IP( cfg, node, a, b, c, d ))
		{
			break;
		}
		else
		{
			node = (APTR) IExec->GetSucc( (APTR) node );
		}
	}

	if ( node )
	{
		if ( cfg->cfg_LogUserConnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Connection from %s rejected. (Blacklisted)", addrbuf );
		}

		cfg->Connect_Rejected++;
		cfg->Connect_BlackListed++;

//		printf( "BlackListed: %ld, Total Rejected: %ld\n", cfg->Connect_BlackListed, cfg->Connect_Rejected );

		goto bailout;
	}

	// -- Passed .. Normal connections

	IExec->Forbid();
	users = cfg->UserCount;
	IExec->Permit();

	if ( users < MAX_USERS )
	{
		if ( cfg->cfg_LogUserConnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Connection from %s accepted.", addrbuf );
		}

		cfg->Connect_Accecpted++;

//		printf( "WhiteListed: %ld, Total Accecpted: %ld\n", cfg->Connect_WhiteListed, cfg->Connect_Accecpted );
	}
	else
	{
		Log_PrintF( cfg, LOGTYPE_Info, "Server: Closing connection, maximum users reached." );

		cfg->Connect_Rejected++;

//		printf( "BlackListed: %ld, Total Rejected: %ld\n", cfg->Connect_BlackListed, cfg->Connect_Rejected );
		goto bailout;
	}

	rejected = FALSE;

bailout:

	{
		struct CommandMessage *cmsg;

		cmsg = myCalloc( sizeof( struct CommandMessage ) );

		if ( cmsg )
		{
			cmsg->cm_Command = CMD_ConnectStats;

			IExec->PutMsg( CmdMsgPort, & cmsg->cm_Message );
		}
	}

	return( rejected );
}

// --

static int myOpen_Net( struct Config *cfg )
{
struct SocketIFace *ISocket;
struct sockaddr_in serverAddress;
int error;
int reuse;

	error = TRUE;

	cfg->Server_SocketBase = IExec->OpenLibrary( "bsdsocket.library", 4 );

	if ( cfg->Server_SocketBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Error opening bsdsocket library v4" );
		goto bailout;
	}

	ISocket = (APTR) IExec->GetInterface( cfg->Server_SocketBase, "main", 1, NULL );

	if ( ISocket == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Error obtaining bsdsocket interface" );
		goto bailout;
	}

	cfg->Server_ISocket = ISocket;

	ISocket->SocketBaseTags(
		SBTM_SETVAL(SBTC_BREAKMASK), SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E,
		TAG_END
	);

	// --

	cfg->Server_ListenSocket = ISocket->socket( AF_INET, SOCK_STREAM, 0 );

	if ( cfg->Server_ListenSocket == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Failed to create listen socket '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	// --	

	reuse = 1;

	if ( ISocket->setsockopt( cfg->Server_ListenSocket, SOL_SOCKET, SO_REUSEADDR, & reuse, sizeof( int )) < 0 )
	{
printf( "set sock opt failed\n" );
	}

	// --

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = INADDR_ANY;
	serverAddress.sin_port = htons( cfg->cfg_Active_Settings.Port );

	if ( ISocket->bind( cfg->Server_ListenSocket, (struct sockaddr *) & serverAddress, sizeof( serverAddress )) == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Failed to bind listen socket '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( ISocket->listen( cfg->Server_ListenSocket, MAX_SERVERS ) == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Server: Failed to listen on socket '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
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
struct SocketIFace *ISocket;
int s;

	ISocket = cfg->Server_ISocket;

	if ( cfg->Server_DuplicateSendSocket != -1 )
	{
		s = ISocket->ObtainSocket( cfg->Server_DuplicateSendSocket, AF_INET, SOCK_STREAM, 0 );
		if ( s != -1 ) ISocket->CloseSocket( s );
		cfg->Server_DuplicateSendSocket = -1;
	}

	if ( cfg->Server_DuplicateReadSocket != -1 )
	{
		s = ISocket->ObtainSocket( cfg->Server_DuplicateReadSocket, AF_INET, SOCK_STREAM, 0 );
		if ( s != -1 ) ISocket->CloseSocket( s );
		cfg->Server_DuplicateReadSocket = -1;
	}

	if ( cfg->NetSend_ClientSocket != -1 )
	{
		ISocket->CloseSocket( cfg->NetSend_ClientSocket );
		cfg->NetSend_ClientSocket = -1;
	}

	if ( cfg->Server_ListenSocket != -1 )
	{
		ISocket->CloseSocket( cfg->Server_ListenSocket );
		cfg->Server_ListenSocket = -1;
	}

	if ( cfg->Server_ISocket )
	{
		IExec->DropInterface( (APTR) cfg->Server_ISocket );
		cfg->Server_ISocket = NULL;
	}

	if ( cfg->Server_SocketBase )
	{
		IExec->CloseLibrary( cfg->Server_SocketBase );
		cfg->Server_SocketBase = NULL;
	}
}

// --

static void myProcess_Main( struct Config *cfg )
{
struct SocketIFace *ISocket;
struct sockaddr_in clientAddress;
socklen_t clientAddressLength;
char addrbuf[ INET_ADDRSTRLEN ];
int a, b, c, d, s;

	clientAddressLength = sizeof( clientAddress );

	ISocket = cfg->Server_ISocket;

	Log_PrintF( cfg, LOGTYPE_Info, "Listening on port %ld.", cfg->cfg_Active_Settings.Port );

	while( ! cfg->cfg_ServerShutdown )
	{
		if ( cfg->Server_DuplicateSendSocket != -1 )
		{
			s = ISocket->ObtainSocket( cfg->Server_DuplicateSendSocket, AF_INET, SOCK_STREAM, 0 );
			if ( s != -1 ) ISocket->CloseSocket( s );
			cfg->Server_DuplicateSendSocket = -1;
		}

		if ( cfg->Server_DuplicateReadSocket != -1 )
		{
			s = ISocket->ObtainSocket( cfg->Server_DuplicateReadSocket, AF_INET, SOCK_STREAM, 0 );
			if ( s != -1 ) ISocket->CloseSocket( s );
			cfg->Server_DuplicateReadSocket = -1;
		}

		if ( cfg->Server_ClientSocket != -1 )
		{
			ISocket->CloseSocket( cfg->Server_ClientSocket );
			cfg->Server_ClientSocket = -1;
		}

		// -- Wait for next connection

		cfg->Server_ClientSocket = ISocket->accept( cfg->Server_ListenSocket, (void *) & clientAddress, & clientAddressLength );

		if ( cfg->Server_ClientSocket == -1 )
		{
			if ( ! cfg->Server_Exit )
			{
				Log_PrintF( cfg, LOGTYPE_Error, "Server: accept() failed with '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
			}
			break;
		}

		// -- Get IP Infomation

		if ( ISocket->inet_ntop( AF_INET, & clientAddress.sin_addr, addrbuf, INET_ADDRSTRLEN ) == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Server: inet_ntop() failed with '%s'. (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
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

		cfg->Server_DuplicateSendSocket = ISocket->ReleaseCopyOfSocket( cfg->Server_ClientSocket, UNIQUE_ID );

		if ( cfg->Server_DuplicateSendSocket == -1 )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Server: Failed to clone Socket. (%d)", myStrError( ISocket->Errno() ), ISocket->Errno() );
			continue;
		}

		cfg->Server_DuplicateReadSocket = ISocket->ReleaseCopyOfSocket( cfg->Server_ClientSocket, UNIQUE_ID );

		if ( cfg->Server_DuplicateReadSocket == -1 )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Server: Failed to clone Socket. (%d)", myStrError( ISocket->Errno() ), ISocket->Errno() );
			continue;
		}

		myStart_Net_Send( cfg );
	}

	// -- 

//	Log_PrintF( cfg, LOGTYPE_Info, "Server stopping" );
}

// --

static int myProcess_Init( struct Config *cfg )
{
int retval;
int ts;

	retval = FALSE;

	cfg->Server_ListenSocket = -1;
	cfg->Server_ClientSocket = -1;
	cfg->Server_DuplicateSendSocket = -1;
	cfg->Server_DuplicateReadSocket = -1;

	// --

	if ( cfg->cfg_LogServerStart )
	{
		Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Server starting up .." );
	}

	// Copy Settings
	memcpy( & cfg->cfg_Active_Settings, & cfg->cfg_Disk_Settings, sizeof( struct Settings ));

	// --

	ts = MAX(  16, cfg->cfg_TileSize );
	ts = MIN( 256, ts );
	cfg->GfxRead_Screen_TileSize = ts;

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
APTR un;

	// --

	if ( cfg->cfg_LogServerStop )
	{
		Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "Server shutting down .." );
	}

	// --

	myStop_Net_Send( cfg );

	myStop_Gfx_Read( cfg );

	Send_Free_Pointer( cfg );

	myClose_Net( cfg );

	// --

	IExec->ObtainSemaphore( & cfg->Server_UpdateSema );

	while( TRUE )
	{
		un = (APTR) IExec->RemHead( & cfg->Server_UpdateList );

		if ( un == NULL )
		{
			break;
		}

		myFree( un );
	}

	while( TRUE )
	{
		un = (APTR) IExec->RemHead( & cfg->Server_UpdateFree );

		if ( un == NULL )
		{
			break;
		}

		myFree( un );
	}

	IExec->ReleaseSemaphore( & cfg->Server_UpdateSema );

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

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	Config->cfg_ServerStatus = PROCESS_Starting;
	GUIFunc_UpdateServerStatus( Config );

	stat = myProcess_Init( Config );

	if ( stat )
	{
		Config->Server_Task = Self;
	
		// 0 = off, 1 = starting, 2 = running, 3 = shutting down
		Config->cfg_ServerStatus = PROCESS_Running;
		GUIFunc_UpdateServerStatus( Config );

		// Set signal after Status
		IExec->Signal( Parent, SIGBREAKF_CTRL_E );

		// --

		if ( Config->cfg_ActionsServerStartEnable )
		{
			DoAction_ServerStart( Config );
		}

		myProcess_Main( Config );

		if ( Config->cfg_ActionsServerStopEnable )
		{
			DoAction_ServerStop( Config );
		}

		// --

		// 0 = off, 1 = starting, 2 = running, 3 = shutting down
		Config->cfg_ServerStatus = PROCESS_Stopping;
		GUIFunc_UpdateServerStatus( Config );

		// --

		Parent = Config->Server_Exit;

		Config->Server_Task = NULL;
	}

	myProcess_Free( Config );

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	Config->cfg_ServerStatus = PROCESS_Stopped;
	GUIFunc_UpdateServerStatus( Config );

	//--------

	if ( Parent )
	{
		IExec->Forbid();
		IExec->Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

int myStart_Server( struct Config *cfg )
{
struct StartMessage msg;
uint32 mask;
int error;

	error = TRUE;

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_ServerStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_ServerStatus == PROCESS_Stopping ))
	{
		IDOS->Delay( 2 );
	}

	if ( cfg->cfg_ServerStatus == PROCESS_Stopped )
	{
		msg.Parent = IExec->FindTask( NULL );
		msg.Config = cfg;

		if ( IDOS->CreateNewProcTags(
			NP_Name,	    "Server Process",
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

	if ( cfg->cfg_ServerStatus == PROCESS_Running )
	{
		error = FALSE;
	}

	return( error );
}

// --

void myStop_Server( struct Config *cfg )
{
uint32 mask;
int cnt;

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	if ( cfg->cfg_ServerStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_ServerStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_ServerStatus == PROCESS_Stopping ))
		{
			IDOS->Delay( 2 );
		}

		if ( cfg->cfg_ServerStatus == PROCESS_Running )
		{
			cfg->Server_Exit = IExec->FindTask( NULL );

			IExec->Signal( cfg->Server_Task, SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E );

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
						printf( "\rmyStop_Server still waiting : %d", cnt );
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
