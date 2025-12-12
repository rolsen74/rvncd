
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_LOG_H__
#define __INC__RVNCD_LOG_H__

// --

enum
{
	LOGTYPE_Info = 0,
	LOGTYPE_Warning,
	LOGTYPE_Error,
	LOGTYPE_Event = 0x80,
};

// --

void	Log_PrintF(				struct Config *cfg, S32 type, const STR fmt, ... );

// --

#endif // __INC__RVNCD_LOG_H__
