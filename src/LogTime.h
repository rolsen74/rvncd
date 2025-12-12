
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __SRC__LOGTIME_H__
#define __SRC__LOGTIME_H__

// --

void			LogTime_Init(					struct LogTime *lt );
void			LogTime_Stop(					struct LogTime *lt );
void			LogTime_GetConnectTime(			struct LogTime *lt, STR buf, S32 len );
void			LogTime_GetDurationTime(		struct LogTime *lt, STR buf, S32 len );


// --

#endif // __SRC__LOGTIME_H__
