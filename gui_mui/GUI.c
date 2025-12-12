
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

struct Library *			MUIMasterBase		= NULL;

#ifdef _AOS4_
struct MUIMasterIFace * 	IMUIMaster			= NULL;
#endif

Object *App;

// --

extern struct MUI_CustomClass *AppClass;

S32		AppClass_Init( struct Config *cfg );
void	AppClass_Free( struct Config *cfg );
S32		GUIFunc_Init(					struct Config *cfg );
void	GUIFunc_Free(					struct Config *cfg );
void	GUIFunc_Handle(					struct Config *cfg );
void	GUIFunc_Default_Config(			struct Config *cfg );

struct GUIFunctions ActiveGUI =
{
	.GUI_Init				= GUIFunc_Init,
	.GUI_Free				= GUIFunc_Free,
	.GUI_Handle				= GUIFunc_Handle,
	.Set_Default_Config		= GUIFunc_Default_Config,
};

// --

S32 GUIFunc_Init( struct Config *cfg UNUSED )
{
S32 retval;

	retval = FALSE;

	// --

	#ifdef _AOS3_

	MUIMasterBase = OpenLibrary( "muimaster.library", 0 );

	if ( ! MUIMasterBase )
	{
		SHELLBUF_PRINTF( "Error opening mui master\n" );
		goto bailout;
	}

	#endif

	#ifdef _AOS4_

	MUIMasterBase = OpenLibrary( "muimaster.library", 0 );
	IMUIMaster = (PTR) GetInterface( MUIMasterBase, "main", 1, NULL );

	if ( ! IMUIMaster )
	{
		SHELLBUF_PRINTF( "Error opening mui master\n" );
		goto bailout;
	}

	#endif

	// --

SHELLBUF_PRINTF( "app 11\n" ); fflush( stdout );

	if ( ! AppClass_Init( cfg ))
	{
		goto bailout;
	}

	// --

SHELLBUF_PRINTF( "app 12\n" ); fflush( stdout );

//	App = NewObject( AppClass->mcc_Class, NULL,
	App = MUI_NewObject( MUIC_Application,
		MUIA_Application_Author,		"Rene W. Olsen",
		MUIA_Application_Base,			"rVNCd",
		MUIA_Application_Copyright,		"© 2025 Rene W. Olsen",
		MUIA_Application_Description,	"Amiga VNC Server",
		MUIA_Application_Title,			"rVNCd",
		MUIA_Application_Version,		& VERSTAG[7],
		TAG_END
	);

	if ( ! App )
	{
		goto bailout;
	}

SHELLBUF_PRINTF( "app 13\n" ); fflush( stdout );

	DoMethod( App, MUIM_Application_NewInput, & ActiveGUI.gui_SignalBits );

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --

void GUIFunc_Free( struct Config *cfg UNUSED )
{
	ActiveGUI.gui_SignalBits = 0;

//	if ( App )
//	{
//		MUI_DisposeObject( App );
//		App = NULL;
//	}

	GUIFunc_Free( cfg );

	#ifdef _AOS4_

	if ( IMUIMaster )
	{
		DropInterface( (PTR) IMUIMaster );
		IMUIMaster = NULL;
	}

	#endif

	if ( MUIMasterBase )
	{
		CloseLibrary( MUIMasterBase );
		MUIMasterBase = NULL;
	}
}

// --

void GUIFunc_Handle( struct Config *cfg UNUSED )
{
	if ( App )
	{
		DoMethod( App, MUIM_Application_NewInput, & ActiveGUI.gui_SignalBits );
	}
}

// --

void myGUI_AddKeyMessage( struct Config *cfg UNUSED, struct CommandKey *msg UNUSED )
{

}

// --

void myGUI_EncodingMessage( struct Config *cfg UNUSED, struct CommandEncoding *msg UNUSED )
{

}

// --

void myGUI_PxlFmtRefresh( struct Config *cfg UNUSED )
{

}

// --
// Set Default Config

void GUIFunc_Default_Config( struct Config *cfg )
{
	cfg->MainWinState = WINSTAT_Open;
	cfg->cfg_WinData[WIN_Main].Status = WINSTAT_Open;
//	cfg->cfg_WinData[WIN_Main].BusyWin = MUIGUI_BusyMainWindow;
	cfg->cfg_WinData[WIN_Main].OpenWin = MUIGUI_OpenMainWindow;
	cfg->cfg_WinData[WIN_Main].CloseWin = MUIGUI_CloseMainWindow;
//	cfg->cfg_WinData[WIN_Main].HandleWin = MUIGUI_HandleMainWindow;

	cfg->cfg_WinData[WIN_About].Status = WINSTAT_Open;
//	cfg->cfg_WinData[WIN_About].BusyWin = MUIGUI_BusyAboutWindow;
	cfg->cfg_WinData[WIN_About].OpenWin = MUIGUI_OpenAboutWindow;
	cfg->cfg_WinData[WIN_About].CloseWin = MUIGUI_CloseAboutWindow;
//	cfg->cfg_WinData[WIN_About].HandleWin = MUIGUI_HandleAboutWindow;

	cfg->cfg_WinData[WIN_Encodings].Status = WINSTAT_Close;
//	cfg->cfg_WinData[WIN_Encodings].BusyWin = MUIGUI_BusyEncodingsWindow;
	cfg->cfg_WinData[WIN_Encodings].OpenWin = MUIGUI_OpenEncodingsWindow;
	cfg->cfg_WinData[WIN_Encodings].CloseWin = MUIGUI_CloseEncodingsWindow;
//	cfg->cfg_WinData[WIN_Encodings].HandleWin = MUIGUI_HandleEncodingsWindow;

	cfg->cfg_WinData[WIN_KeyLogger].Status = WINSTAT_Close;
//	cfg->cfg_WinData[WIN_KeyLogger].BusyWin = MUIGUI_BusyKeyWindow;
	cfg->cfg_WinData[WIN_KeyLogger].OpenWin = MUIGUI_OpenKeyWindow;
	cfg->cfg_WinData[WIN_KeyLogger].CloseWin = MUIGUI_CloseKeyWindow;
//	cfg->cfg_WinData[WIN_KeyLogger].HandleWin = MUIGUI_HandleKeyWindow;

	cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Close;
//	cfg->cfg_WinData[WIN_Session].BusyWin = MUIGUI_BusySessionWindow;
	cfg->cfg_WinData[WIN_Session].OpenWin = MUIGUI_OpenSessionWindow;
	cfg->cfg_WinData[WIN_Session].CloseWin = MUIGUI_CloseSessionWindow;
//	cfg->cfg_WinData[WIN_Session].HandleWin = MUIGUI_HandleSessionWindow;

	cfg->cfg_WinData[WIN_PixelFormat].Status = WINSTAT_Close;
//	cfg->cfg_WinData[WIN_PixelFormat].BusyWin = MUIGUI_BusyPixelWindow;
	cfg->cfg_WinData[WIN_PixelFormat].OpenWin = MUIGUI_OpenPixelWindow;
	cfg->cfg_WinData[WIN_PixelFormat].CloseWin = MUIGUI_ClosePixelWindow;
//	cfg->cfg_WinData[WIN_PixelFormat].HandleWin = MUIGUI_HandlePixelWindow;

	cfg->cfg_WinData[WIN_IPEdit].Status = WINSTAT_Close;
//	cfg->cfg_WinData[WIN_IPEdit].BusyWin = MUIGUI_BusyIPEditWindow;
	cfg->cfg_WinData[WIN_IPEdit].OpenWin = MUIGUI_OpenIPEditWindow;
	cfg->cfg_WinData[WIN_IPEdit].CloseWin = MUIGUI_CloseIPEditWindow;
//	cfg->cfg_WinData[WIN_IPEdit].HandleWin = MUIGUI_HandleIPEditWindow;

	cfg->cfg_WinData[WIN_Quit].Status = WINSTAT_Close;
//	cfg->cfg_WinData[WIN_Quit].BusyWin = MUIGUI_BusyQuitWindow;
	cfg->cfg_WinData[WIN_Quit].OpenWin = MUIGUI_OpenQuitWindow;
	cfg->cfg_WinData[WIN_Quit].CloseWin = MUIGUI_CloseQuitWindow;
//	cfg->cfg_WinData[WIN_Quit].HandleWin = MUIGUI_HandleQuitWindow;
}

// --

#endif // __HAVE__RVNCD__GUI__
