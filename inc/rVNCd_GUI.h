
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_GUI_H__
#define __INC__RVNCD_GUI_H__

// --

struct GUIFunctions
{
	// GUI Init .. called on startup
	S32		(*GUI_Init)(struct Config *);

	// GUI Free .. called on shutdown
	void	(*GUI_Free)(struct Config *);

	// Handle GUI input
	void	(*GUI_Handle)(struct Config *);

	// GUI (Signal) Mask, what Signal to react too
	U32		gui_SignalBits;

	// 
	void	(*GUI_SessionMessage)( struct Config *cfg, struct CommandSession *msg );

	// 
	void	(*GUI_RefreshMessage)( struct Config *cfg, struct CommandRefresh *msg );

	// 
	void	(*GUI_RefreshStats)( struct Config *cfg, struct CommandRefresh *msg );

	//
	void	(*GUI_PxlFmtMessage)( struct Config *cfg, struct CommandPxlFmt *msg );

	// ---
	// Called when Server, Start/Stops
	void	(*Server_Status_Change)(struct Config *,enum ProcessStat );

	// Set Default Config values.. like Windows functions, open status
	void	(*Set_Default_Config)(struct Config *);

	// Refresh (Visible) Gadgets
	void	(*Refresh_Settings)(struct Config *);

	// Check if Setting have changed, since Server started
	#ifdef DEBUG
	S32		(*Check_Settings)(struct Config *, STR file, U32 line );
	#else
	S32		(*Check_Settings)(struct Config * );
	#endif

	// Log : Add Message ( update gui display ? )
	void	(*Log_AddMsg)(struct Config *cfg,struct CommandLogString *msg);
};



// --

#endif // __INC__RVNCD_GUI_H__
