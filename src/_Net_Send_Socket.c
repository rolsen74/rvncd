
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 Send_Open_Socket( struct Config *cfg )
{
//struct SocketIFace *ISocket;
struct timeval timeout;
S32 d;
S32 enable;
S32 error;
S32 stat;
S32 s;

	error = TRUE;

	// --

	cfg->NetSend_SocketBase = OpenLibrary( "bsdsocket.library", 4 );

	if ( cfg->NetSend_SocketBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening bsdsocket library v4" );
		goto bailout;
	}

	#ifdef _AOS4_

	cfg->NetSend_ISocket = (PTR) GetInterface( cfg->NetSend_SocketBase, "main", 1, NULL );

	if ( ! cfg->NetSend_ISocket )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining bsdsocket interface" );
		goto bailout;
	}

	#endif

	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->NetSend_ISocket;
	#else
	struct Library *SocketBase = cfg->NetSend_SocketBase;
	#endif

	stat = SocketBaseTags(
		SBTM_SETVAL(SBTC_BREAKMASK), NET_EXIT_SIGNAL,
		TAG_END
	);

	if ( DoVerbose > 1 )
	{
		SHELLBUF_PRINTF( "NetSend: Set BreakMask returned %d\n", stat );
	}

	// --

	d = cfg->Server_DuplicateSendSocket;
	s = ObtainSocket( d, AF_INET, SOCK_STREAM, 0 );

	if ( s == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to obtain Socket (Sockets: %ld %ld)", s, d );
		goto bailout;
	}

	cfg->Server_DuplicateSendSocket = -1;
	cfg->NetSend_ClientSocket = s;

	// -- Enable Keep Alive

	enable = 1;

	setsockopt( cfg->NetSend_ClientSocket, SOL_SOCKET, SO_KEEPALIVE, & enable, sizeof( S32 ));

	// -- Set 10s Timeout

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	setsockopt( cfg->NetSend_ClientSocket, SOL_SOCKET, SO_RCVTIMEO, & timeout, sizeof( timeout ));

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

void Send_Close_Socket( struct Config *cfg )
{
//struct SocketIFace *ISocket;

//	ISocket = cfg->NetSend_ISocket;

	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->NetSend_ISocket;
	#else
	struct Library *SocketBase = cfg->NetSend_SocketBase;
	#endif

	if ( cfg->NetSend_ClientSocket != -1 )
	{
		CloseSocket( cfg->NetSend_ClientSocket );
		cfg->NetSend_ClientSocket = -1;
	}

	#ifdef _AOS4_

	if ( cfg->NetSend_ISocket )
	{
		DropInterface( (PTR) cfg->NetSend_ISocket );
		cfg->NetSend_ISocket = NULL;
	}

	#endif

	if ( cfg->NetSend_SocketBase )
	{
		CloseLibrary( cfg->NetSend_SocketBase );
		cfg->NetSend_SocketBase = NULL;
	}
}

// --

