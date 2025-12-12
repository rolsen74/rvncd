
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

static S32 doAuth_Type_2( struct Config *cfg )
{
U8 *Response;	//[16];
U8 Challenge[16];
U8 Password[8];
//U8 Types[257];
U32 *auth;
S32 rejected;
S32 stat;
S32 cnt;
S32 rc;

	rejected = TRUE;

	auth	= (PTR) cfg->NetSend_SendBuffer;
	Response= (PTR) cfg->NetSend_SendBuffer;

	// -- Init Supported Auth Types

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

S32 VNC_Authenticate_37( struct Config *cfg )
{
U8 Types[257];
U32 val;
S32 rejected;
S32 cnt;
S32 rc;

	rejected = TRUE;

	// -- Init Supported Auth Types

	cnt = 1;

	if ( ! cfg->cfg_Active_Settings.Password[0] )
	{
		Types[cnt++] = 1;		// No Auth
	}
	else
	{
		Types[cnt++] = 2;		// Old VNC Authentication
	}

	Types[0] = cnt-1;		// Number of Auths

	// -- Send Supported Auth Types

	rc = Func_NetSend( cfg, & Types[0], cnt );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	rc = Func_NetRead( cfg, & Types[0], 1, 0 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	val = Types[0];

	switch( val )
	{
		case 1:
		{
			if ( ! cfg->cfg_Active_Settings.Password[0] )
			{
				rejected = FALSE;
			}
			else
			{
				rejected = TRUE;
			}
			break;
		}

		case 2:
		{
			// todo : check it we are supporting this version
			// when I added other Auth's
			rejected = doAuth_Type_2( cfg );
			break;	
		}		

		default:
		{
			rejected = TRUE;
			#ifdef DEBUG
			SHELLBUF_PRINTF( "Unsupported Auth Type (%" PRIu32 ")\n", val );
			#endif
			goto bailout;
		}
	}

	if ( rejected )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Password authenticate failed" );
	}
	else
	{
		Log_PrintF( cfg, LOGTYPE_Info, "Password accepted" );
	}

bailout:

	return( rejected );
}

// --
