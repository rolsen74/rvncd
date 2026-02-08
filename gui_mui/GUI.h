
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

// Make sure only One GUI is included, so reuse name
#ifndef __RVNCD_GUI_H__
#define __RVNCD_GUI_H__

// --

#include <proto/gadtools.h>
#include <proto/muimaster.h>

#include <libraries/mui.h>

#include <mui/NListview_mcc.h>

// --

extern Object *App;
extern Object *Main_Win;
extern Object *btn_Server_Off;
extern Object *btn_Server_On;

// --

enum GUI_ID
{
	GUI_Dummy,
	// GID_Window,
	// GID_Root,
	GID_StartServer,
	GID_StopServer,
	// GID_Status,
	// GID_LVSelect,
	// GID_LVClickTab,
	// GID_LVPage,
	// GID_NeedTab,
	// GID_NeedPage,

	// GID_Support_RichCursor,
	// GID_Support_RRE,
	// GID_Support_ZLib,
	// GID_Disable_Blanker,
	// GID_Send_Clipboard,
	// GID_Read_Clipboard,
	// GID_Send_Bell,
	// GID_Port,
	// GID_ServerName,
	// GID_Password,
	// GID_TileBufferKB,

	// GID_Main_UserConn,

	// GID_Program_Verbose,
	// GID_Program_AutoStart,
	// GID_Program_Disable_ARexx,
	// GID_Program_Disable_CxBroker,
	// GID_Program_Disable_GUI,
	// GID_Program_Open_GUI,
	// GID_Program_WinStat,

	// GID_Log_Program_Start,
	// GID_Log_Program_Stop,
	// GID_Log_Server_Start,
	// GID_Log_Server_Stop,
	// GID_Log_User_Connect,
	// GID_Log_User_Disconnect,
	// GID_Log_Infos,
	// GID_Log_Warnings,
	// GID_Log_Errors,

	// GID_IPAdd,
	// GID_IPList,
	// GID_IPDelete,
	// GID_IPEdit,
	// GID_IPDown,
	// GID_IPUp,

	// GID_Screen_TileSize,
	// GID_Screen_DelaySlider,

	// GID_Actions_Program_Start_Type,
	// GID_Actions_Program_Start_Enable,
	// GID_Actions_Program_Start_String,
	// GID_Actions_Program_Stop_Type,
	// GID_Actions_Program_Stop_Enable,
	// GID_Actions_Program_Stop_String,
	// GID_Actions_Server_Start_Type,
	// GID_Actions_Server_Start_Enable,
	// GID_Actions_Server_Start_String,
	// GID_Actions_Server_Stop_Type,
	// GID_Actions_Server_Stop_Enable,
	// GID_Actions_Server_Stop_String,
	// GID_Actions_User_Connect_Type,
	// GID_Actions_User_Connect_Enable,
	// GID_Actions_User_Connect_String,
	// GID_Actions_User_Disconnect_Type,
	// GID_Actions_User_Disconnect_Enable,
	// GID_Actions_User_Disconnect_String,

	// GID_Stats_Accecpted,
	// GID_Stats_Rejected,
	// GID_Stats_Allowed,
	// GID_Stats_Blocked,
	// GID_Stats_Connect,
	// GID_Stats_Duration,

	// GID_Mouse_Type,
	// GID_Mouse_Filename,

	// GID_Protocol_33_Enable,
	// GID_Protocol_37_Enable,

	GID_LAST
};

// --
// MUI have those Windows

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

//void			MUIGUI_BusyAboutWindow(			struct Config *cfg, S32 val );
S32				MUIGUI_OpenAboutWindow(			struct Config *cfg );
void			MUIGUI_CloseAboutWindow(		struct Config *cfg );
//void			MUIGUI_HandleAboutWindow(		struct Config *cfg );

//void			MUIGUI_BusyEncodingsWindow(		struct Config *cfg, S32 val );
S32				MUIGUI_OpenEncodingsWindow(		struct Config *cfg );
void			MUIGUI_CloseEncodingsWindow(	struct Config *cfg );
//void			MUIGUI_HandleEncodingsWindow(	struct Config *cfg );
void			myGUI_EncodingMessage(			struct Config *cfg, struct CommandEncoding *msg );

//void			MUIGUI_BusyIPEditWindow(		struct Config *cfg, S32 val );
S32				MUIGUI_OpenIPEditWindow(		struct Config *cfg );
void			MUIGUI_CloseIPEditWindow(		struct Config *cfg );
//void			MUIGUI_HandleIPEditWindow(		struct Config *cfg );

//void			MUIGUI_BusyKeyWindow(			struct Config *cfg, S32 val );
S32				MUIGUI_OpenKeyWindow(			struct Config *cfg );
void			MUIGUI_CloseKeyWindow(			struct Config *cfg );
//void			MUIGUI_HandleKeyWindow(			struct Config *cfg );
void			myGUI_AddKeyMessage(			struct Config *cfg, struct CommandKey *msg );

//void			MUIGUI_BusyMainWindow(			struct Config *cfg, S32 val );
S32				MUIGUI_OpenMainWindow(			struct Config *cfg );
void			MUIGUI_CloseMainWindow(			struct Config *cfg );
//void			MUIGUI_HandleMainWindow(		struct Config *cfg );
//void			myGUI_Main_SessionMessage(		struct Config *cfg, struct CommandSession *msg );

//void			MUIGUI_BusyPixelWindow(			struct Config *cfg, S32 val );
S32				MUIGUI_OpenPixelWindow(			struct Config *cfg );
void			MUIGUI_ClosePixelWindow(		struct Config *cfg );
//void			MUIGUI_HandlePixelWindow(		struct Config *cfg );
//void			myGUI_PxlFmtMessage(			struct Config *cfg, struct CommandPxlFmt *msg );
//void			myGUI_PxlFmtRefresh(			struct Config *cfg );

//void			MUIGUI_BusyQuitWindow(			struct Config *cfg, S32 val );
S32				MUIGUI_OpenQuitWindow(			struct Config *cfg );
void			MUIGUI_CloseQuitWindow(			struct Config *cfg );
//void			MUIGUI_HandleQuitWindow(		struct Config *cfg );

//void			MUIGUI_BusySessionWindow(		struct Config *cfg, S32 val );
S32				MUIGUI_OpenSessionWindow(		struct Config *cfg );
void			MUIGUI_CloseSessionWindow(		struct Config *cfg );
//void			MUIGUI_HandleSessionWindow(		struct Config *cfg );
//void			myGUI_SessionMessage(			struct Config *cfg, struct CommandSession *msg );

// --

#endif // __RVNCD_GUI_H__
