
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __RVNCD_GUI_H__
#define __RVNCD_GUI_H__

// --

#define		WIN_LAST	1

struct WinNode
{
	S16				XPos;
	S16				YPos;
	S16				Width;
	S16				Height;
	S16				Busy;
	S32				Status;			// Save
	struct Window *	WindowAdr;
	void			(*BusyWin)	( struct Config *cfg, S32 val );
	S32				(*OpenWin)	( struct Config *cfg );
	void			(*CloseWin)	( struct Config *cfg );
	void			(*HandleWin)( struct Config *cfg );
	void			(*TimerTick)( struct Config *cfg );
};

// --

void myGUI_AddKeyMessage( struct Config *cfg, struct CommandKey *msg );
void myGUI_EncodingMessage( struct Config *cfg, struct CommandEncoding *msg );
void myGUI_PxlFmtRefresh( struct Config *cfg );

// --

#endif // __RVNCD_GUI_H__
