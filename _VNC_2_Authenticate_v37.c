
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

static int doAuth_Type_2( struct Config *cfg )
{
uint8 *Response;	//[16];
uint8 Challenge[16];
uint8 Password[8];
//uint8 Types[257];
uint32 *auth;
int rejected;
int stat;
int cnt;
int rc;

	rejected = TRUE;

	auth	= cfg->NetSend_SendBuffer;
	Response= cfg->NetSend_SendBuffer;

	// -- Init Supported Auth Types

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

int VNC_Authenticate_37( struct Config *cfg )
{
uint8 Types[257];
uint32 val;
int rejected;
int cnt;
int rc;

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

	rc = myNetSend( cfg, & Types[0], cnt );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	rc = myNetRead( cfg, & Types[0], 1, 0 );

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
			printf( "Unsupported Auth Type (%ld)\n", val );
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
