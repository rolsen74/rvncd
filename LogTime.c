
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

#if 0

struct LogTime
{
	int					lt_Inited;			// Struct have been Initalized
	int					lt_Ended;			// Time Stopped
	struct TimeVal		lt_Logon;			// Start Tick
	struct TimeVal		lt_Logoff;			// End Tick
	struct DateStamp	lt_Connect;			// Connection Time
};

#endif

// --

void LogTime_Init( struct LogTime *lt )
{
	memset( lt, 0, sizeof( struct LogTime ));

	lt->lt_Inited = 0xac;

	IDOS->DateStamp(	& lt->lt_Connect );
	ITimer->GetUpTime(	& lt->lt_Logon );
}

// --

void LogTime_Stop( struct LogTime *lt )
{
	if ( lt->lt_Inited != 0xac )
	{
		printf( "LogTime_Stop: Error not initalized\n" );
		return;
	}

	if ( ! lt->lt_Ended )
	{
		lt->lt_Ended = TRUE;

		ITimer->GetUpTime( & lt->lt_Logoff );
	}
}

// --

void LogTime_GetConnectTime( struct LogTime *lt, char *buf, int len )
{
char date[ LEN_DATSTRING ];
char time[ LEN_DATSTRING ];
struct DateTime dt;

	if ( lt->lt_Inited != 0xac )
	{
		printf( "LogTime_GetConnectTime: Error not initalized\n" );
		return;
	}

	memset( & dt, 0, sizeof( dt ));

	dt.dat_Stamp	= lt->lt_Connect;
	dt.dat_Format	= FORMAT_DOS;
	dt.dat_StrDate	= date;
	dt.dat_StrTime	= time;

	IDOS->DateToStr( & dt );

	snprintf( buf, len, "%s - %s", time, date );
}

// --

void LogTime_GetDurationTime( struct LogTime *lt, char *buf, int len )
{
struct TimeVal cur;
char buf_years[16];
char buf_hours[16];
char buf_days[16];
char buf_mins[16];
char buf_secs[16];
int val_years;
int val_hours;
int val_days;
int val_mins;
int val_tmp;

	if ( lt->lt_Inited != 0xac )
	{
		printf( "LogTime_GetDurationTime: Error not initalized\n" );
		return;
	}

	// --

	if ( ! lt->lt_Ended )
	{
		ITimer->GetUpTime( & cur );
	}
	else
	{
		cur = lt->lt_Logoff;
	}

	ITimer->SubTime( & cur, & lt->lt_Logon );

	// --

	val_tmp = 60 * 60 * 24 * 365;

	val_years = cur.Seconds / val_tmp;

	cur.Seconds -= val_tmp * val_years;

	// --

	val_tmp = 60 * 60 * 24;

	val_days = cur.Seconds / val_tmp;

	cur.Seconds -= val_tmp * val_days;

	// --

	val_tmp = 60 * 60;

	val_hours = cur.Seconds / val_tmp;

	cur.Seconds -= val_tmp * val_hours;

	// --

	val_tmp = 60;

	val_mins = cur.Seconds / val_tmp;

	cur.Seconds -= val_tmp * val_mins;

	// --

	if ( val_years )
	{
		snprintf( buf_years, 16, "%dy ", val_years );
	}
	else
	{
		buf_years[0] = 0;
	}

	// --

	if ( val_days )
	{
		snprintf( buf_days, 16, "%dd ", val_days );
	}
	else
	{
		buf_days[0] = 0;
	}

	// --

	if ( val_hours )
	{
		snprintf( buf_hours, 16, "%dh ", val_hours );
	}
	else
	{
		buf_hours[0] = 0;
	}

	// --

	if ( val_mins )
	{
		snprintf( buf_mins, 16, "%dm ", val_mins );
	}
	else
	{
		buf_mins[0] = 0;
	}

	// --

	if (( cur.Seconds ) || (( ! val_years ) && ( ! val_days ) && ( ! val_hours ) && ( ! val_mins )))
	{
		snprintf( buf_secs, 16, "%lds ", cur.Seconds );
	}
	else
	{
		buf_secs[0] = 0;
	}

	// --

	snprintf( buf, len, "%s%s%s%s%s", buf_years, buf_days, buf_hours, buf_mins, buf_secs );
}

// --

#if 0

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

	if ( cfg->cfg_ClientRunning )
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

#endif
