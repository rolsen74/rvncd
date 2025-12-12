
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_TIMER_H__
#define __INC__RVNCD_TIMER_H__

// --

S32		Timer_Init(		void );
void	Timer_Free(		void );
void	Timer_Start(	void );
void	Timer_Stop(		void );
void	Timer_Handle(	struct Config *cfg );

// --

extern struct VNCTimeRequest *TimerIOReq;
extern struct VNCMsgPort TimerMsgPort;

// --

#endif // __INC__RVNCD_TIMER_H__
