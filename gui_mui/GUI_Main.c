
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#ifdef GUI_MUI

// --

static Object *Main_Win;
static Object *PageList;
static Object *PagePage;
static Object *StatList;
static Object *btn_off;
static Object *btn_on;

// --

static STR PageTitles[] =
{
	"Main",
	"Program",
	"Actions",
	"Screen",
	"Server",
	"Server Stats",
	"Log",
	"Mouse",
	"Protocols",
	"Black List",
	"White List",
	NULL
};

// --

Object *GUI_Main_Action_Page( struct Config *cfg );
Object *GUI_Main_BlackList_Page( struct Config *cfg );
Object *GUI_Main_Log_Page( struct Config *cfg );
Object *GUI_Main_Main_Page( struct Config *cfg );
Object *GUI_Main_Mouse_Page( struct Config *cfg );
Object *GUI_Main_Program_Page( struct Config *cfg );
Object *GUI_Main_Protocol_Page( struct Config *cfg );
Object *GUI_Main_Screen_Page( struct Config *cfg );
Object *GUI_Main_Server_Page( struct Config *cfg );
Object *GUI_Main_ServerStat_Page( struct Config *cfg );
Object *GUI_Main_WhiteList_Page( struct Config *cfg );

S32 MUIGUI_OpenMainWindow( struct Config *cfg UNUSED )
{
S32 error;

	// --

	error = FALSE;

	if ( Main_Win )
	{
		set( Main_Win, MUIA_Window_Open, TRUE );
		error = TRUE;
		goto bailout;
	}

	Main_Win = MUI_NewObject( MUIC_Window,
		MUIA_Window_Title,					& VERSTAG[7],
		MUIA_Window_ID,						MAKE_ID('W','I','N','A'),
		MUIA_Window_Width,					800,
		MUIA_Window_Height,					600,

		MUIA_Window_RootObject,				MUI_NewObject( MUIC_Group,
			MUIA_Group_Horiz,				FALSE,

			#if 0
			MUIA_Group_Child,				MUI_NewObject( MUIC_Group,
				MUIA_Group_Horiz,			TRUE,
//				MUIA_Weight,				900,

				MUIA_Group_Child,			PageList = MUI_NewObject( MUIC_NList,
//					MUIA_Weight,			150,
				End,

				MUIA_Group_Child,			MUI_NewObject( MUIC_Balance,
				End,

				MUIA_Group_Child,			PagePage = MUI_NewObject( MUIC_Group,
					MUIA_Group_PageMode,	TRUE,
					MUIA_Frame,				MUIV_Frame_Group,
					MUIA_Background,		MUII_BACKGROUND,
//					MUIA_Weight,			850,

					MUIA_Group_Child,		GUI_Main_Main_Page(cfg),
					MUIA_Group_Child,		GUI_Main_Program_Page(cfg),
					MUIA_Group_Child,		GUI_Main_Action_Page(cfg),
					MUIA_Group_Child,		GUI_Main_Screen_Page(cfg),
					MUIA_Group_Child,		GUI_Main_Server_Page(cfg),
					MUIA_Group_Child,		GUI_Main_ServerStat_Page(cfg),
					MUIA_Group_Child,		GUI_Main_Log_Page(cfg),
					MUIA_Group_Child,		GUI_Main_Mouse_Page(cfg),
					MUIA_Group_Child,		GUI_Main_Protocol_Page(cfg),
					MUIA_Group_Child,		GUI_Main_BlackList_Page(cfg),
					MUIA_Group_Child,		GUI_Main_WhiteList_Page(cfg),
				End,
			End,

			MUIA_Group_Child,				MUI_NewObject( MUIC_Balance,
			End,
			#endif

//			MUIA_Group_Child,				StatList = MUI_NewObject( MUIC_NList,
			MUIA_Group_Child,				StatList = MUI_NewObject( MUIC_Group,
//				MUIA_Weight,				100,
			End,

			MUIA_Group_Child,				MUI_NewObject( MUIC_Group,
				MUIA_Group_Horiz,			TRUE,

//				MUIA_Group_Child,			SimpleButton( "Online" ),
				MUIA_Group_Child,			btn_on = MUI_NewObject( MUIC_Text,
					MUIA_Frame,				MUIV_Frame_Button,
					MUIA_Text_Contents,		"Online",
					MUIA_Text_PreParse,		"\33c",
//					MUIA_Text_HiChar,		key,
//					MUIA_ControlChar,		key,
					MUIA_InputMode,			MUIV_InputMode_RelVerify,
					MUIA_Background,		MUII_ButtonBack,
					MUIA_Disabled,			( cfg->cfg_ServerStatus == PROCESS_Stopped ) ? FALSE : TRUE,
				End,

				MUIA_Group_Child,			MUI_NewObject( MUIC_Group,
				End,

//				MUIA_Group_Child,			SimpleButton( "Offline" ),
				MUIA_Group_Child,			btn_off = MUI_NewObject( MUIC_Text,
					MUIA_Frame,				MUIV_Frame_Button,
					MUIA_Text_Contents,		"Offline",
					MUIA_Text_PreParse,		"\33c",
//					MUIA_Text_HiChar,		key,
//					MUIA_ControlChar,		key,
					MUIA_InputMode,			MUIV_InputMode_RelVerify,
					MUIA_Background,		MUII_ButtonBack,
					MUIA_Disabled,			( cfg->cfg_ServerStatus == PROCESS_Running ) ? FALSE : TRUE,
				End,
			End,
		End,
	End;

	if ( ! Main_Win )
	{
		SHELLBUF_PRINTF( "Error creating Main_Win object\n" );
		goto bailout;
	}

	DoMethod( PageList, MUIM_NList_Insert, PageTitles, -1, MUIV_NList_Insert_Bottom, 0 );
	DoMethod( App, OM_ADDMEMBER, Main_Win );

	DoMethod( PageList, MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime, 
		PagePage, 3, MUIM_NoNotifySet, MUIA_Group_ActivePage, MUIV_TriggerValue );

//	DoMethod( tmp.BT_Jump   ,MUIM_Notify,MUIA_Pressed,FALSE,obj,1,MUIM_ScreenPanel_Jump  );

	set( Main_Win, MUIA_Window_Open, TRUE );

	error = FALSE;

bailout:

	return( error );
}

// --

void MUIGUI_CloseMainWindow( struct Config *cfg UNUSED )
{
}

// --

#endif // GUI_MUI
