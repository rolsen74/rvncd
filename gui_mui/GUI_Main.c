
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

Object *Main_Win;
static Object *StatusList;
static Object *PageList;
static Object *PagePage;
static Object *StatList;
Object *btn_Server_Off;
Object *btn_Server_On;

// --

static const STR NeededLibs[] =
{
	"NList.mcc",
	NULL
};

static const STR PageTitles[] =
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
	"IP List",
	NULL
};

// --

Object *GUI_Main_01_Main_Page( struct Config *cfg );
Object *GUI_Main_02_Program_Page( struct Config *cfg );
Object *GUI_Main_03_Action_Page( struct Config *cfg );
Object *GUI_Main_04_Screen_Page( struct Config *cfg );
Object *GUI_Main_05_Server_Page( struct Config *cfg );
Object *GUI_Main_06_ServerStat_Page( struct Config *cfg );
Object *GUI_Main_07_Log_Page( struct Config *cfg );
Object *GUI_Main_08_Mouse_Page( struct Config *cfg );
Object *GUI_Main_09_Protocol_Page( struct Config *cfg );
Object *GUI_Main_10_IPs_Page( struct Config *cfg );

// --

S32 MUIGUI_OpenMainWindow( struct Config *cfg )
{
S32 error;

	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "MUIGUI_OpenMainWindow\n" );
	}

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

			MUIA_Group_Child,				MUI_NewObject( MUIC_Group,
				MUIA_Group_Horiz,			TRUE,

				MUIA_Group_Child,			PageList = MUI_NewObject( MUIC_NList,
					MUIA_NList_SourceArray,	PageTitles,
					MUIA_Weight,			150,
				End,

				MUIA_Group_Child,			MUI_NewObject( MUIC_Balance,
				End,

				MUIA_Group_Child,			PagePage = MUI_NewObject( MUIC_Group,
					MUIA_Group_PageMode,	TRUE,
					MUIA_Frame,				MUIV_Frame_Group,
					MUIA_Background,		MUII_BACKGROUND,
					MUIA_Weight,			850,

					MUIA_Group_Child,		GUI_Main_01_Main_Page(cfg),
					MUIA_Group_Child,		GUI_Main_02_Program_Page(cfg),
					MUIA_Group_Child,		GUI_Main_03_Action_Page(cfg),
					MUIA_Group_Child,		GUI_Main_04_Screen_Page(cfg),
					MUIA_Group_Child,		GUI_Main_05_Server_Page(cfg),
					MUIA_Group_Child,		GUI_Main_06_ServerStat_Page(cfg),
					MUIA_Group_Child,		GUI_Main_07_Log_Page(cfg),
					MUIA_Group_Child,		GUI_Main_08_Mouse_Page(cfg),
					MUIA_Group_Child,		GUI_Main_09_Protocol_Page(cfg),
					MUIA_Group_Child,		GUI_Main_10_IPs_Page(cfg),
				End,
			End,

			MUIA_Group_Child,				MUI_NewObject( MUIC_Balance,
			End,

			MUIA_Group_Child,				StatusList = MUI_NewObject( MUIC_NList,
//				MUIA_NList_SourceArray,		PageTitles,
				MUIA_Weight,				0,
			End,

			MUIA_Group_Child,				MUI_NewObject( MUIC_Group,
				MUIA_Group_Horiz,			TRUE,

				MUIA_Group_Child,			btn_Server_On = MUI_NewObject( MUIC_Text,
					MUIA_Frame,				MUIV_Frame_Button,
					MUIA_Text_Contents,		"Online",
					MUIA_Text_PreParse,		"\33c",
//					MUIA_Text_HiChar,		key,
//					MUIA_ControlChar,		key,
					MUIA_InputMode,			MUIV_InputMode_RelVerify,
					MUIA_Background,		MUII_ButtonBack,
					MUIA_Disabled,			( cfg->cfg_ServerStatus == PROCESS_Stopped ) ? FALSE : TRUE,
				End,

				MUIA_Group_Child,			MUI_NewObject( MUIC_Rectangle,
				End,

				MUIA_Group_Child,			btn_Server_Off = MUI_NewObject( MUIC_Text,
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
		PTR base;

		// -- Check for library, and tell user if missing
		for( U32 cnt=0 ; NeededLibs[cnt] ; cnt++ )
		{
			if (( base = OpenLibrary( NeededLibs[cnt], 0 )))
			{
				CloseLibrary( base );
			}
			else
			{
				SHELLBUF_PRINTF1( "Missing '%s'\n", NeededLibs[cnt] );
			}
		}

		// --

		goto bailout;
	}

//	DoMethod( PageList, MUIM_NList_Insert, PageTitles, -1, MUIV_NList_Insert_Bottom, 0 );
	DoMethod( App, OM_ADDMEMBER, Main_Win );

	DoMethod( PageList, MUIM_Notify, MUIA_NList_Active, MUIV_EveryTime, 
		PagePage, 3, MUIM_NoNotifySet, MUIA_Group_ActivePage, MUIV_TriggerValue );

	DoMethod( Main_Win, MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
		App, 2, MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit );

	DoMethod( btn_Server_On, MUIM_Notify, MUIA_Pressed, FALSE, 
		App, 2, MUIM_Application_ReturnID, GID_StartServer );

	DoMethod( btn_Server_Off, MUIM_Notify, MUIA_Pressed, FALSE, 
		App, 2, MUIM_Application_ReturnID, GID_StopServer );

	set( Main_Win, MUIA_Window_Open, TRUE );

	error = FALSE;

bailout:

	return( error );
}

// --

void MUIGUI_CloseMainWindow( struct Config *cfg UNUSED )
{
	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "MUIGUI_CloseMainWindow\n" );
	}

	// --
}

// --
