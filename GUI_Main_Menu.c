
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static void myMenu_Reset( struct Config *cfg )
{
	Config_Reset( cfg );

	myGUI_Main_RefreshSettings( cfg );
}

// --

static void myMenu_User1_Kick( struct Config *cfg )
{
	if ( cfg->UserCount )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Kicking User 1" );
		}

		cfg->cfg_KickUser = 1;
	}
}

// --

static void myMenu_SaveAS( struct Config *cfg UNUSED )
{
char *Filename;
BOOL stat;
int len;
//int err;

	Filename = NULL;

	cfg->cfg_WinData[WIN_Main].BusyWin( cfg, TRUE );

	if ( AslHandle == NULL )
	{
		AslHandle = IAsl->AllocAslRequestTags( ASL_FileRequest,
			ASLFR_InitialPattern, "#?.config",
			ASLFR_DoPatterns, TRUE,
			TAG_END
		);

		if ( AslHandle == NULL )
		{
			goto bailout;
		}
	}

	stat = IAsl->AslRequestTags( AslHandle,
		ASLFR_Window, cfg->cfg_WinData[WIN_Main].WindowAdr,
		TAG_END
	);

	if ( stat )
	{
		if (( AslHandle->fr_File == NULL ) || ( AslHandle->fr_File[0] == 0 ))
		{
			IDOS->TimedDosRequesterTags(
				TDR_Window, cfg->cfg_WinData[WIN_Main].WindowAdr,
				TDR_ImageType, TDRIMAGE_ERROR,
				TDR_TitleString, "rVNCd",
				TDR_FormatString, "Filename is missing",
				TDR_GadgetString, "Okay",
				TAG_END
			);
			goto bailout;
		}

		if (( AslHandle->fr_Drawer ) && ( AslHandle->fr_Drawer[0] ))
		{
			len = strlen( AslHandle->fr_Drawer );

			if (( AslHandle->fr_Drawer[len-1] == ':' ) || ( AslHandle->fr_Drawer[len-1] == '/' ))
			{
				Filename = myASPrintF( "%s%s", AslHandle->fr_Drawer, AslHandle->fr_File );
			}
			else
			{
				Filename = myASPrintF( "%s/%s", AslHandle->fr_Drawer, AslHandle->fr_File );
			}
		}
		else
		{
			Filename = myASPrintF( "%s", AslHandle->fr_File );
		}

		if ( Filename == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
			goto bailout;
		}

		Config_Save( cfg, Filename );
	}

bailout:

	cfg->cfg_WinData[WIN_Main].BusyWin( cfg, FALSE );

	if ( Filename )
	{
		myFree( Filename );
	}

	return;
}

// --

static void myMenu_Load( struct Config *cfg )
{
char *Filename;
BOOL stat;
int len;
int err;

	Filename = NULL;

	cfg->cfg_WinData[WIN_Main].BusyWin( cfg, TRUE );

	if ( AslHandle == NULL )
	{
		AslHandle = IAsl->AllocAslRequestTags( ASL_FileRequest,
			ASLFR_InitialPattern, "#?.config",
			ASLFR_DoPatterns, TRUE,
			TAG_END
		);

		if ( AslHandle == NULL )
		{
			goto bailout;
		}
	}

	stat = IAsl->AslRequestTags( AslHandle,
		ASLFR_Window, cfg->cfg_WinData[WIN_Main].WindowAdr,
		TAG_END
	);

	if ( stat )
	{
		if (( AslHandle->fr_File == NULL ) || ( AslHandle->fr_File[0] == 0 ))
		{
			IDOS->TimedDosRequesterTags(
				TDR_Window, cfg->cfg_WinData[WIN_Main].WindowAdr,
				TDR_ImageType, TDRIMAGE_ERROR,
				TDR_TitleString, "rVNCd",
				TDR_FormatString, "Filename is missing",
				TDR_GadgetString, "Okay",
				TAG_END
			);
			goto bailout;
		}

		if (( AslHandle->fr_Drawer ) && ( AslHandle->fr_Drawer[0] ))
		{
			len = strlen( AslHandle->fr_Drawer );

			if (( AslHandle->fr_Drawer[len-1] == ':' ) || ( AslHandle->fr_Drawer[len-1] == '/' ))
			{
				Filename = myASPrintF( "%s%s", AslHandle->fr_Drawer, AslHandle->fr_File );
			}
			else
			{
				Filename = myASPrintF( "%s/%s", AslHandle->fr_Drawer, AslHandle->fr_File );
			}
		}
		else
		{
			Filename = myASPrintF( "%s", AslHandle->fr_File );
		}

		if ( Filename == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
			goto bailout;
		}

		Config_Reset( cfg );
		err = Config_Read( cfg, Filename, TRUE );

		if ( err )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Failed to load '%s' Config file", IDOS->FilePart( Filename ));
		}
		else
		{
			myGUI_Main_RefreshSettings( cfg );
			myGUI_Main_CheckSettings( cfg );
			Log_PrintF( cfg, LOGTYPE_Info, "Config file '%s' loaded", IDOS->FilePart( Filename ) );
		}
	}

bailout:

	cfg->cfg_WinData[WIN_Main].BusyWin( cfg, FALSE );

	if ( Filename )
	{
		myFree( Filename );
	}

	return;
}

// --

static int myGUI_MenuID( struct Config *cfg, uint32 id )
{
int retval;

	retval = FALSE;

	switch( id )
	{
		case MENUID_KeyLogger:
		{
			cfg->cfg_WinData[WIN_KeyLogger].Status = WINSTAT_Open;
			cfg->cfg_WinData[WIN_KeyLogger].OpenWin( cfg );
			break;
		}

		case MENUID_Encodings:
		{
			cfg->cfg_WinData[WIN_Encodings].Status = WINSTAT_Open;
			cfg->cfg_WinData[WIN_Encodings].OpenWin( cfg );
			break;
		}

		case MENUID_PixelFormat:
		{
			cfg->cfg_WinData[WIN_PixelFormat].Status = WINSTAT_Open;
			cfg->cfg_WinData[WIN_PixelFormat].OpenWin( cfg );
			break;
		}

		case MENUID_Session:
		{
			cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Open;
			cfg->cfg_WinData[WIN_Session].OpenWin( cfg );
			break;
		}

		case MENUID_About:
		{
			cfg->cfg_WinData[WIN_About].Status = WINSTAT_Open;
			cfg->cfg_WinData[WIN_About].OpenWin( cfg );
			break;
		}

		case MENUID_Quit:
		{
			Func_Quit( cfg );
			break;
		}

		case MENUID_Load:
		{
			myMenu_Load( cfg );
			break;
		}

		case MENUID_Save:
		{
			Config_Save( cfg, cfg->cfg_Config_Filename );
			break;
		}

		case MENUID_SaveAS:
		{
			myMenu_SaveAS( cfg );
			break;
		}

		case MENUID_SaveDefault:
		{
			Config_Save( cfg, NULL );
			break;
		}

		case MENUID_Reset:
		{
			myMenu_Reset( cfg );
			break;
		}

		case MENUID_User1_Kick:
		{
			myMenu_User1_Kick( cfg );
			break;
		}

		default:
		{
			break;
		}
	}

	return( retval );
}

// --

static int myGUI_MenuPick( struct Config *cfg )
{
//Object *m;
uint32 id;
int retval;

	id = NO_MENU_ID;

	retval = FALSE;

	while( TRUE )
	{
		id = IIntuition->IDoMethod( MainMenuStrip, MM_NEXTSELECT, 0, id );
		
		if ( id == NO_MENU_ID )
		{
			break;
		}

		// Find Menu Object .. if needed
//		m = (APTR) IIntuition->IDoMethod( MainMenuStrip, MM_FINDID, 0, id );

		retval |= myGUI_MenuID( cfg, id );
	}

	return( retval );
}

// --

static int myGUI_CreateMenus( struct Config *cfg UNUSED )
{
	MainMenuStrip = IIntuition->NewObject( NULL, "menuclass",
		MA_Type,				T_ROOT,

		MA_AddChild,			IIntuition->NewObject( NULL, "menuclass",
			MA_Type,			T_MENU,
			MA_Label,			"Project",

// -- 			  MA_AddChild,		  IIntuition->NewObject( NULL, "menuclass",
// -- 				  MA_ID,		  MENUID_SaveDefaultLog,
// -- 				  MA_Type,		  T_ITEM,
// -- 				  MA_Label,		  GSTR(STR_MENU_PROJECT_SAVE_LOG),
// -- 				  MA_Key,		  GSTR(STR_MENU_PROJECT_SAVE_LOG_SHORTCUT),
// -- 			  End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

 			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
 				MA_Type,				T_ITEM,
 				MA_Label,				"Debug",

 				MA_AddChild,			IIntuition->NewObject( NULL, "menuclass",
 					MA_ID,				MENUID_Encodings,
 					MA_Type,			T_ITEM,
					MA_Label,			"Encodings",
//					MA_Key,				GSTR(STR_MENU_VIEW_ALL_SHORTCUT),
 				End,

 				MA_AddChild,			IIntuition->NewObject( NULL, "menuclass",
 					MA_ID,				MENUID_KeyLogger,
 					MA_Type,			T_ITEM,
					MA_Label,			"Key Logger",
//					MA_Key,				GSTR(STR_MENU_VIEW_ALL_SHORTCUT),
 				End,

 				MA_AddChild,			IIntuition->NewObject( NULL, "menuclass",
 					MA_ID,				MENUID_PixelFormat,
 					MA_Type,			T_ITEM,
					MA_Label,			"Pixel Format",
//					MA_Key,				GSTR(STR_MENU_VIEW_ALL_SHORTCUT),
 				End,

 				MA_AddChild,			IIntuition->NewObject( NULL, "menuclass",
 					MA_ID,				MENUID_Session,
 					MA_Type,			T_ITEM,
					MA_Label,			"Session Info",
//					MA_Key,				GSTR(STR_MENU_VIEW_ALL_SHORTCUT),
 				End,


			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

 			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
 				MA_Type,				T_ITEM,
 				MA_Label,				"Users",

 				MA_AddChild,			IIntuition->NewObject( NULL, "menuclass",
					MA_ID,				MENUID_User1,
 					MA_Type,			T_ITEM,
					MA_Label,			"User 1",
					MA_Hidden,			TRUE,

					MA_AddChild,		IIntuition->NewObject( NULL, "menuclass",
						MA_ID,			-1,
						MA_Type,		T_ITEM,
						MA_Label,		"Kick",

						MA_AddChild,	IIntuition->NewObject( NULL, "menuclass",
							MA_ID,		MENUID_User1_Kick,
							MA_Type,	T_ITEM,
							MA_Label,	"Confirm",
						End,
					End,
 				End,
			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_ID,					MENUID_About,
				MA_Type,				T_ITEM,
				MA_Label,				"About",
				MA_Key,					"?",
			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_ID,					MENUID_Quit,
				MA_Type,				T_ITEM,
				MA_Label,				"Quit",
				MA_Key,					"Q",
			End,
		End,

		MA_AddChild,					IIntuition->NewObject( NULL, "menuclass",
			MA_Type,					T_MENU,
			MA_Label,					"Preferences",

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_ID,					MENUID_Load,
				MA_Type,				T_ITEM,
				MA_Label,				"Load Settings",
			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_ID,					MENUID_Save,
				MA_Type,				T_ITEM,
				MA_Label,				"Save Settings",
			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_ID,					MENUID_SaveAS,
				MA_Type,				T_ITEM,
				MA_Label,				"Save Settings as",
			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_ID,					MENUID_SaveDefault,
				MA_Type,				T_ITEM,
				MA_Label,				"Save Settings as default",
			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

			MA_AddChild,				IIntuition->NewObject( NULL, "menuclass",
				MA_Type,				T_ITEM,
				MA_Label,				"Reset Settings",

				MA_AddChild,			IIntuition->NewObject( NULL, "menuclass",
					MA_ID,				MENUID_Reset,
					MA_Type,			T_ITEM,
					MA_Label,			"Confirm",
				End,
			End,
		End,
	End;

	return( ( MainMenuStrip ) ? 1 : 0 );
}

// --

static void myGUI_DeleteMenus( struct Config *cfg UNUSED )
{
	if ( MainMenuStrip )
	{
		IIntuition->DisposeObject( MainMenuStrip );
		MainMenuStrip = NULL;
	}
}

// --

