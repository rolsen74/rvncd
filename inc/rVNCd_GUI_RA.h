
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifdef GUI_RA

// Make sure only One GUI is included, so reuse name
#ifndef __RVNCD_GUI_H__
#define __RVNCD_GUI_H__

// --

#include <proto/bitmap.h>
#include <proto/button.h>
#include <proto/checkbox.h>
#include <proto/chooser.h>
#include <proto/clicktab.h>
#include <proto/getfile.h>
#include <proto/integer.h>
#include <proto/label.h>
#include <proto/layout.h>
#include <proto/listbrowser.h>
#include <proto/scroller.h>
#include <proto/slider.h>
#include <proto/string.h>
#include <proto/window.h>

#include <classes/window.h>
#include <gadgets/button.h>
#include <gadgets/checkbox.h>
#include <gadgets/chooser.h>
#include <gadgets/clicktab.h>
#include <gadgets/getfile.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <gadgets/integer.h>
#include <gadgets/scroller.h>
#include <gadgets/slider.h>
#include <gadgets/string.h>
#include <graphics/gfx.h>
#include <images/bitmap.h>
#include <images/label.h>
#include <intuition/icclass.h>
#include <intuition/menuclass.h>
#include <reaction/reaction_macros.h>

// --

// --
// Reaction have those Windows

//enum
//{
//	WIN_Main,		// Should be first
//	WIN_About,
//	WIN_Session,
//	WIN_KeyLogger,
//	WIN_Encodings,
//	WIN_PixelFormat,
//	WIN_IPEdit,
//	WIN_Quit,
//	WIN_LAST
//};

#define WIN_Main			0	// Should be first
#define WIN_About			1
#define WIN_Session			2
#define WIN_KeyLogger		3
#define WIN_Encodings		4
#define WIN_PixelFormat		5
#define WIN_IPEdit			6
#define WIN_Quit			7
#define WIN_LAST			8

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

extern struct DiskObject *ProgramIcon;
extern struct VNCMsgPort WinAppPort;
extern struct VNCMsgPort WinMsgPort;
//extern Class *ListBrowserClass;
//extern Class *ClickTabClass;
//extern Class *IntegerClass;
//extern Class *WindowClass;
//extern Class *LayoutClass;
//extern Class *ButtonClass;
//extern Class *SttingClass;
//extern Class *BitMapClass;
extern Class *PageClass;

// --



// --

#endif // GUI_RA
#endif // __RVNCD_GUI_H__
