
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

// Make sure only One GUI is included, so reuse name
#ifndef __HAVE__RVNCD__GUI__
#define __HAVE__RVNCD__GUI__

#ifdef GUI_RA

// --

struct VNCMsgPort			WinAppPort;
struct VNCMsgPort			WinMsgPort;

struct LayoutIFace * 		ILayout				= NULL;
struct ListBrowserIFace * 	IListBrowser		= NULL;

Class *						ButtonClass			= NULL;
Class *						BitMapClass			= NULL;
Class *						CheckBoxClass		= NULL;
Class *						ChooserClass		= NULL;
Class *						ClickTabClass		= NULL;
Class *						GetFileClass		= NULL;
Class *						IntegerClass		= NULL;
Class *						LabelClass			= NULL;
Class *						LayoutClass			= NULL;
Class *						ListBrowserClass	= NULL;
Class *						PageClass			= NULL;
Class *						StringClass			= NULL;
Class *						SliderClass			= NULL;
Class *						ScrollerClass		= NULL;
Class *						WindowClass			= NULL;

struct Library *			ButtonBase			= NULL;
struct Library *			BitMapBase			= NULL;
struct Library *			CheckBoxBase		= NULL;
struct Library *			ChooserBase			= NULL;
struct Library *			ClickTabBase		= NULL;
struct Library *			GetFileBase			= NULL;
struct Library *			IntegerBase			= NULL;
struct Library *			LabelBase			= NULL;
struct Library *			LayoutBase			= NULL;
struct Library *			ListBrowserBase		= NULL;
struct Library *			ScrollerBase		= NULL;
struct Library *			SliderBase			= NULL;
struct Library *			StringBase			= NULL;
struct Library *			WindowBase			= NULL;


#ifdef DEBUG
void 	GUIFunc_UpdateServerStatus(		struct Config *cfg, enum ProcessStat pstat, STR file, U32 line );
S32		GUIFunc_CheckSettings(			struct Config *cfg, STR file, U32 line );
#else
void 	GUIFunc_UpdateServerStatus(		struct Config *cfg, enum ProcessStat pstat );
S32		GUIFunc_CheckSettings(			struct Config *cfg );
#endif
void 	GUIFunc_Default_Config(			struct Config *cfg );
void	GUIFunc_RefreshSettings(		struct Config *cfg );
S32		GUIFunc_Init(					struct Config *cfg );
void	GUIFunc_Free(					struct Config *cfg );
void	GUIFunc_Handle(					struct Config *cfg );
void	GUIFunc_AddLogMsg(				struct Config *cfg, struct CommandLogString *msg );
void	GUIFunc_SessionMessage(			struct Config *cfg, struct CommandSession *msg );
void	GUIFunc_RefreshMessage(			struct Config *cfg, struct CommandRefresh *msg );
void	GUIFunc_RefreshStats(			struct Config *cfg, struct CommandRefresh *msg );
void	GUIFunc_PxlFmtMessage(			struct Config *cfg, struct CommandPxlFmt *msg );

struct GUIFunctions ActiveGUI =
{
	.Server_Status_Change	= GUIFunc_UpdateServerStatus,
	.Set_Default_Config		= GUIFunc_Default_Config,
	.Refresh_Settings		= GUIFunc_RefreshSettings,
	.Check_Settings			= GUIFunc_CheckSettings,
	.GUI_Init				= GUIFunc_Init,
	.GUI_Free				= GUIFunc_Free,
	.GUI_Handle				= GUIFunc_Handle,
	.Log_AddMsg				= GUIFunc_AddLogMsg,
	.GUI_SessionMessage		= GUIFunc_SessionMessage,
	.GUI_RefreshMessage		= GUIFunc_RefreshMessage,
	.GUI_RefreshStats		= GUIFunc_RefreshStats,
	.GUI_PxlFmtMessage		= GUIFunc_PxlFmtMessage,
};

// --

#endif // GUI_RA
#endif // __HAVE__RVNCD__GUI__
