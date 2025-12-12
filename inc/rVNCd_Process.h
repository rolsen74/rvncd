
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_PROCESS_H__
#define __INC__RVNCD_PROCESS_H__

// --
// -- Process Status

enum ProcessStat
{
	PROCESS_Stopped,
	PROCESS_Starting,
	PROCESS_Running,
	PROCESS_Stopping,
};

// --

struct StartMessage
{
	struct Task *	Parent;
	struct Config *	Config;
};

// --

S32				myStart_Server(			struct Config *cfg );
void			myStop_Server(			struct Config *cfg );

S32				myStart_Net_Read(		struct Config *cfg );
void			myStop_Net_Read(		struct Config *cfg );

S32				myStart_Net_Send(		struct Config *cfg );
void			myStop_Net_Send(		struct Config *cfg );

S32				myStart_Gfx_Read(		struct Config *cfg );
void			myStop_Gfx_Read(		struct Config *cfg );

S32				myStart_WatchDog(		struct Config *cfg );
void			myStop_WatchDog(		struct Config *cfg );
void			WatchDog_StartTimer(	struct Config *cfg );
void			WatchDog_StopTimer(		struct Config *cfg );

// --

#endif // __INC__RVNCD_PROCESS_H__
