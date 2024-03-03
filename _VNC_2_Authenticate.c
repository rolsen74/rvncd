 
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

/*
** Purpose:
** - Validate Authenticate
**
** Returns:
** - True and the socket will be closed
** - False and we continue
*/

// --

static int doAuth( struct Config *cfg )
{
struct SocketIFace *ISocket;
uint8 *Response;	//[16];
uint8 Challenge[16];
uint8 Password[8];
uint32 *auth;
int rejected;
int stat;
int cnt;
int rc;

	rejected = TRUE;

	ISocket = cfg->NetSend_ISocket;
	auth	= cfg->NetSend_SendBuffer;
	Response= cfg->NetSend_SendBuffer;

	// --

	*auth = 2; // Need Auth

	rc = ISocket->send( cfg->NetSend_ClientSocket, auth, 4, 0 );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to send data (%d)", ISocket->Errno() );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "Failed to send data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Send_Bytes += rc;

	// -- Send Challenge

	for( cnt=0 ; cnt<16 ; cnt++ )
	{
		Challenge[cnt] = rand() & 0xff;
	}

	memcpy( cfg->NetSend_SendBuffer, Challenge, 16 );

	rc = ISocket->send( cfg->NetSend_ClientSocket, cfg->NetSend_SendBuffer, 16, 0 );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to send data (%d)", ISocket->Errno() );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "Failed to send data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Send_Bytes += rc;

	// -- Read Response

	rc = ISocket->recv( cfg->NetSend_ClientSocket, cfg->NetSend_SendBuffer, 16, 0 );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to read data (%d)", ISocket->Errno() );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "Failed to read data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		cfg->cfg_ServerRunning = FALSE;

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Read_Bytes += rc;

	// -- Check Response

	for( cnt=0 ; cnt < 8 && cfg->cfg_Active_Settings.Password[cnt] ; cnt++ )
	{
		Password[cnt] = cfg->cfg_Active_Settings.Password[cnt];
	}

	for( ; cnt<8 ; cnt++ )
	{
		Password[cnt] = 0;
	}

	deskey( Password, DE1 );

	des( & Response[0], & Response[0] );
	des( & Response[8], & Response[8] );

	stat = memcmp( Challenge, Response, 16 );

	// 0 = Okay
	// 1 = Failed .. this also closes connection .. no retries
	// 2 = Too Many fails.. close connection
	*auth = ( stat ) ? 1 : 0 ;

	if ( *auth )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Password authenticate failed" );
	}
	else
	{
		Log_PrintF( cfg, LOGTYPE_Info, "Password accepted" );
	}

	rc = ISocket->send( cfg->NetSend_ClientSocket, auth, 4, 0 );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to read data (%d)", ISocket->Errno() );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "Failed to send data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Send_Bytes += rc;

	rejected = ( stat ) ? TRUE : FALSE ;

bailout:

	return( rejected );
}

// --

static int doNoAuth( struct Config *cfg )
{
struct SocketIFace *ISocket;
uint32 *auth;
int rejected;
int rc;

	rejected = TRUE;

	ISocket = cfg->NetSend_ISocket;

	auth = cfg->NetSend_SendBuffer;

	*auth = 1; // No Auth

	rc = ISocket->send( cfg->NetSend_ClientSocket, auth, 4, 0 );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to send data (%d)", ISocket->Errno() );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "Failed to send data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Send_Bytes += rc;

	rejected = FALSE;

bailout:

	return( rejected );
}

// --

int VNC_Authenticate( struct Config *cfg )
{
int rejected;

	if ( cfg->cfg_Active_Settings.Password[0] )
	{
		rejected = doAuth( cfg );
	}
	else
	{
		rejected = doNoAuth( cfg );
	}

	return( rejected );
}

// --
