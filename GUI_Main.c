
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

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
	GID_Support_ZLib,
	GID_Disable_Blanker,
	GID_Send_Clipboard,
	GID_Read_Clipboard,
	GID_Port,
	GID_ServerName,
	GID_Password,

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

	GID_WhiteAdd,
	GID_WhiteList,
	GID_WhiteDelete,
	GID_WhiteEdit,
	GID_WhiteDown,
	GID_WhiteUp,

	GID_BlackAdd,
	GID_BlackList,
	GID_BlackDelete,
	GID_BlackEdit,
	GID_BlackDown,
	GID_BlackUp,

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
	GID_Stats_Whitelisted,
	GID_Stats_Blacklisted,
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

static struct List				MainListHeader;
static APTR						MainMenuStrip		= NULL;

static struct List				MainWhiteList;
static struct List				MainBlackList;

static struct List				StatusList;
static int						StatusListCount		= 0;

static char						WorkBuffer[MAX_ACTIONBUFFER];
static Object *					GUIObjects[GID_LAST];

static struct FileRequester	*	AslHandle			= NULL;
static char						OnlineUserBuffer[128];

struct DiskObject *				ProgramIcon			= NULL;

// --

static const char *ActionTypes[] =
{
	"Cmd",
	NULL
};

static const char *WinStatStrings[] =
{
	"Closed",
	"Opened",
	"Iconified",
	NULL
};

// --

static void VARARGS68K mySetTags( struct Config *cfg, void *object, ... );

// --

#include "GUI_Main_Actions_Program.c"
#include "GUI_Main_Actions_Server.c"
#include "GUI_Main_Actions_User.c"
#include "GUI_Main_Actions.c"

#include "GUI_Main_List_Black.c"
#include "GUI_Main_List_White.c"

#include "GUI_Main_Protocols_v33.c"
#include "GUI_Main_Protocols_v37.c"
#include "GUI_Main_Protocols.c"

#include "GUI_Main_Log.c"
#include "GUI_Main_Mouse.c"
#include "GUI_Main_Stats.c"
#include "GUI_Main_Screen.c"
#include "GUI_Main_Server.c"
#include "GUI_Main_Program.c"

#include "GUI_Main_Menu.c"

// --

struct LVStruct
{
	const char *	Title;
};

static const struct LVStruct LVMain[] =
{
{ "Main" },
{ "Program" },
{ "Actions" },
{ "Screen" },
{ "Server" },
{ "Server Stats" },
{ "Log" },
{ "Mouse" },
{ "Protocols" },
{ "Black List" },
{ "White List" },
{ NULL },
};

static struct ColumnInfo StatusColumnInfo[] =
{
	{ 80, "Time", 0 },
	{ 80, "Type", 0 },
	{ 80, "Description", 0 },
	{ -1, (APTR) -1, -1 }
};

// --

static Object *Page_Main( struct Config *cfg UNUSED )
{
Object *o;

	// Proberly show a logo here
	o = IIntuition->NewObject( LayoutClass, NULL,

		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,

			LAYOUT_Orientation,				LAYOUT_ORIENT_HORIZ,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,

				LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,

				LAYOUT_AddChild,			IIntuition->NewObject( ButtonClass, NULL,
					GA_Text,				& VERSTAG[7],
					GA_ReadOnly,			TRUE,
					BUTTON_Transparent,		TRUE,
					BUTTON_BevelStyle,		BVS_NONE,
				End,

			End,
			CHILD_WeightedWidth,			0,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,

			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Users",

			LAYOUT_AddChild,				GUIObjects[ GID_Main_UserConn ] = IIntuition->NewObject( ButtonClass, NULL,
				GA_Text,					"User 1:",
				GA_ReadOnly,				TRUE,
				BUTTON_Justification,		BCJ_LEFT,
				BUTTON_Transparent,			TRUE,
				BUTTON_BevelStyle,			BVS_NONE,
			End,

		End,
		CHILD_WeightedHeight,				0,
	End;

	return( o );
}

// --

static Object *Page_NeedUpdate( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( ButtonClass, NULL,
		BUTTON_Justification,	BCJ_CENTER,
		BUTTON_BevelStyle,		BVS_NONE,
		BUTTON_Transparent,		TRUE,
		GA_ReadOnly,			TRUE,
		GA_Text,				"Settings updated, restart required",
	End;

	return( o );
}

// --

static Object *Page_Empty( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_BevelStyle, BVS_NONE,
	End;

	return( o );
}

// --

void myGUI_BusyMainWindow( struct Config *cfg, int val )
{
	if ( val )
	{
		/**/ cfg->cfg_WinData[WIN_Main].Busy++;

		if ( cfg->cfg_WinData[WIN_Main].Busy == 1 )
		{
			mySetTags( cfg, GUIObjects[ GID_Window ],
				WA_BusyPointer, TRUE,
				TAG_END
			);
		}
	}
	else
	{
		if ( cfg->cfg_WinData[WIN_Main].Busy > 0 )
		{
			 cfg->cfg_WinData[WIN_Main].Busy--;

			if ( cfg->cfg_WinData[WIN_Main].Busy == 0 )
			{
				mySetTags( cfg, GUIObjects[ GID_Window ],
					WA_BusyPointer, FALSE,
					TAG_END
				);
			}
		}
	}
}

// --

int myGUI_OpenMainWindow( struct Config *cfg UNUSED )
{
struct CommandLogString *node;
struct Node *n;
char *str;
int error;
int pos;

	// --

	error = FALSE;

	if ( cfg->cfg_ProgramDisableGUI )
	{
		Log_PrintF( cfg, LOGTYPE_Warning, "Program: GUI Disabled can't open" );
		goto bailout;
	}

	if ( GUIObjects[ GID_Window ] )
	{
		if ( cfg->cfg_WinData[WIN_Main].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_Main].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_Main].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
			}
		}
		else
		{
			IIntuition->WindowToFront( cfg->cfg_WinData[WIN_Main].WindowAdr );
			IIntuition->ActivateWindow( cfg->cfg_WinData[WIN_Main].WindowAdr );
		}
		goto bailout;
	}

	error = TRUE;

	// --

	myGUI_CreateMenus( cfg );

	// --

	IExec->NewList( & MainListHeader );
	IExec->NewList( & MainWhiteList );
	IExec->NewList( & MainBlackList );
	IExec->NewList( & StatusList );
	StatusListCount = 0;

	pos = 0;

	while( LVMain[pos].Title )
	{
		n = IListBrowser->AllocListBrowserNode( 1,
			LBNA_Column, 0,
				LBNCA_CopyText, FALSE,
				LBNCA_Text, LVMain[pos].Title,
			TAG_END
		);

		if ( n )
		{
			IExec->AddTail( & MainListHeader, n );
		}

		pos++;
	}

	// --

	node = (APTR) IExec->GetHead( & LogStringList );

	while( node )
	{
		/**/ if (( node->cl_Type & 0xf ) == LOGTYPE_Error )
		{
			str = "Error";
		}
		else if (( node->cl_Type & 0xf ) == LOGTYPE_Warning )
		{
			str = "Warning";
		}
		else // Info
		{
			str = "Info";
		}

		// Can use WordWrap and AutoFit at the same time
		n = IListBrowser->AllocListBrowserNode( 3,
			LBNA_Column, 0,
				LBNCA_Text, node->cl_Time,
			LBNA_Column, 1,
				LBNCA_Text, str,
			LBNA_Column, 2,
				LBNCA_Text, node->cl_String,
//				LBNCA_WordWrap, TRUE,
			TAG_END
		);

		if ( n )
		{
			IExec->AddTail( & StatusList, n );
			StatusListCount++;
		}

		node = (APTR) IExec->GetSucc( (APTR) node );
	}

	// --

	GUIObjects[ GID_Window ] = IIntuition->NewObject( WindowClass, NULL,
		WA_Activate,							    TRUE,
		WA_CloseGadget,							    TRUE,
		WA_DepthGadget,							    TRUE,
		WA_DragBar,								    TRUE,
		WA_SizeGadget,							    TRUE,
		WA_Title,								    & VERSTAG[7],
		WA_BusyPointer,								cfg->cfg_WinData[WIN_Main].Busy > 0,

		( cfg->cfg_WinData[WIN_Main].Width == 0 ) ?
		TAG_IGNORE : WA_Left, cfg->cfg_WinData[WIN_Main].XPos,

		( cfg->cfg_WinData[WIN_Main].Width == 0 ) ?
		TAG_IGNORE : WA_Top, cfg->cfg_WinData[WIN_Main].YPos,

		WA_Width, ( cfg->cfg_WinData[WIN_Main].Width != 0 ) ?
		cfg->cfg_WinData[WIN_Main].Width : 700,

		WA_Height, ( cfg->cfg_WinData[WIN_Main].Width != 0 ) ?
		cfg->cfg_WinData[WIN_Main].Height : 500,

		( cfg->cfg_WinData[WIN_Main].Width != 0 ) ?
		TAG_IGNORE : WINDOW_Position, WPOS_CENTERSCREEN,

//		WA_PubScreen,							    gs->up_PubScreen,
		WINDOW_AppPort,								WinAppPort,
//		WINDOW_AppWindow,							TRUE,
		WINDOW_SharedPort,							WinMsgPort,
		WINDOW_MenuStrip,					    	MainMenuStrip,
		WINDOW_PopupGadget,							TRUE,
		WINDOW_Icon,								ProgramIcon,
		WINDOW_IconTitle,							"rVNCd",
		WINDOW_IconNoDispose,						TRUE,
		WINDOW_IconifyGadget,						TRUE,
//		WINDOW_MenuUserData,				    	WGUD_HOOK,
//		WINDOW_Position,						    WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,						    GUIObjects[ GID_Root ] = IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,					GUIObjects[ GID_LVSelect ] = IIntuition->NewObject( ListBrowserClass, NULL,
					GA_ID,							GID_LVSelect,
					GA_RelVerify,					TRUE,
					LISTBROWSER_ShowSelected,		TRUE,
					LISTBROWSER_AutoFit,			TRUE,
					LISTBROWSER_Labels,				& MainListHeader,
				End,
				CHILD_WeightedWidth,				25,

				LAYOUT_WeightBar,					TRUE,

				LAYOUT_AddChild,					GUIObjects[ GID_LVClickTab ] = IIntuition->NewObject( ClickTabClass, NULL,
					CLICKTAB_Current,				0,
					CLICKTAB_PageGroup,				GUIObjects[ GID_LVPage ] = IIntuition->NewObject( PageClass, NULL,
						PAGE_Add,					Page_Main( cfg ),
						PAGE_Add,					Page_Program( cfg ),
						PAGE_Add,					Page_Actions( cfg ),
						PAGE_Add,					Page_Screen( cfg ),
						PAGE_Add,					Page_Server( cfg ),
						PAGE_Add,					Page_Stats( cfg ),
						PAGE_Add,					Page_Log( cfg ),
						PAGE_Add,					Page_Mouse( cfg ),
						PAGE_Add,					Page_Protocols( cfg ),
						PAGE_Add,					Page_Black_List( cfg ),
						PAGE_Add,					Page_White_List( cfg ),
					End,
				End,
				CHILD_WeightedWidth,				75,
			End,
			CHILD_WeightedHeight,					75,

			LAYOUT_WeightBar,						TRUE,

			LAYOUT_AddChild,						GUIObjects[ GID_Status ] = IIntuition->NewObject( ListBrowserClass, NULL,
				LISTBROWSER_ColumnInfo,			    & StatusColumnInfo,
				LISTBROWSER_Labels,				    & StatusList,
				LISTBROWSER_Striping,				LBS_ROWS,
				LISTBROWSER_AutoFit,				TRUE,
//				LISTBROWSER_WrapText,				TRUE,
			End,
			CHILD_WeightedHeight,					25,

			LAYOUT_AddChild,					    IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,					GUIObjects[ GID_StartServer ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,							GID_StartServer,
					GA_Text,						"Online",
					GA_Disabled,					( cfg->cfg_ServerStatus == PROCESS_Stopped ) ? FALSE : TRUE,
					GA_RelVerify,					TRUE,
					BUTTON_Justification,			BCJ_CENTER,
				End,
				CHILD_WeightedWidth,				30,

				LAYOUT_AddChild,					GUIObjects[ GID_NeedTab ] = IIntuition->NewObject( ClickTabClass, NULL,
					CLICKTAB_Current,				( cfg->cfg_SettingChanged ) ? 1 : 0,
					CLICKTAB_PageGroup,				GUIObjects[ GID_NeedPage ] = IIntuition->NewObject( PageClass, NULL,
						PAGE_Add,					Page_Empty( cfg ),
						PAGE_Add,					Page_NeedUpdate( cfg ),
					End,
				End,
				CHILD_WeightedWidth,				40,

				LAYOUT_AddChild,					GUIObjects[ GID_StopServer ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,							GID_StopServer,
					GA_Text,						"Offline",
					GA_Disabled,					( cfg->cfg_ServerStatus == PROCESS_Running ) ? FALSE : TRUE,
					GA_RelVerify,					TRUE,
					BUTTON_Justification,			BCJ_CENTER,
				End,
				CHILD_WeightedWidth,				30,
			End,
			CHILD_WeightedHeight,				    0,
		End,
	End;

	if ( ! GUIObjects[ GID_Window ] )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating Main GUI Object" );
		goto bailout;
	}

	if ( cfg->cfg_WinData[WIN_Main].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_Main].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_Main].WindowAdr == NULL )
		{
			goto bailout;
		}

//		cfg->cfg_WinData[WIN_Main].WindowAdr->UserData = (APTR) myGUI_HandleMainWindow;
	}
	else
	{
		if ( IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ) == 0 )
		{
			goto bailout;
		}
	}

	error = FALSE;

bailout:

	return( error );
}

// --

void myGUI_CloseMainWindow( struct Config *cfg UNUSED )
{
struct Window *win;

	if ( ! GUIObjects[ GID_Window ] )
	{
		goto bailout;
	}

	win = cfg->cfg_WinData[WIN_Main].WindowAdr;

	if ( win )
	{
		cfg->cfg_WinData[WIN_Main].XPos = win->LeftEdge;
		cfg->cfg_WinData[WIN_Main].YPos = win->TopEdge;
		cfg->cfg_WinData[WIN_Main].Width = win->Width - win->BorderLeft - win->BorderRight;
		cfg->cfg_WinData[WIN_Main].Height = win->Height - win->BorderTop - win->BorderBottom;
	}

	IIntuition->DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_Main].WindowAdr = NULL;

	myGUI_DeleteMenus( cfg );

	if ( AslHandle )
	{
		IAsl->FreeAslRequest( AslHandle );
		AslHandle = NULL;
	}

bailout:

	return;
}

// --

void GUIFunc_UpdateServerStatus( struct Config *cfg )
{
int stat;

	if ( ! GUIObjects[ GID_Window ] )
	{
		goto bailout;
	}

	// --

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	stat = ( cfg->cfg_ServerStatus == PROCESS_Stopped ) ? FALSE : TRUE ;

	mySetTags( cfg, GUIObjects[ GID_StartServer ],
		GA_Disabled, stat,
		TAG_END
	);

	// --

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	stat = ( cfg->cfg_ServerStatus == PROCESS_Running ) ? FALSE : TRUE ;

	mySetTags( cfg, GUIObjects[ GID_StopServer ],
		GA_Disabled, stat,
		TAG_END
	);

	// --

	myGUI_Main_CheckSettings( cfg );

// --

bailout:

	return;
}

// --

void GUIFunc_StartServer( struct Config *cfg )
{
	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	if ( cfg->cfg_ServerStatus == PROCESS_Stopped )
	{
		myStart_Server( cfg );
	}
}

// --

void GUIFunc_StopServer( struct Config *cfg )
{
	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	if ( cfg->cfg_ServerStatus == PROCESS_Running )
	{
		myStop_Server( cfg );
	}
}

// --

static void GUIFunc_LVSelect( struct Config *cfg UNUSED )
{
uint32 cur;
uint32 sel;

	cur = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_LVPage ],
		PAGE_Current, & cur,
		TAG_END
	);

	sel = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_LVSelect ],
		LISTBROWSER_Selected, & sel,
		TAG_END
	);

	if ( cur != sel )
	{
		mySetTags( cfg, GUIObjects[ GID_LVPage ],
			PAGE_Current, sel,
			TAG_END
		);

		IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_RETHINK );
	}
}

// --

void myGUI_Main_RefreshSettings( struct Config *cfg )
{
	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	// -- Program

	mySetTags( cfg, GUIObjects[ GID_Program_Verbose ],
		GA_Selected, ( DoVerbose ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Program_AutoStart ],
		GA_Selected, ( cfg->AutoStart ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Program_Disable_ARexx ],
		GA_Selected, ( cfg->cfg_ProgramDisableARexx ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Program_Disable_CxBroker ],
		GA_Selected, ( cfg->cfg_ProgramDisableCxBroker ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Program_Disable_GUI ],
		GA_Selected, ( cfg->cfg_ProgramDisableGUI ) ? 1 : 0,
		TAG_END
	);

	// -- Server

	mySetTags( cfg, GUIObjects[ GID_Port ],
		INTEGER_Number,	cfg->cfg_Disk_Settings.Port,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_ServerName ],
		STRINGA_TextVal, cfg->cfg_Disk_Settings.Name,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Password ],
		STRINGA_TextVal, cfg->cfg_Disk_Settings.Password,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Disable_Blanker ],
		GA_Selected, ( cfg->cfg_Disk_Settings.DisableBlanker ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Send_Clipboard ],
		GA_Selected, ( cfg->cfg_Disk_Settings.SendClipboard ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Read_Clipboard ],
		GA_Selected, ( cfg->cfg_Disk_Settings.ReadClipboard ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Support_ZLib ],
		GA_Selected, ( cfg->cfg_Disk_Settings.ZLib ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Support_RichCursor ],
		GA_Selected, ( cfg->cfg_Disk_Settings.RichCursor ) ? 1 : 0,
		TAG_END
	);

	// -- Connection

	myGUI_Main_UpdateWhiteList( cfg );
	myGUI_Main_UpdateBlackList( cfg );

	// -- Screen

	// -- Log



	Page_Refresh_Log( cfg );












	// -- Screen

	Page_Refresh_Screen( cfg );

	// -- Mouse

	Page_Refresh_Mouse( cfg );

	// -- Protocol v3.3

	Page_Refresh_Protocol_33( cfg );

	// -- Protocol v3.7

	Page_Refresh_Protocol_37( cfg );

	// -- Action : Program

	Page_Refresh_Action_Program( cfg );

	// -- Action : Server

	Page_Refresh_Action_Server( cfg );

	// -- Action : User

	Page_Refresh_Action_User( cfg );

	// -- Stats

	Page_Refresh_Stats( cfg );

	// --




	if ( cfg->cfg_WinData[WIN_Main].WindowAdr )
	{
		IIntuition->RefreshGadgets( (APTR) GUIObjects[ GID_Root ], cfg->cfg_WinData[WIN_Main].WindowAdr, NULL );
	}
}

// --

void myGUI_AddLogStringMessage( struct Config *cfg UNUSED, struct CommandLogString *msg )
{
char *str;
APTR n;

	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	mySetTags( cfg, GUIObjects[ GID_Status ],
		LISTBROWSER_Labels,	NULL,
		TAG_END
	);

	/**/ if (( msg->cl_Type & 0x0f ) == LOGTYPE_Error )
	{
		str = "Error";
	}
	else if (( msg->cl_Type & 0xf ) == LOGTYPE_Warning )
	{
		str = "Warning";
	}
	else // Info
	{
		str = "Info";
	}

	// Can't use WordWrap and AutoFit at the same time
	n = IListBrowser->AllocListBrowserNode( 3,
		LBNA_Column, 0,
			LBNCA_Text, msg->cl_Time,
		LBNA_Column, 1,
			LBNCA_Text, str,
		LBNA_Column, 2,
			LBNCA_Text, msg->cl_String,
//			LBNCA_WordWrap, TRUE,
		TAG_END
	);

	if ( n )
	{
		IExec->AddTail( & StatusList, n );
		StatusListCount++;

		while( MAX_LogListNodes < StatusListCount )
		{
			n = IExec->RemHead( & StatusList );
			IListBrowser->FreeListBrowserNode( n );
			StatusListCount--;
		}
	}

	mySetTags( cfg, GUIObjects[ GID_Status ],
		LISTBROWSER_Labels,	& StatusList,
		LISTBROWSER_Position, LBP_BOTTOM,
		LISTBROWSER_AutoFit, TRUE,
		TAG_END
	);
}

// --

void myGUI_RefreshMessage( struct Config *cfg, struct CommandRefresh *msg UNUSED )
{
	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	myGUI_Main_RefreshSettings( cfg );
}

// --

void myGUI_RefreshStats( struct Config *cfg, struct CommandRefresh *msg UNUSED )
{
	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	Page_Refresh_Stats( cfg );
}

// --

static void myShowHideMenus( Object *m, int hide )
{
//Object *p;
//Object *c;

	if ( m == NULL )
	{
		goto bailout;
	}

	IIntuition->SetAttrs( m, MA_Hidden, hide, TAG_END );

	#if 0
	IIntuition->GetAttrs( m,
		MA_Parent, & p,
		TAG_END
	);

	while( p )
	{
		m = (APTR) IIntuition->IDoMethod( p, MM_NEXTCHILD, 0, m );

printf( "m: %p\n", p );
printf( "c: %p\n", m );

		if ( m == NULL )
		{
			break;
		}
		else
		{
			myShowHideMenus( m, hide );
		}
	}
	#endif

bailout:

	return;
}

// --

void myGUI_Main_SessionMessage( struct Config *cfg, struct CommandSession *msg )
{
Object *m;
int hide;

	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	if ( msg->cs_Starting )
	{
		static char Buf_Connect[64];

		LogTime_GetConnectTime( & cfg->SessionStatus.si_LogTime, Buf_Connect, sizeof( Buf_Connect ));

		sprintf( OnlineUserBuffer, "User 1: %d.%d.%d.%d  (%s)", 
			cfg->SessionStatus.si_IPAddr[0],
			cfg->SessionStatus.si_IPAddr[1],
			cfg->SessionStatus.si_IPAddr[2],
			cfg->SessionStatus.si_IPAddr[3],
			Buf_Connect
		);
	}
	else
	{
		sprintf( OnlineUserBuffer, "User 1:" );
	}

	mySetTags( cfg, GUIObjects[ GID_Main_UserConn ],
		GA_Text, OnlineUserBuffer,
		TAG_END
	);

	// -- Adjust menus

	hide = msg->cs_Starting == 0;

	m = (APTR) IIntuition->IDoMethod( MainMenuStrip, MM_FINDID, 0, MENUID_User1 );

	myShowHideMenus( m, hide );
}

// --

static void VARARGS68K mySetTags( struct Config *cfg, void *object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_Main].WindowAdr )
	{
		ILayout->SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_Main].WindowAdr,
			NULL,
			va_getlinearva( ap, APTR )
		);
	}
	else
	{
		IIntuition->SetAttrsA( object, va_getlinearva( ap, APTR ));
	}

	va_end( ap );
}

// --

void myGUI_Main_CheckSettings( struct Config *cfg )
{
int changed;

	if (( cfg->cfg_ServerStatus != PROCESS_Starting )
	&&	( cfg->cfg_ServerStatus != PROCESS_Running ))
	{
		changed = 0;
		goto update;
	}

	while( TRUE )
	{
		changed = 1;

		if ( memcmp( & cfg->cfg_Disk_Settings, & cfg->cfg_Active_Settings, sizeof( struct Settings )))
		{
			break;
		}

		// Check Pointer Filename, if Type is set to Custom
		if ( cfg->cfg_Disk_Settings.PointerType )
		{
			if ( ! mySameString( cfg->cfg_PointerFilename, cfg->cfg_PointerFilename2 ))
			{
				break;
			}
		}

		// Check other things

		changed = 0;
		break;
	}

update:

	if ( cfg->cfg_SettingChanged != changed )
	{
		cfg->cfg_SettingChanged = changed;

		if ( GUIObjects[ GID_Window ] )
		{
			mySetTags( cfg, GUIObjects[ GID_NeedPage ],
				PAGE_Current, ( cfg->cfg_SettingChanged ) ? 1 : 0,
				TAG_END
			);

			IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_RETHINK );
		}
	}
}

// --

void myGUI_Main_TimerTick( struct Config *cfg )
{
	if ( ! GUIObjects[ GID_Window ] )
	{
		goto bailout;
	}

	Page_Refresh_Stats( cfg );

bailout:

	return;
}

// --

static const GUIFunc GUIFuncs[GID_LAST] =
{
	[GID_StartServer]						= { .Function = GUIFunc_StartServer },
	[GID_StopServer]						= { .Function = GUIFunc_StopServer },
	[GID_LVSelect]							= { .Function = GUIFunc_LVSelect },
	[GID_Support_RichCursor]				= { .Function = GUIFunc_Support_RichCursor },
	[GID_Support_ZLib]						= { .Function = GUIFunc_Support_ZLib },
	[GID_Disable_Blanker]					= { .Function = GUIFunc_Disable_Blanker },
	[GID_Send_Clipboard]					= { .Function = GUIFunc_Send_Clipboard },
	[GID_Read_Clipboard]					= { .Function = GUIFunc_Read_Clipboard },
	[GID_Port]								= { .Function = GUIFunc_Port },
	[GID_Program_AutoStart]					= { .Function = GUIFunc_AutoStart },
	[GID_Program_Verbose]					= { .Function = GUIFunc_Verbose },
	[GID_Program_Disable_ARexx]				= { .Function = GUIFunc_ARexx },
	[GID_Program_Disable_CxBroker]			= { .Function = GUIFunc_Broker },
	[GID_Program_Disable_GUI]				= { .Function = GUIFunc_GUI },
	[GID_Program_WinStat]					= { .Function = GUIFunc_ProgramWinStat },
	[GID_Log_Program_Start]					= { .Function = GUIFunc_ProgramStart },
	[GID_Log_Program_Stop]					= { .Function = GUIFunc_ProgramStop },
	[GID_Log_Server_Start]					= { .Function = GUIFunc_ServerStart },
	[GID_Log_Server_Stop]					= { .Function = GUIFunc_ServerStop },
	[GID_Log_User_Connect]					= { .Function = GUIFunc_UserConnect },
	[GID_Log_User_Disconnect]				= { .Function = GUIFunc_UserDisconnect },
	[GID_Log_Infos]							= { .Function = GUIFunc_Infos },
	[GID_Log_Warnings]						= { .Function = GUIFunc_Warnings },
	[GID_Log_Errors]						= { .Function = GUIFunc_Errors },
	[GID_ServerName]						= { .Function = GUIFunc_UpdateServerName },
	[GID_Password]							= { .Function = GUIFunc_UpdatePassword },
	[GID_Actions_Program_Start_String]		= { .Function = GUIFunc_UpdateActionProgramStart },
	[GID_Actions_Program_Stop_String]		= { .Function = GUIFunc_UpdateActionProgramStop },
	[GID_Actions_Server_Start_String]		= { .Function = GUIFunc_UpdateActionServerStart },
	[GID_Actions_Server_Stop_String]		= { .Function = GUIFunc_UpdateActionServerStop },
	[GID_Actions_User_Connect_String]		= { .Function = GUIFunc_UpdateActionUserConnect },
	[GID_Actions_User_Disconnect_String]	= { .Function = GUIFunc_UpdateActionUserDisconnect },

					// ------------
					// -- Black List

	[GID_BlackAdd]							= { .Function = GUIFunc_BlackAdd },
	[GID_BlackEdit]							= { .Function = GUIFunc_BlackEdit },
	[GID_BlackList]							= { .Function = GUIFunc_BlackList },
	[GID_BlackUp]							= { .Function = GUIFunc_BlackUp },
	[GID_BlackDown]							= { .Function = GUIFunc_BlackDown },
	[GID_BlackDelete]						= { .Function = GUIFunc_BlackDelete },

					// ------------
					// -- White List

	[GID_WhiteAdd]							= { .Function = GUIFunc_WhiteAdd },
	[GID_WhiteEdit]							= { .Function = GUIFunc_WhiteEdit },
	[GID_WhiteList]							= { .Function = GUIFunc_WhiteList },
	[GID_WhiteUp]							= { .Function = GUIFunc_WhiteUp },
	[GID_WhiteDown]							= { .Function = GUIFunc_WhiteDown },
	[GID_WhiteDelete]						= { .Function = GUIFunc_WhiteDelete },

					// ------------
					// -- Screen

	[GID_Screen_TileSize]					= { .Function = GUIFunc_Screen_TileSize },
	[GID_Screen_DelaySlider]				= { .Function = GUIFunc_Screen_Delay },

					// ------------
					// -- Mouse

	[GID_Mouse_Type]						= { .Function = GUIFunc_Mouse_Type },
	[GID_Mouse_Filename]					= { .Function = GUIFunc_Mouse_Filename },

					// ------------
					// -- Protocol
					// -- v3.3

	[GID_Protocol_33_Enable]				= { .Function = GUIFunc_Protocol_33_Enable },

					// -- v3.7

	[GID_Protocol_37_Enable]				= { .Function = GUIFunc_Protocol_37_Enable },

					// ------------
					// -- Actions
					// -- Program

	[GID_Actions_Program_Start_Enable]		= { .Function = GUIFunc_Actions_Program_Start_Enable },
	[GID_Actions_Program_Stop_Enable]		= { .Function = GUIFunc_Actions_Program_Stop_Enable },

					// -- Server

	[GID_Actions_Server_Start_Enable]		= { .Function = GUIFunc_Actions_Server_Start_Enable },
	[GID_Actions_Server_Stop_Enable]		= { .Function = GUIFunc_Actions_Server_Stop_Enable },

					// -- User

	[GID_Actions_User_Connect_Enable]		= { .Function = GUIFunc_Actions_User_Connect_Enable },
	[GID_Actions_User_Disconnect_Enable]	= { .Function = GUIFunc_Actions_User_Disconnect_Enable },
};

// --

void myGUI_HandleMainWindow( struct Config *cfg )
{
void (*Func)(struct Config *);
uint32 result;
uint16 code;
int theend;
int id;

    theend = FALSE;

	while(( result = IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_HANDLEINPUT, &code )) != WMHI_LASTMSG )
	{
		switch( result & WMHI_CLASSMASK )
		{
			case WMHI_CLOSEWINDOW:
			{
				theend = TRUE;
				break;
			}

			case WMHI_MENUPICK:
			{
				myGUI_MenuPick( cfg );
				break;
			}

			case WMHI_ICONIFY:
			{
				cfg->cfg_WinData[WIN_Main].Status = WINSTAT_Iconifyed;
				if ( IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_Main].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_Main].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_Main].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
				break;
			}

			case WMHI_GADGETUP:
			{
				id = result & WMHI_GADGETMASK;

				if ( id >= GID_LAST )
				{
					break;
				}

				Func = GUIFuncs[id].Function;

				if ( ! Func )
				{
					break;
				}

				Func( cfg );
				break;
			}

			default:
			{
				break;
			}
		}
	}

	if ( theend )
	{
		myGUI_CloseMainWindow( cfg );
	}
}

// --
