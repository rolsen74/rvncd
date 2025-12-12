
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

#ifdef GUI_RA

#ifndef WIN_H
#define WIN_H

// --

enum GUI_ID
{
	GID_Window,
	GID_Root,
	GID_StartServer,
	GID_StopServer,
	GID_Status,
	GID_LVSelect,
	GID_LVClickTab,
	GID_LVPage,
	GID_NeedTab,
	GID_NeedPage,

	GID_Support_RichCursor,
	GID_Support_RRE,
	GID_Support_ZLib,
	GID_Disable_Blanker,
	GID_Send_Clipboard,
	GID_Read_Clipboard,
	GID_Send_Bell,
	GID_Port,
	GID_ServerName,
	GID_Password,
	GID_TileBufferKB,

	GID_Main_UserConn,

	GID_Program_Verbose,
	GID_Program_AutoStart,
	GID_Program_Disable_ARexx,
	GID_Program_Disable_CxBroker,
	GID_Program_Disable_GUI,
	GID_Program_Open_GUI,
	GID_Program_WinStat,

	GID_Log_Program_Start,
	GID_Log_Program_Stop,
	GID_Log_Server_Start,
	GID_Log_Server_Stop,
	GID_Log_User_Connect,
	GID_Log_User_Disconnect,
	GID_Log_Infos,
	GID_Log_Warnings,
	GID_Log_Errors,

	GID_IPAdd,
	GID_IPList,
	GID_IPDelete,
	GID_IPEdit,
	GID_IPDown,
	GID_IPUp,

	GID_Screen_TileSize,
	GID_Screen_DelaySlider,

	GID_Actions_Program_Start_Type,
	GID_Actions_Program_Start_Enable,
	GID_Actions_Program_Start_String,
	GID_Actions_Program_Stop_Type,
	GID_Actions_Program_Stop_Enable,
	GID_Actions_Program_Stop_String,
	GID_Actions_Server_Start_Type,
	GID_Actions_Server_Start_Enable,
	GID_Actions_Server_Start_String,
	GID_Actions_Server_Stop_Type,
	GID_Actions_Server_Stop_Enable,
	GID_Actions_Server_Stop_String,
	GID_Actions_User_Connect_Type,
	GID_Actions_User_Connect_Enable,
	GID_Actions_User_Connect_String,
	GID_Actions_User_Disconnect_Type,
	GID_Actions_User_Disconnect_Enable,
	GID_Actions_User_Disconnect_String,

	GID_Stats_Accecpted,
	GID_Stats_Rejected,
	GID_Stats_Allowed,
	GID_Stats_Blocked,
	GID_Stats_Connect,
	GID_Stats_Duration,

	GID_Mouse_Type,
	GID_Mouse_Filename,

	GID_Protocol_33_Enable,
	GID_Protocol_37_Enable,

	GID_LAST
};

enum
{
	MENUID_Dummy, // Zero is not a valid id

	MENUID_PixelFormat,
	MENUID_KeyLogger,
	MENUID_Encodings,
	MENUID_Session,
	MENUID_About,

	MENUID_User1,
	MENUID_User1_Kick,

	MENUID_Quit,
	MENUID_Load,
	MENUID_Save,
	MENUID_SaveAS,
	MENUID_SaveDefault,
	MENUID_Reset,
	MENUID_Last
};

// --

extern struct FileRequester *AslHandle;
extern struct VNCList MainIPList;
extern const STR WinVerboseStrings[];
extern const STR WinStatStrings[];
extern const STR ActionTypes[];
extern char WorkBuffer[MAX_ACTIONBUFFER];
extern PTR MainMenuStrip;

// --

void VARARGS68K Main_SetTags( struct Config *cfg, PTR object, ... );
#define mySetTags Main_SetTags

#define GUIObjects Main_GUIObjects
extern Object *Main_GUIObjects[GID_LAST];

// --

Object *Page_Actions( struct Config *cfg );
Object *Page_Action_Programs( struct Config *cfg );
Object *Page_Action_Server( struct Config *cfg );
Object *Page_Action_User( struct Config *cfg );
Object *Page_Program( struct Config *cfg );
Object *Page_Screen( struct Config *cfg );
Object *Page_Server( struct Config *cfg );
Object *Page_Stats( struct Config *cfg );
Object *Page_Log( struct Config *cfg );
Object *Page_Mouse( struct Config *cfg );
Object *Page_Protocols( struct Config *cfg );
Object *Page_Protocols_v33( struct Config *cfg );
Object *Page_Protocols_v37( struct Config *cfg );
Object *Page_IP_List( struct Config *cfg );

void Page_Refresh_Action_Program( struct Config *cfg );
void Page_Refresh_Action_Server( struct Config *cfg );
void Page_Refresh_Action_User( struct Config *cfg );
void Page_Refresh_Screen( struct Config *cfg );
void Page_Refresh_Stats( struct Config *cfg );
void Page_Refresh_Log( struct Config *cfg );
void Page_Refresh_Protocol_33( struct Config *cfg );
void Page_Refresh_Protocol_37( struct Config *cfg );
void Page_Refresh_IP( struct Config *cfg );
void Page_Refresh_Mouse( struct Config *cfg );

void GUIFunc_Support_Support_RichCursor( struct Config *cfg );
void GUIFunc_Support_Support_RRE( struct Config *cfg );
void GUIFunc_Support_Support_ZLib( struct Config *cfg );
void GUIFunc_Disable_Blanker( struct Config *cfg );
void GUIFunc_Send_Clipboard( struct Config *cfg );
void GUIFunc_Read_Clipboard( struct Config *cfg );
void GUIFunc_Send_Bell( struct Config *cfg );
void GUIFunc_Port( struct Config *cfg );
void GUIFunc_AutoStart( struct Config *cfg );
void GUIFunc_Verbose( struct Config *cfg );
void GUIFunc_ARexx( struct Config *cfg );
void GUIFunc_GUI( struct Config *cfg );
void GUIFunc_ProgramWinStat( struct Config *cfg );
void GUIFunc_ProgramStart( struct Config *cfg );
void GUIFunc_ProgramStop( struct Config *cfg );
void GUIFunc_ServerStart( struct Config *cfg );
void GUIFunc_ServerStop( struct Config *cfg );
void GUIFunc_UserConnect( struct Config *cfg );
void GUIFunc_UserDisconnect( struct Config *cfg );
void GUIFunc_Infos( struct Config *cfg );
void GUIFunc_Warnings( struct Config *cfg );
void GUIFunc_Errors( struct Config *cfg );
void GUIFunc_UpdateServerName( struct Config *cfg );
void GUIFunc_TileBufferKB( struct Config *cfg );
void GUIFunc_UpdatePassword( struct Config *cfg );
void GUIFunc_UpdateActionProgramStart( struct Config *cfg );
void GUIFunc_UpdateActionProgramStop( struct Config *cfg );
void GUIFunc_UpdateActionServerStart( struct Config *cfg );
void GUIFunc_UpdateActionServerStop( struct Config *cfg );
void GUIFunc_UpdateActionUserConnect( struct Config *cfg );
void GUIFunc_UpdateActionUserDisconnect( struct Config *cfg );
void GUIFunc_IPAdd( struct Config *cfg );
void GUIFunc_IPList( struct Config *cfg );
void GUIFunc_IPUp( struct Config *cfg );
void GUIFunc_IPDown( struct Config *cfg );
void GUIFunc_IPDelete( struct Config *cfg );
void GUIFunc_IPEdit( struct Config *cfg );
void GUIFunc_Support_RichCursor( struct Config *cfg );
void GUIFunc_Support_RRE( struct Config *cfg );
void GUIFunc_Support_ZLib( struct Config *cfg );
void GUIFunc_Broker( struct Config *cfg );
void GUIFunc_Screen_TileSize( struct Config *cfg );
void GUIFunc_Screen_Delay( struct Config *cfg );
void GUIFunc_Mouse_Type( struct Config *cfg );
void GUIFunc_Mouse_Filename( struct Config *cfg );
void GUIFunc_Protocol_33_Enable( struct Config *cfg );
void GUIFunc_Protocol_37_Enable( struct Config *cfg );
void GUIFunc_Actions_Program_Start_Enable( struct Config *cfg );
void GUIFunc_Actions_Program_Stop_Enable( struct Config *cfg );
void GUIFunc_Actions_Server_Start_Enable( struct Config *cfg );
void GUIFunc_Actions_Server_Stop_Enable( struct Config *cfg );
void GUIFunc_Actions_User_Connect_Enable( struct Config *cfg );
void GUIFunc_Actions_User_Disconnect_Enable( struct Config *cfg );

S32 myGUI_CreateMenus( struct Config *cfg );
void myGUI_DeleteMenus( struct Config *cfg );
S32 myGUI_MenuPick( struct Config *cfg );

// --

#endif // WIN_H
#endif // GUI_RA

// --
