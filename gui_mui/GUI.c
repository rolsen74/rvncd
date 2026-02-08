
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

struct Library *			MUIMasterBase		= NULL;

#ifdef _AOS4_
struct MUIMasterIFace * 	IMUIMaster			= NULL;
#endif

Object *App;

// --

extern struct MUI_CustomClass *AppClass;

void 	GUIFunc_UpdateServerStatus(		struct Config *cfg, enum ProcessStat pstat );
//S32		GUIFunc_CheckSettings(			struct Config *cfg );

S32		AppClass_Init( struct Config *cfg );
void	AppClass_Free( struct Config *cfg );
S32		GUIFunc_Init(					struct Config *cfg );
void	GUIFunc_Free(					struct Config *cfg );
void	GUIFunc_Handle(					struct Config *cfg );
void	GUIFunc_Default_Config(			struct Config *cfg );

//void	GUIFunc_RefreshSettings(		struct Config *cfg );
//void	GUIFunc_AddLogMsg(				struct Config *cfg, struct CommandLogString *msg );
//void	GUIFunc_SessionMessage(			struct Config *cfg, struct CommandSession *msg );
//void	GUIFunc_RefreshMessage(			struct Config *cfg, struct CommandRefresh *msg );
//void	GUIFunc_RefreshStats(			struct Config *cfg, struct CommandRefresh *msg );
//void	GUIFunc_PxlFmtMessage(			struct Config *cfg, struct CommandPxlFmt *msg );

struct GUIFunctions ActiveGUI =
{
	.Server_Status_Change	= GUIFunc_UpdateServerStatus,
	.GUI_Init				= GUIFunc_Init,
	.GUI_Free				= GUIFunc_Free,
	.GUI_Handle				= GUIFunc_Handle,
	.Set_Default_Config		= GUIFunc_Default_Config,
};

// --

enum 
{
	MENU_FILE=1,
	MENU_OPEN,
	MENU_EXIT,
};

static struct NewMenu MenuStr[]=
{
	{ NM_TITLE,	"File",				 0 , 0, 0,  (PTR) MENU_FILE   },
	{ NM_ITEM,	"About",			"?", 0, 0,  (PTR) MENU_OPEN   },
	{ NM_ITEM,	NM_BARLABEL,		 0 , 0, 0,  (PTR) 0           },
	{ NM_ITEM,	"Quit",				"Q", 0, 0,  (PTR) MENU_EXIT   },

    { NM_TITLE,	"Windows",			 0 , 0, 0,  (PTR) MENU_FILE   },
	{ NM_ITEM,	"Encodings",		 0 , 0, 0,  (PTR) MENU_FILE   },
	{ NM_ITEM,	"Key Logger",		 0 , 0, 0,  (PTR) MENU_FILE   },
	{ NM_ITEM,	"Pixel Format",		 0 , 0, 0,  (PTR) MENU_FILE   },
	{ NM_ITEM,	"Session Info",		 0 , 0, 0,  (PTR) MENU_FILE   },

	{ NM_TITLE,	"Settings",			 0 , 0, 0,  (PTR) MENU_FILE   },
	{ NM_ITEM,	"Load Setting",		 0 , 0, 0,  (PTR) MENU_OPEN   },
	{ NM_ITEM,	NM_BARLABEL,		 0 , 0, 0,  (PTR) 0           },
	{ NM_ITEM,	"Save Setting",		 0 , 0, 0,  (PTR) MENU_OPEN   },
	{ NM_ITEM,	"Save Setting as",	 0 , 0, 0,  (PTR) MENU_OPEN   },
	{ NM_ITEM,	"Save Default",		 0 , 0, 0,  (PTR) MENU_OPEN   },
	{ NM_ITEM,	NM_BARLABEL,		 0 , 0, 0,  (PTR) 0           },
	{ NM_ITEM,	"Reset",			 0 , 0, 0,  (PTR) MENU_EXIT   },
	{ NM_END,	NULL,				 0 , 0, 0,  (PTR) 0           },
};

// --

S32 GUIFunc_Init( struct Config *cfg UNUSED )
{
S32 retval;

	retval = FALSE;

	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "GUIFunc_Init : MUI\n" );
	}

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

	if ( ! AppClass_Init( cfg ))
	{
		goto bailout;
	}

	// --

	App = NewObject( AppClass->mcc_Class, NULL,
		MUIA_Application_Author,		"Rene W. Olsen",
		MUIA_Application_Base,			"rVNCd",
		MUIA_Application_Copyright,		"© 2024-2025 Rene W. Olsen",
		MUIA_Application_Description,	"Amiga VNC Server",
		MUIA_Application_Title,			"rVNCd",
		MUIA_Application_Version,		& VERSTAG[1],
		MUIA_Application_Menustrip,		MUI_MakeObject( MUIO_MenustripNM, MenuStr, 0 ),
		TAG_END
	);

	if ( ! App )
	{
		goto bailout;
	}

	DoMethod( App, MUIM_Application_NewInput, & ActiveGUI.gui_SignalBits );

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --

void GUIFunc_Free( struct Config *cfg UNUSED )
{
	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "GUIFunc_Free\n" );
	}

	// --

	ActiveGUI.gui_SignalBits = 0;

	if ( App )
	{
		MUI_DisposeObject( App );
		App = NULL;
	}

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
U32 mask;
U32 id;

	if ( ! App )
	{
		goto bailout;
	}

	do
	{
		id = DoMethod( App, MUIM_Application_NewInput, & ActiveGUI.gui_SignalBits );

		switch( id )
		{
			case MUIV_Application_ReturnID_Quit:
			{
				Program_Running = FALSE;
				break;
			}

			case GID_StartServer:
			{
				SHELLBUF_PRINTF( "GID_StartServer\n" );
				myStart_Server( cfg );
				break;
			}		

			case GID_StopServer:
			{
				SHELLBUF_PRINTF( "GID_StopServer\n" );
				myStop_Server( cfg );
				break;
			}

			default:
			{
				if ( id )
				{
					SHELLBUF_PRINTF1( "MUIID: %08" PRIx32 "\n", id );
				}
				break;
			}
		}
	}
	while( id );

bailout:

	return;
}

// --

void myGUI_AddKeyMessage( struct Config *cfg UNUSED, struct CommandKey *msg UNUSED )
{
	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "myGUI_AddKeyMessage\n" );
	}

	// --
}

// --

void myGUI_EncodingMessage( struct Config *cfg UNUSED, struct CommandEncoding *msg UNUSED )
{
	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "myGUI_EncodingMessage\n" );
	}

	// --
}

// --

void myGUI_PxlFmtRefresh( struct Config *cfg UNUSED )
{
	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "myGUI_PxlFmtRefresh\n" );
	}

	// --
}

// --
// Set Default Config

void GUIFunc_Default_Config( struct Config *cfg )
{
	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "GUIFunc_Default_Config\n" );
	}

	// --

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

#ifdef DEBUG
void GUIFunc_UpdateServerStatus( struct Config *cfg, enum ProcessStat pstat, STR file UNUSED, U32 line UNUSED )
#else
void GUIFunc_UpdateServerStatus( struct Config *cfg, enum ProcessStat pstat )
#endif
{
S32 stat;

	if ( ! Main_Win )
	{
		goto bailout;
	}

	// --

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
//	stat = ( cfg->cfg_ServerStatus == PROCESS_Stopped ) ? FALSE : TRUE ;

	stat = ( pstat == PROCESS_Stopped ) ? FALSE : TRUE ;
	set( btn_Server_On, MUIA_Disabled, stat );

	// --

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
//	stat = ( cfg->cfg_ServerStatus == PROCESS_Running ) ? FALSE : TRUE ;

	stat = ( pstat == PROCESS_Running ) ? FALSE : TRUE ;
	set( btn_Server_Off, MUIA_Disabled, stat );

	// --

bailout:

	return;
}

// --
