
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

static int doAuth( struct Config *cfg )
{
uint8 *Response;	//[16];
uint8 Challenge[16];
uint8 Password[8];
uint32 *auth;
int rejected;
int stat;
int cnt;
int rc;

	rejected = TRUE;

	auth	= cfg->NetSend_SendBuffer;
	Response= cfg->NetSend_SendBuffer;

	// --

	*auth = 2; // Need Auth

	rc = myNetSend( cfg, auth, 4 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	// -- Send Challenge

	for( cnt=0 ; cnt<16 ; cnt++ )
	{
		Challenge[cnt] = rand() & 0xff;
	}

	memcpy( cfg->NetSend_SendBuffer, Challenge, 16 );

	rc = myNetSend( cfg, cfg->NetSend_SendBuffer, 16 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	// -- Read Response

	rc = myNetRead( cfg, cfg->NetSend_SendBuffer, 16, 0 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

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

	rc = myNetSend( cfg, auth, 4 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	rejected = ( stat ) ? TRUE : FALSE ;

bailout:

	return( rejected );
}

// --

static int doNoAuth( struct Config *cfg )
{
uint32 *auth;
int rejected;
int rc;

	rejected = TRUE;

	auth = cfg->NetSend_SendBuffer;

	*auth = 1; // No Auth

	rc = myNetSend( cfg, auth, 4 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	rejected = FALSE;

bailout:

	return( rejected );
}

// --

int VNC_Authenticate_33( struct Config *cfg )
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
