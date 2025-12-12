
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_LOGTIME_H__
#define __INC__RVNCD_LOGTIME_H__

// --

struct LogTime
{
	S32					lt_Inited;			// Struct have been Initalized
	S32					lt_Ended;			// Time Stopped
	struct VNCTimeVal	lt_Logon;			// Start Tick
	struct VNCTimeVal	lt_Logoff;			// End Tick
	struct DateStamp	lt_Connect;			// Connection Time
};

// --

void	LogTime_Init(				struct LogTime *lt );
void	LogTime_Stop(				struct LogTime *lt );
void	LogTime_GetConnectTime(		struct LogTime *lt, STR buf, S32 len );
void	LogTime_GetDurationTime(	struct LogTime *lt, STR buf, S32 len );

// --

#endif // __INC__RVNCD_LOGTIME_H__
