
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __SRC__LOG_H__
#define __SRC__LOG_H__

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

#endif // __SRC__LOG_H__

