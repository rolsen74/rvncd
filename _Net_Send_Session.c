 
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

char *Session_Connect_Time( char *buf, struct DateStamp *ds )
{
struct DateTime dt;
char date[ LEN_DATSTRING ];
char time[ LEN_DATSTRING ];

	memset( & dt, 0, sizeof( dt ));

	dt.dat_Stamp	= *ds;
	dt.dat_Format	= FORMAT_DOS;
	dt.dat_StrDate	= date;
	dt.dat_StrTime	= time;

	IDOS->DateToStr( & dt );

	sprintf( buf, "%s - %s", time, date );

	return( buf );
}

// --

char *Session_Bytes( char *buf, uint64 val )
{
uint64 v4;
uint64 v2;
uint64 v3;
uint64 tmp;

	if ( val < 1024LL )
	{
		sprintf( buf, "%lld B", val );
		return( buf );
	}

	v2 = 512LL;
	v3 = 1024LL;
	v4 = 1024LL * 1024LL;

	if ( val < v4 )
	{
		sprintf( buf, "%lld KB", ( val + v2 ) / ( v3 ));
		return( buf );
	}

	v2 *= 1024LL;
	v3 *= 1024LL;
	v4 *= 1024LL;

	if ( val < v4 )
	{
		tmp = ( val + v2 ) * 10 / v3;
		sprintf( buf, "%lld.%lld MB", tmp / 10, tmp % 10 );
		return( buf );
	}

	v2 *= 1024LL;
	v3 *= 1024LL;
	v4 *= 1024LL;

	if ( val < v4 )
	{
		tmp = ( val + v2 ) * 10 / v3;
		sprintf( buf, "%lld.%lld GB", tmp / 10, tmp % 10 );
		return( buf );
	}

	v2 *= 1024LL;
	v3 *= 1024LL;
//	v4 *= 1024LL;

	tmp = ( val + v2 ) * 10 / v3;
	sprintf( buf, "%lld.%lld TB", tmp / 10, tmp % 10 );

	return( buf );
};

// --

char *Session_Procent( char *buf, uint64 total, uint64 val )
{
uint64 v;

	if ( total )
	{
		v = (( val * 100LL ) / total );

		sprintf( buf, "%lld (%lld%%)", val, v );
	}
	else
	{
		sprintf( buf, "0 (--%%)" );
	}

	return( buf );
}

// --

char *Session_Duration( struct Config *cfg, char *buf, struct TimeVal *time )
{
struct TimeVal cur;
char *yearsbuf;
char *daysbuf;
char *hoursbuf;
char *minsbuf;
char *secsbuf;
int years;
int days;
int hours;
int mins;
int val;

	if ( cfg->cfg_ServerRunning )
	{
		ITimer->GetUpTime( & cur );
		ITimer->SubTime( & cur, time );
	}
	else
	{
		cur = cfg->SessionStatus.si_Time_Logoff;
		ITimer->SubTime( & cur, time );
	}

	// --

	val = 60 * 60 * 24 * 365;

	years = cur.Seconds / val;

	cur.Seconds -= val * years;

	// --

	val = 60 * 60 * 24;

	days = cur.Seconds / val;

	cur.Seconds -= val * days;

	// --

	val = 60 * 60;

	hours = cur.Seconds / val;

	cur.Seconds -= val * hours;

	// --

	val = 60;

	mins = cur.Seconds / val;

	cur.Seconds -= val * mins;

	// --

	if ( years )
	{
		yearsbuf = myASPrintF( "%ldy ", years );
	}
	else
	{
		yearsbuf = myASPrintF( "" );
	}

	// --

	if ( days )
	{
		daysbuf = myASPrintF( "%ldd ", days );
	}
	else
	{
		daysbuf = myASPrintF( "" );
	}

	// --

	if ( hours )
	{
		hoursbuf = myASPrintF( "%ldh ", hours );
	}
	else
	{
		hoursbuf = myASPrintF( "" );
	}

	// --

	if ( mins )
	{
		minsbuf = myASPrintF( "%ldm ", mins );
	}
	else
	{
		minsbuf = myASPrintF( "" );
	}

	// --

	if (( cur.Seconds ) || (( years == 0 ) && ( days == 0 ) && ( hours == 0 ) && ( mins == 0 )))
	{
		secsbuf = myASPrintF( "%lds ", cur.Seconds );
	}
	else
	{
		secsbuf = myASPrintF( "" );
	}

	// --

	sprintf( buf, "%s%s%s%s%s", yearsbuf, daysbuf, hoursbuf, minsbuf, secsbuf );

	// --

	myFree( yearsbuf );
	myFree( daysbuf );
	myFree( hoursbuf );
	myFree( minsbuf );
	myFree( secsbuf );

	// --

	return( buf );
}

// --

void myInitSessionInfo( struct Config *cfg, int a, int b, int c, int d )
{
struct CommandSession *msg;

	memset( & cfg->SessionStatus, 0, sizeof( cfg->SessionStatus ));

	ITimer->GetUpTime( & cfg->SessionStatus.si_Time_Logon );
	IDOS->DateStamp( & cfg->SessionStatus.si_Time_Connect );

	cfg->SessionStatus.si_IPAddr[0] = a;
	cfg->SessionStatus.si_IPAddr[1] = b;
	cfg->SessionStatus.si_IPAddr[2] = c;
	cfg->SessionStatus.si_IPAddr[3] = d;

	cfg->cfg_ServerPrintSession = TRUE;
	cfg->cfg_SessionUsed = TRUE;

	// --

	msg = myCalloc( sizeof( struct CommandSession ) );

	if ( msg )
	{
		msg->cs_Command = CMD_Session;

		IExec->PutMsg( CmdMsgPort, & msg->cs_Message );
	}
}

// --

void myPrintSessionInfo( struct Config *cfg )
{
struct CommandSession *msg;
char *buf;

	// --

	ITimer->GetUpTime( & cfg->SessionStatus.si_Time_Logoff );

	// --

	msg = myCalloc( sizeof( struct CommandSession ));

	if ( msg )
	{
		msg->cs_Command = CMD_Session;

		IExec->PutMsg( CmdMsgPort, & msg->cs_Message );
	}

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

	buf = myMalloc( 1024 );

	printf( "\n" );
	printf( "Session Infomation\n" );
	printf( "\n" );
	printf( "IP Address ..... : %d.%d.%d.%d\n", cfg->SessionStatus.si_IPAddr[0], cfg->SessionStatus.si_IPAddr[1], cfg->SessionStatus.si_IPAddr[2], cfg->SessionStatus.si_IPAddr[3] );
	printf( "Connected at ... : %s\n", Session_Connect_Time( buf, & cfg->SessionStatus.si_Time_Connect )),
	printf( "Duration ....... : %s\n", Session_Duration( cfg, buf, & cfg->SessionStatus.si_Time_Logon ));
	printf( "\n" );
	printf( "Bytes Read ..... : %s\n", Session_Bytes( buf, cfg->SessionStatus.si_Read_Bytes ));
	printf( "Bytes Send ..... : %s\n", Session_Bytes( buf, cfg->SessionStatus.si_Send_Bytes ));
	printf( "\n" );
	printf( "Tiles .......... : %s\n", Session_Procent( buf, cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Total ));
	printf( "Raw Tiles ...... : %s\n", Session_Procent( buf, cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Raw   ));
	printf( "ZLib Tiles ..... : %s\n", Session_Procent( buf, cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_ZLib  ));
	printf( "\n" );

	myFree( buf );

bailout:

	return;
}	

// --
