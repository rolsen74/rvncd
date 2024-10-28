
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

int Send_Open_Socket( struct Config *cfg )
{
struct SocketIFace *ISocket;
struct timeval timeout;
int32 d;
int enable;
int error;
int s;

	error = TRUE;

	// --

	cfg->NetSend_SocketBase = IExec->OpenLibrary( "bsdsocket.library", 4 );

	if ( cfg->NetSend_SocketBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening bsdsocket library v4" );
		goto bailout;
	}

	ISocket = (APTR) IExec->GetInterface( cfg->NetSend_SocketBase, "main", 1, NULL );

	if ( ISocket == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining bsdsocket interface" );
		goto bailout;
	}

	cfg->NetSend_ISocket = ISocket;

	ISocket->SocketBaseTags(
		SBTM_SETVAL(SBTC_BREAKMASK), SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E,
		TAG_END
	);

	// --

	d = cfg->Server_DuplicateSendSocket;
	s = ISocket->ObtainSocket( d, AF_INET, SOCK_STREAM, 0 );

	if ( s == -1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to obtain Socket (Sockets: %ld %ld)", s, d );
		goto bailout;
	}

	cfg->Server_DuplicateSendSocket = -1;
	cfg->NetSend_ClientSocket = s;

	// -- Enable Keep Alive

	enable = 1;

	ISocket->setsockopt( cfg->NetSend_ClientSocket, SOL_SOCKET, SO_KEEPALIVE, & enable, sizeof( int ));

	// -- Set 10s Timeout

	timeout.tv_sec = 10;
	timeout.tv_usec = 0;

	ISocket->setsockopt( cfg->NetSend_ClientSocket, SOL_SOCKET, SO_RCVTIMEO, & timeout, sizeof( timeout ));

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

void Send_Close_Socket( struct Config *cfg )
{
struct SocketIFace *ISocket;

	ISocket = cfg->NetSend_ISocket;

	if ( cfg->NetSend_ClientSocket != -1 )
	{
		ISocket->CloseSocket( cfg->NetSend_ClientSocket );
		cfg->NetSend_ClientSocket = -1;
	}

	if ( cfg->NetSend_ISocket )
	{
		IExec->DropInterface( (APTR) cfg->NetSend_ISocket );
		cfg->NetSend_ISocket = NULL;
	}

	if ( cfg->NetSend_SocketBase )
	{
		IExec->CloseLibrary( cfg->NetSend_SocketBase );
		cfg->NetSend_SocketBase = NULL;
	}
}

// --

