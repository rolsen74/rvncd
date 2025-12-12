
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if 0

struct LogTime
{
	S32					lt_Inited;			// Struct have been Initalized
	S32					lt_Ended;			// Time Stopped
	struct VNCTimeVal	lt_Logon;			// Start Tick
	struct VNCTimeVal	lt_Logoff;			// End Tick
	struct DateStamp	lt_Connect;			// Connection Time
};

#endif

// --

void LogTime_Init( struct LogTime *lt )
{
	memset( lt, 0, sizeof( struct LogTime ));

	lt->lt_Inited = 0xac;

	DateStamp(	& lt->lt_Connect );

	#ifdef _AOS4_
	GetUpTime( (PTR) & lt->lt_Logon );
	#else
	GetSysTime( (PTR) & lt->lt_Logon );
	#endif
}

// --

void LogTime_Stop( struct LogTime *lt )
{
	if ( lt->lt_Inited != 0xac )
	{
		SHELLBUF_PRINTF( "LogTime_Stop: Error not initalized\n" );
		return;
	}

	if ( ! lt->lt_Ended )
	{
		lt->lt_Ended = TRUE;

		#ifdef _AOS4_
		GetUpTime( (PTR) & lt->lt_Logoff );
		#else
		GetSysTime( (PTR) & lt->lt_Logoff );
		#endif
	}
}

// --

void LogTime_GetConnectTime( struct LogTime *lt, STR buf, S32 len )
{
char date[ LEN_DATSTRING ];
char time[ LEN_DATSTRING ];
struct DateTime dt;

	if ( lt->lt_Inited != 0xac )
	{
		SHELLBUF_PRINTF( "LogTime_GetConnectTime: Error not initalized\n" );
		return;
	}

	memset( & dt, 0, sizeof( dt ));

	dt.dat_Stamp	= lt->lt_Connect;
	dt.dat_Format	= FORMAT_DOS;
	dt.dat_StrDate	= date;
	dt.dat_StrTime	= time;

	DateToStr( & dt );

	snprintf( buf, len, "%s - %s", time, date );
}

// --

void LogTime_GetDurationTime( struct LogTime *lt, STR buf, S32 len )
{
struct VNCTimeVal cur;
char buf_years[16];
char buf_hours[16];
char buf_days[16];
char buf_mins[16];
char buf_secs[16];
S32 val_years;
S32 val_hours;
S32 val_days;
S32 val_mins;
S32 val_tmp;

	if ( lt->lt_Inited != 0xac )
	{
		SHELLBUF_PRINTF( "LogTime_GetDurationTime: Error not initalized\n" );
		return;
	}

	// --

	if ( ! lt->lt_Ended )
	{
		#ifdef _AOS4_
		GetUpTime( (PTR) & cur );
		#else
		GetSysTime( (PTR) & cur );
		#endif
	}
	else
	{
		cur = lt->lt_Logoff;
	}

	SubTime( (PTR) & cur, (PTR) & lt->lt_Logon );

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
		snprintf( buf_secs, 16, "%" PRIu32 "s ", cur.Seconds );
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

STRSession_Duration( struct Config *cfg, STR buf, struct VNCTimeVal *time )
{
struct VNCTimeVal cur;
STRyearsbuf;
STRdaysbuf;
STRhoursbuf;
STRminsbuf;
STRsecsbuf;
S32 years;
S32 days;
S32 hours;
S32 mins;
S32 val;

	if ( cfg->cfg_ClientRunning )
	{
		GetUpTime( & cur );
		SubTime( & cur, time );
	}
	else
	{
		cur = cfg->SessionStatus.si_Time_Logoff;
		SubTime( & cur, time );
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
		yearsbuf = mem_ASPrintF( "%ldy ", years );
	}
	else
	{
		yearsbuf = mem_ASPrintF( "" );
	}

	// --

	if ( days )
	{
		daysbuf = mem_ASPrintF( "%ldd ", days );
	}
	else
	{
		daysbuf = mem_ASPrintF( "" );
	}

	// --

	if ( hours )
	{
		hoursbuf = mem_ASPrintF( "%ldh ", hours );
	}
	else
	{
		hoursbuf = mem_ASPrintF( "" );
	}

	// --

	if ( mins )
	{
		minsbuf = mem_ASPrintF( "%ldm ", mins );
	}
	else
	{
		minsbuf = mem_ASPrintF( "" );
	}

	// --

	if (( cur.Seconds ) || (( years == 0 ) && ( days == 0 ) && ( hours == 0 ) && ( mins == 0 )))
	{
		secsbuf = mem_ASPrintF( "%lds ", cur.Seconds );
	}
	else
	{
		secsbuf = mem_ASPrintF( "" );
	}

	// --

	sprintf( buf, "%s%s%s%s%s", yearsbuf, daysbuf, hoursbuf, minsbuf, secsbuf );

	// --

	mem_Free( yearsbuf );
	mem_Free( daysbuf );
	mem_Free( hoursbuf );
	mem_Free( minsbuf );
	mem_Free( secsbuf );

	// --

	return( buf );
}

#endif
