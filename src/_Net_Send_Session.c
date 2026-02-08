
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

void myInitSessionInfo( struct Config *cfg, S32 a, S32 b, S32 c, S32 d )
{
//struct CommandSession *msg;

	memset( & cfg->SessionStatus, 0, sizeof( cfg->SessionStatus ));

	LogTime_Init( & cfg->SessionStatus.si_LogTime );

	cfg->SessionStatus.si_Pixels_Delay	= 5;	// Start delay
	cfg->SessionStatus.si_Pixels_Freq	= 0;	// Update freq .. 1+x sec

	cfg->SessionStatus.si_IPAddr[0] = a;
	cfg->SessionStatus.si_IPAddr[1] = b;
	cfg->SessionStatus.si_IPAddr[2] = c;
	cfg->SessionStatus.si_IPAddr[3] = d;

	cfg->cfg_ServerPrintSession = TRUE;
	cfg->cfg_SessionUsed = TRUE;

	// --

//	msg = mem_AllocClr( sizeof( struct CommandSession ) );
//
//	if ( msg )
//	{
//		msg->cs_Command = CMD_Session;
//		msg->cs_Starting = 1;
//	
//		MsgPort_PutMsg( CmdMsgPort, & msg->cs_Message );
//	}
}

// --

void myPrintSessionInfo( struct Config *cfg )
{
//struct CommandSession *msg;

	// --

	LogTime_Stop( & cfg->SessionStatus.si_LogTime );

	// --

//	msg = mem_AllocClr( sizeof( struct CommandSession ));
//
//	if ( msg )
//	{
//		msg->cs_Command = CMD_Session;
//		msg->cs_Starting = 0;
//
//		MsgPort_PutMsg( CmdMsgPort, & msg->cs_Message );
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
	static char Buf_Tiles_Rich[32];
	static char Buf_Tiles_ZLib[32];
	static char Buf_Tiles_RRE[32];
	static char Buf_Tiles_Raw[32];
	static char Buf_Duration[64];
	static char Buf_Connect[64];
	static char Buf_Read[32];
	static char Buf_Send[32];

	LogTime_GetConnectTime(  & cfg->SessionStatus.si_LogTime, Buf_Connect,  sizeof( Buf_Connect  ));
	LogTime_GetDurationTime( & cfg->SessionStatus.si_LogTime, Buf_Duration, sizeof( Buf_Duration ));
	Session_GetProcent(	Buf_Tiles_Total,	sizeof( Buf_Tiles_Total ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Total );
	Session_GetProcent(	Buf_Tiles_Rich,		sizeof( Buf_Tiles_Rich ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Rich );
	Session_GetProcent(	Buf_Tiles_ZLib,		sizeof( Buf_Tiles_ZLib ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_ZLib );
	Session_GetProcent(	Buf_Tiles_RRE,		sizeof( Buf_Tiles_RRE ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_RRE );
	Session_GetProcent(	Buf_Tiles_Raw,		sizeof( Buf_Tiles_Raw ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Raw );
	Session_GetBytes(	Buf_Read,			sizeof( Buf_Read ),			cfg->SessionStatus.si_Read_Bytes );
	Session_GetBytes(	Buf_Send,			sizeof( Buf_Send ),			cfg->SessionStatus.si_Send_Bytes );

	SHELLBUF_PRINTF( "\n" );
	SHELLBUF_PRINTF( "Session Infomation\n" );
	SHELLBUF_PRINTF( "\n" );
	SHELLBUF_PRINTF4( "IP Address ..... : %d.%d.%d.%d\n", cfg->SessionStatus.si_IPAddr[0], cfg->SessionStatus.si_IPAddr[1], cfg->SessionStatus.si_IPAddr[2], cfg->SessionStatus.si_IPAddr[3] );
	SHELLBUF_PRINTF1( "Connected at ... : %s\n", Buf_Connect );
	SHELLBUF_PRINTF1( "Duration ....... : %s\n", Buf_Duration );
	SHELLBUF_PRINTF( "\n" );
	SHELLBUF_PRINTF1( "Pixel Send ..... : %llu\n", cfg->SessionStatus.si_Pixels_Send );
	SHELLBUF_PRINTF1( "Bytes Read ..... : %s\n", Buf_Read );
	SHELLBUF_PRINTF1( "Bytes Send ..... : %s\n", Buf_Send );
	SHELLBUF_PRINTF( "\n" );
	SHELLBUF_PRINTF1( "Tiles .......... : %s\n", Buf_Tiles_Total );
	SHELLBUF_PRINTF1( "Rich Tiles ..... : %s\n", Buf_Tiles_Rich );
	SHELLBUF_PRINTF1( "Raw Tiles ...... : %s\n", Buf_Tiles_Raw );
	SHELLBUF_PRINTF1( "RRE Tiles ...... : %s\n", Buf_Tiles_RRE );
	SHELLBUF_PRINTF1( "ZLib Tiles ..... : %s\n", Buf_Tiles_ZLib );
	SHELLBUF_PRINTF( "\n" );

bailout:

	return;
}	

// --

void Session_GetBytes( STR buf, S32 len, U64 val )
{
U64 v4;
U64 v2;
U64 v3;
U64 tmp;

	if ( val < 1024LL )
	{
		snprintf( buf, len, "%" PRIu64 " B", val );
		return;
	}

	v2 = 512LL;
	v3 = 1024LL;
	v4 = 1024LL * 1024LL;

	if ( val < v4 )
	{
		snprintf( buf, len, "%" PRIu64 " KB", ( val + v2 ) / ( v3 ));
		return;
	}

	v2 *= 1024LL;
	v3 *= 1024LL;
	v4 *= 1024LL;

	if ( val < v4 )
	{
		tmp = ( val + v2 ) * 10 / v3;
		snprintf( buf, len, "%" PRIu64 ".%" PRIu64 " MB", tmp / 10, tmp % 10 );
		return;
	}

	v2 *= 1024LL;
	v3 *= 1024LL;
	v4 *= 1024LL;

	if ( val < v4 )
	{
		tmp = ( val + v2 ) * 10 / v3;
		snprintf( buf, len, "%" PRIu64 ".%" PRIu64 " GB", tmp / 10, tmp % 10 );
		return;
	}

	v2 *= 1024LL;
	v3 *= 1024LL;
//	v4 *= 1024LL;

	tmp = ( val + v2 ) * 10 / v3;
	snprintf( buf, len, "%" PRIu64 ".%" PRIu64 " TB", tmp / 10, tmp % 10 );
}

// --

void Session_GetProcent( STR buf, S32 len, U64 total, U64 val )
{
U64 v;

	if ( total )
	{
		v = (( val * 100LL ) / total );

		snprintf( buf, len, "%" PRIu64 " (%" PRIu64 "%%)", val, v );
	}
	else
	{
		snprintf( buf, len, "0 (--%%)" );
	}
}

// --
