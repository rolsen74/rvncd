
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

void myInitSessionInfo( struct Config *cfg, int a, int b, int c, int d )
{
//struct CommandSession *msg;

	memset( & cfg->SessionStatus, 0, sizeof( cfg->SessionStatus ));

	LogTime_Init( & cfg->SessionStatus.si_LogTime );

	cfg->SessionStatus.si_IPAddr[0] = a;
	cfg->SessionStatus.si_IPAddr[1] = b;
	cfg->SessionStatus.si_IPAddr[2] = c;
	cfg->SessionStatus.si_IPAddr[3] = d;

	cfg->cfg_ServerPrintSession = TRUE;
	cfg->cfg_SessionUsed = TRUE;

	// --

//	msg = myCalloc( sizeof( struct CommandSession ) );
//
//	if ( msg )
//	{
//		msg->cs_Command = CMD_Session;
//		msg->cs_Starting = 1;
//	
//		IExec->PutMsg( CmdMsgPort, & msg->cs_Message );
//	}
}

// --

void myPrintSessionInfo( struct Config *cfg )
{
//struct CommandSession *msg;

	// --

	LogTime_Stop( & cfg->SessionStatus.si_LogTime );

	// --

//	msg = myCalloc( sizeof( struct CommandSession ));
//
//	if ( msg )
//	{
//		msg->cs_Command = CMD_Session;
//		msg->cs_Starting = 0;
//
//		IExec->PutMsg( CmdMsgPort, & msg->cs_Message );
//	}

	// --

	if ( ! cfg->cfg_ServerPrintSession )
	{
		goto bailout;
	}	

	cfg->cfg_ServerPrintSession = FALSE;

	if ( ! DoVerbose )
	{
		goto bailout;
	}

	static char Buf_Tiles_Total[32];
	static char Buf_Tiles_ZLib[32];
	static char Buf_Tiles_Raw[32];
	static char Buf_Duration[64];
	static char Buf_Connect[64];
	static char Buf_Read[32];
	static char Buf_Send[32];

	LogTime_GetConnectTime(  & cfg->SessionStatus.si_LogTime, Buf_Connect,  sizeof( Buf_Connect  ));
	LogTime_GetDurationTime( & cfg->SessionStatus.si_LogTime, Buf_Duration, sizeof( Buf_Duration ));
	Session_GetProcent(	Buf_Tiles_Total,	sizeof( Buf_Tiles_Total ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Total );
	Session_GetProcent(	Buf_Tiles_ZLib,		sizeof( Buf_Tiles_ZLib ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_ZLib );
	Session_GetProcent(	Buf_Tiles_Raw,		sizeof( Buf_Tiles_Raw ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Raw );
	Session_GetBytes(	Buf_Read,			sizeof( Buf_Read ),			cfg->SessionStatus.si_Read_Bytes );
	Session_GetBytes(	Buf_Send,			sizeof( Buf_Send ),			cfg->SessionStatus.si_Send_Bytes );

	printf( "\n" );
	printf( "Session Infomation\n" );
	printf( "\n" );
	printf( "IP Address ..... : %d.%d.%d.%d\n", cfg->SessionStatus.si_IPAddr[0], cfg->SessionStatus.si_IPAddr[1], cfg->SessionStatus.si_IPAddr[2], cfg->SessionStatus.si_IPAddr[3] );
	printf( "Connected at ... : %s\n", Buf_Connect );
	printf( "Duration ....... : %s\n", Buf_Duration );
	printf( "\n" );
	printf( "Bytes Read ..... : %s\n", Buf_Read );
	printf( "Bytes Send ..... : %s\n", Buf_Send );
	printf( "\n" );
	printf( "Tiles .......... : %s\n", Buf_Tiles_Total );
	printf( "Raw Tiles ...... : %s\n", Buf_Tiles_Raw );
	printf( "ZLib Tiles ..... : %s\n", Buf_Tiles_ZLib );
	printf( "\n" );

bailout:

	return;
}	

// --

void Session_GetBytes( char *buf, int len, uint64 val )
{
uint64 v4;
uint64 v2;
uint64 v3;
uint64 tmp;

	if ( val < 1024LL )
	{
		snprintf( buf, len, "%lld B", val );
		return;
	}

	v2 = 512LL;
	v3 = 1024LL;
	v4 = 1024LL * 1024LL;

	if ( val < v4 )
	{
		snprintf( buf, len, "%lld KB", ( val + v2 ) / ( v3 ));
		return;
	}

	v2 *= 1024LL;
	v3 *= 1024LL;
	v4 *= 1024LL;

	if ( val < v4 )
	{
		tmp = ( val + v2 ) * 10 / v3;
		snprintf( buf, len, "%lld.%lld MB", tmp / 10, tmp % 10 );
		return;
	}

	v2 *= 1024LL;
	v3 *= 1024LL;
	v4 *= 1024LL;

	if ( val < v4 )
	{
		tmp = ( val + v2 ) * 10 / v3;
		snprintf( buf, len, "%lld.%lld GB", tmp / 10, tmp % 10 );
		return;
	}

	v2 *= 1024LL;
	v3 *= 1024LL;
//	v4 *= 1024LL;

	tmp = ( val + v2 ) * 10 / v3;
	snprintf( buf, len, "%lld.%lld TB", tmp / 10, tmp % 10 );
}

// --

void Session_GetProcent( char *buf, int len, uint64 total, uint64 val )
{
uint64 v;

	if ( total )
	{
		v = (( val * 100LL ) / total );

		snprintf( buf, len, "%lld (%lld%%)", val, v );
	}
	else
	{
		snprintf( buf, len, "0 (--%%)" );
	}
}

// --
