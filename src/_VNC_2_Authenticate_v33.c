
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

static S32 doAuth( struct Config *cfg )
{
U8 *Response;	//[16];
U8 Challenge[16];
U8 Password[8];
U32 *auth;
S32 rejected;
S32 stat;
S32 cnt;
S32 rc;

	rejected = TRUE;

	auth	= (PTR) cfg->NetSend_SendBuffer;
	Response= (PTR) cfg->NetSend_SendBuffer;

	// --

	*auth = 2; // Need Auth

	rc = Func_NetSend( cfg, auth, 4 );

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

	rc = Func_NetSend( cfg, cfg->NetSend_SendBuffer, 16 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	// -- Read Response

	rc = Func_NetRead( cfg, cfg->NetSend_SendBuffer, 16, 0 );

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

	rc = Func_NetSend( cfg, auth, 4 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	rejected = ( stat ) ? TRUE : FALSE ;

bailout:

	return( rejected );
}

// --

static S32 doNoAuth( struct Config *cfg )
{
U32 *auth;
S32 rejected;
S32 rc;

	rejected = TRUE;

	auth = (PTR) cfg->NetSend_SendBuffer;

	*auth = 1; // No Auth

	rc = Func_NetSend( cfg, auth, 4 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	rejected = FALSE;

bailout:

	return( rejected );
}

// --

S32 VNC_Authenticate_33( struct Config *cfg )
{
S32 rejected;

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
