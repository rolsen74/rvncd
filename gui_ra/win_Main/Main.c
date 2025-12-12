
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

static struct VNCList			MainListHeader;
PTR MainMenuStrip = NULL;

struct VNCList MainIPList;
static struct VNCList StatusList;

char WorkBuffer[MAX_ACTIONBUFFER];
Object *Main_GUIObjects[GID_LAST];

struct FileRequester *AslHandle	= NULL;
static char						OnlineUserBuffer[128];

struct DiskObject *				ProgramIcon			= NULL;

// --

const STR ActionTypes[] =
{
	"Cmd",
	NULL
};

const STR WinStatStrings[] =
{
	"Closed",
	"Opened",
	"Iconified",
	NULL
};

const STR WinVerboseStrings[] =
{
	"0 : Off",
	"1 : Normal",
	"2 : Extra",
	"3 : Even More",
	NULL,
};

// --

enum PageID
{
	PageID_Main,
	PageID_Program,
	PageID_Actions,
	PageID_Screen,
	PageID_Server,
	PageID_Server_Stats,
	PageID_Log,
	PageID_Mouse,
	PageID_Protocols,
	PageID_IP,
	PageID_Last
};

enum PageID ActivePage = PageID_Main;

struct LVStruct
{
	const STR	Title;
};

static const struct LVStruct LVMain[PageID_Last+1] =
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
{ "IP List" },
{ NULL },
};

static struct ColumnInfo StatusColumnInfo[] =
{
	{ 80, "Time", 0 },
	{ 80, "Type", 0 },
	{ 80, "Description", 0 },
	{ -1, (PTR) -1, -1 }
};

// --

static STR SystemStr =
	"( "

	"System: "
	#ifdef _AOS3_
	"AOS3"
	#endif // _ASO3_
	#ifdef _AOS4_
	"AOS4"
	#endif // _ASO4_

	"  Graphics: "
	#ifdef HAVE_CGFX
	"CGfx"
	#endif // HAVE_CGFX
	#ifdef HAVE_P96
	"P96"
	#endif // HAVE_P96
	#ifdef HAVE_GFX54
	"Gfx54"
	#endif // HAVE_GFX

	" )";

static STR OptionStr = "( "
	#ifdef HAVE_AREXX
	"ARexx "
	#endif // HAVE_AREXX
	#ifdef HAVE_CXBROKER
	"CxBroker "
	#endif // HAVE_CXBROKER
	#ifdef HAVE_ZLIB
	"ZLib "
	#endif // HAVE_ZLIB
	")";

Object *Page_Main( struct Config *cfg UNUSED )
{
Object *o;

	// Proberly show a logo here
	o = NewObject( LayoutClass, NULL,

		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,

			LAYOUT_Orientation,				LAYOUT_ORIENT_HORIZ,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
			End,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,

				LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,

				LAYOUT_AddChild,			NewObject( ButtonClass, NULL,
					GA_Text,				& VERSTAG[7],
					GA_ReadOnly,			TRUE,
					BUTTON_Transparent,		TRUE,
					BUTTON_BevelStyle,		BVS_NONE,
				End,

				LAYOUT_AddChild,			NewObject( ButtonClass, NULL,
					GA_Text,				SystemStr,
					GA_ReadOnly,			TRUE,
					BUTTON_Transparent,		TRUE,
					BUTTON_BevelStyle,		BVS_NONE,
				End,

				LAYOUT_AddChild,			NewObject( ButtonClass, NULL,
					GA_Text,				OptionStr,
					GA_ReadOnly,			TRUE,
					BUTTON_Transparent,		TRUE,
					BUTTON_BevelStyle,		BVS_NONE,
				End,
			End,
			CHILD_WeightedWidth,			0,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,

			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Users",

			LAYOUT_AddChild,				GUIObjects[ GID_Main_UserConn ] = NewObject( ButtonClass, NULL,
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

Object *Page_NeedUpdate( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( ButtonClass, NULL,
		BUTTON_Justification,	BCJ_CENTER,
		BUTTON_BevelStyle,		BVS_NONE,
		BUTTON_Transparent,		TRUE,
		GA_ReadOnly,			TRUE,
		GA_Text,				"Settings updated, restart required",
	End;

	return( o );
}

// --

Object *Page_Empty( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_BevelStyle, BVS_NONE,
	End;

	return( o );
}

// --

void RAGUI_BusyMainWindow( struct Config *cfg, S32 val )
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

S32 RAGUI_OpenMainWindow( struct Config *cfg UNUSED )
{
struct CommandLogString *node;
struct Node *n;
STR str;
S32 error;
S32 pos;

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
				cfg->cfg_WinData[WIN_Main].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
			}
		}
		else
		{
			WindowToFront( cfg->cfg_WinData[WIN_Main].WindowAdr );
			ActivateWindow( cfg->cfg_WinData[WIN_Main].WindowAdr );
		}
		goto bailout;
	}

	error = TRUE;

	// --

	myGUI_CreateMenus( cfg );

	// --

	List_Init( & MainListHeader );
	List_Init( & MainIPList );
	List_Init( & StatusList );

	pos = 0;

	while( LVMain[pos].Title )
	{
		n = AllocListBrowserNode( 1,
			LBNA_Column, 0,
				LBNCA_CopyText, FALSE,
				LBNCA_Text, LVMain[pos].Title,
			TAG_END
		);

		if ( n )
		{
			List_AddTail( & MainListHeader, n );
		}

		pos++;
	}

	// --

	node = List_GetHead( & LogStringList );

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
		n = AllocListBrowserNode( 3,
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
			List_AddTail( & StatusList, n );
		}

		node = List_GetNext( node );
	}

	// --

	GUIObjects[ GID_Window ] = NewObject( WindowClass, NULL,
		WA_Activate,								TRUE,
		WA_CloseGadget,								TRUE,
		WA_DepthGadget,								TRUE,
		WA_DragBar,									TRUE,
		WA_SizeGadget,								TRUE,
		WA_Title,									& VERSTAG[7],
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

//		WA_PubScreen,								gs->up_PubScreen,
		WINDOW_AppPort,								& WinAppPort.vmp_MsgPort,
//		WINDOW_AppWindow,							TRUE,
		WINDOW_SharedPort,							& WinMsgPort.vmp_MsgPort,
		WINDOW_MenuStrip,							MainMenuStrip,
		WINDOW_PopupGadget,							TRUE,
		WINDOW_Icon,								ProgramIcon,
		WINDOW_IconTitle,							"rVNCd",
		WINDOW_IconNoDispose,						TRUE,
		WINDOW_IconifyGadget,						TRUE,
//		WINDOW_MenuUserData,						WGUD_HOOK,
//		WINDOW_Position,							WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,							GUIObjects[ GID_Root ] = NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,					GUIObjects[ GID_LVSelect ] = NewObject( ListBrowserClass, NULL,
					GA_ID,							GID_LVSelect,
					GA_RelVerify,					TRUE,
					LISTBROWSER_ShowSelected,		TRUE,
					LISTBROWSER_AutoFit,			TRUE,
					LISTBROWSER_Labels,				& MainListHeader.vl_List,
					LISTBROWSER_Selected,			ActivePage,
				End,
				CHILD_WeightedWidth,				25,

				LAYOUT_WeightBar,					TRUE,

				LAYOUT_AddChild,					GUIObjects[ GID_LVClickTab ] = NewObject( ClickTabClass, NULL,
					CLICKTAB_PageGroup,				GUIObjects[ GID_LVPage ] = NewObject( PageClass, NULL,
						PAGE_Add,					Page_Main( cfg ),
						PAGE_Add,					Page_Program( cfg ),
						PAGE_Add,					Page_Actions( cfg ),
						PAGE_Add,					Page_Screen( cfg ),
						PAGE_Add,					Page_Server( cfg ),
						PAGE_Add,					Page_Stats( cfg ),
						PAGE_Add,					Page_Log( cfg ),
						PAGE_Add,					Page_Mouse( cfg ),
						PAGE_Add,					Page_Protocols( cfg ),
						PAGE_Add,					Page_IP_List( cfg ),
					End,
					CLICKTAB_Current,				ActivePage,
				End,
				CHILD_WeightedWidth,				75,
			End,
			CHILD_WeightedHeight,					75,

			LAYOUT_WeightBar,						TRUE,

			LAYOUT_AddChild,						GUIObjects[ GID_Status ] = NewObject( ListBrowserClass, NULL,
				LISTBROWSER_ColumnInfo,				& StatusColumnInfo,
				LISTBROWSER_Labels,					& StatusList.vl_List,
				LISTBROWSER_Striping,				LBS_ROWS,
				LISTBROWSER_AutoFit,				TRUE,
//				LISTBROWSER_WrapText,				TRUE,
			End,
			CHILD_WeightedHeight,					25,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,					GUIObjects[ GID_StartServer ] = NewObject( ButtonClass, NULL,
					GA_ID,							GID_StartServer,
					GA_Text,						"Online",
					GA_Disabled,					( cfg->cfg_ServerStatus == PROCESS_Stopped ) ? FALSE : TRUE,
					GA_RelVerify,					TRUE,
					BUTTON_Justification,			BCJ_CENTER,
				End,
				CHILD_WeightedWidth,				30,

				LAYOUT_AddChild,					GUIObjects[ GID_NeedTab ] = NewObject( ClickTabClass, NULL,
					CLICKTAB_Current,				( cfg->cfg_SettingChanged ) ? 1 : 0,
					CLICKTAB_PageGroup,				GUIObjects[ GID_NeedPage ] = NewObject( PageClass, NULL,
						PAGE_Add,					Page_Empty( cfg ),
						PAGE_Add,					Page_NeedUpdate( cfg ),
					End,
				End,
				CHILD_WeightedWidth,				40,

				LAYOUT_AddChild,					GUIObjects[ GID_StopServer ] = NewObject( ButtonClass, NULL,
					GA_ID,							GID_StopServer,
					GA_Text,						"Offline",
					GA_Disabled,					( cfg->cfg_ServerStatus == PROCESS_Running ) ? FALSE : TRUE,
					GA_RelVerify,					TRUE,
					BUTTON_Justification,			BCJ_CENTER,
				End,
				CHILD_WeightedWidth,				30,
			End,
			CHILD_WeightedHeight,					0,
		End,
	End;

	if ( ! GUIObjects[ GID_Window ] )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating Main GUI Object" );
		goto bailout;
	}

	if ( cfg->cfg_WinData[WIN_Main].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_Main].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_Main].WindowAdr == NULL )
		{
			goto bailout;
		}

//		cfg->cfg_WinData[WIN_Main].WindowAdr->UserData = (PTR) RAGUI_HandleMainWindow;
	}
	else
	{
		if ( IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ) == 0 )
		{
			goto bailout;
		}
	}

	error = FALSE;


bailout:

	return( error );
}

// --

void RAGUI_CloseMainWindow( struct Config *cfg UNUSED )
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

	DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_Main].WindowAdr = NULL;

	myGUI_DeleteMenus( cfg );

	if ( AslHandle )
	{
		FreeAslRequest( AslHandle );
		AslHandle = NULL;
	}

bailout:

	return;
}

// --

#ifdef DEBUG
void GUIFunc_UpdateServerStatus( struct Config *cfg, enum ProcessStat pstat, STR file UNUSED, U32 line UNUSED )
#else
void GUIFunc_UpdateServerStatus( struct Config *cfg, enum ProcessStat pstat )
#endif
{
S32 stat;

//	#ifdef DEBUG
//	DebugPrintF( "GUIFunc_UpdateServerStatus : %s : %ld :\n", file, line );
//	#endif

	if ( ! GUIObjects[ GID_Window ] )
	{
		goto bailout;
	}

	// --

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
//	stat = ( cfg->cfg_ServerStatus == PROCESS_Stopped ) ? FALSE : TRUE ;
	stat = ( pstat == PROCESS_Stopped ) ? FALSE : TRUE ;

	mySetTags( cfg, GUIObjects[ GID_StartServer ],
		GA_Disabled, stat,
		TAG_END
	);

	// --

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
//	stat = ( cfg->cfg_ServerStatus == PROCESS_Running ) ? FALSE : TRUE ;
	stat = ( pstat == PROCESS_Running ) ? FALSE : TRUE ;

	mySetTags( cfg, GUIObjects[ GID_StopServer ],
		GA_Disabled, stat,
		TAG_END
	);

	// --

//	if ( ActiveGUI.Check_Settings )
//	{
//		#ifdef DEBUG
//		ActiveGUI.Check_Settings( cfg, __FILE__, __LINE__ );
//		#else
//		ActiveGUI.Check_Settings( cfg );
//		#endif
//	}

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

void GUIFunc_LVSelect( struct Config *cfg UNUSED )
{
U32 cur;
U32 sel;

	cur = 0;

	GetAttrs( GUIObjects[ GID_LVPage ],
		PAGE_Current, & cur,
		TAG_END
	);

	sel = 0;

	GetAttrs( GUIObjects[ GID_LVSelect ],
		LISTBROWSER_Selected, & sel,
		TAG_END
	);

	if ( cur != sel )
	{
		// Try too change it
		mySetTags( cfg, GUIObjects[ GID_LVPage ],
			PAGE_Current, sel,
			TAG_END
		);

		// Verify it changed
		GetAttrs( GUIObjects[ GID_LVSelect ],
			LISTBROWSER_Selected, & sel,
			TAG_END
		);

		if ( cur != sel )
		{
			IDoMethod( GUIObjects[ GID_Window ], WM_RETHINK );
		}
	}

	ActivePage = sel;
}

// --

void GUIFunc_RefreshSettings( struct Config *cfg )
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

	mySetTags( cfg, GUIObjects[ GID_TileBufferKB ],
		INTEGER_Number,	cfg->cfg_Disk_Settings.TileBufferKB,
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

	mySetTags( cfg, GUIObjects[ GID_Send_Bell ],
		GA_Selected, ( cfg->cfg_Disk_Settings.SendBell ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Support_RRE ],
		GA_Selected, ( cfg->cfg_Disk_Settings.RRE ) ? 1 : 0,
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

	myGUI_Main_UpdateIPList( cfg );

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
		RefreshGadgets( (PTR) GUIObjects[ GID_Root ], cfg->cfg_WinData[WIN_Main].WindowAdr, NULL );
	}
}

// --

void GUIFunc_AddLogMsg( struct Config *cfg, struct CommandLogString *msg )
{
STR str;
PTR n;

//	DebugPrintF( "RA: GUIFunc_AddLogMsg\n" );

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
	n = AllocListBrowserNode( 3,
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
		List_AddTail( & StatusList, n );

		while( MAX_LogListNodes < StatusList.vl_Entries )
		{
			n = List_RemHead( & StatusList );
			FreeListBrowserNode( n );
		}
	}

	mySetTags( cfg, GUIObjects[ GID_Status ],
		LISTBROWSER_Labels,	& StatusList.vl_List,
		LISTBROWSER_Position, LBP_BOTTOM,
		LISTBROWSER_AutoFit, TRUE,
		TAG_END
	);
}

// --

void GUIFunc_RefreshMessage( struct Config *cfg, struct CommandRefresh *msg UNUSED )
{
	DebugPrintF( "RA: GUIFunc_RefreshMessage\n" );

	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	if ( ActiveGUI.Refresh_Settings )
	{
		ActiveGUI.Refresh_Settings( cfg );
	}
}

// --

void GUIFunc_RefreshStats( struct Config *cfg, struct CommandRefresh *msg UNUSED )
{
//	DebugPrintF( "RA: GUIFunc_RefreshStats\n" );

	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	Page_Refresh_Stats( cfg );
}

// --

static void myShowHideMenus( Object *m, S32 hide )
{
//Object *p;
//Object *c;

	if ( m == NULL )
	{
		goto bailout;
	}

	SetAttrs( m, MA_Hidden, hide, TAG_END );

	#if 0
	GetAttrs( m,
		MA_Parent, & p,
		TAG_END
	);

	while( p )
	{
		m = (PTR) IDoMethod( p, MM_NEXTCHILD, 0, m );

SHELLBUF_PRINTF( "m: %p\n", p );
SHELLBUF_PRINTF( "c: %p\n", m );

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
S32 hide;

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

	m = (PTR) IDoMethod( MainMenuStrip, MM_FINDID, 0, MENUID_User1 );

	myShowHideMenus( m, hide );
}

// --

void VARARGS68K Main_SetTags( struct Config *cfg, PTR object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_Main].WindowAdr )
	{
		SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_Main].WindowAdr,
			NULL,
			va_getlinearva( ap, PTR )
		);
	}
	else
	{
		SetAttrsA( object, va_getlinearva( ap, PTR ));
	}

	va_end( ap );
}

// --

#ifdef DEBUG
S32 GUIFunc_CheckSettings( struct Config *cfg, STR file UNUSED, U32 line UNUSED )
#else
S32 GUIFunc_CheckSettings( struct Config *cfg )
#endif
{
S32 changed;

//	DebugPrintF( "GUIFunc_CheckSettings :\n" );

	if (( cfg->cfg_ServerStatus == PROCESS_Starting )
	||	( cfg->cfg_ServerStatus == PROCESS_Running  ))
	{
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
				if ( ! Func_SameString( cfg->cfg_PointerFilename, cfg->cfg_PointerFilename2 ))
				{
					break;
				}
			}

			// Check other things

			changed = 0;
			break;
		}
	}
	else
	{
		changed = 0;
	}

	if ( cfg->cfg_SettingChanged != changed )
	{
		cfg->cfg_SettingChanged = changed;

		if ( GUIObjects[ GID_Window ] )
		{
			mySetTags( cfg, GUIObjects[ GID_NeedPage ],
				PAGE_Current, ( cfg->cfg_SettingChanged ) ? 1 : 0,
				TAG_END
			);

			IDoMethod( GUIObjects[ GID_Window ], WM_RETHINK );
		}
	}

	return( changed );
}

// --

static const GUIFunc GUIFuncs[GID_LAST] =
{
	[GID_StartServer]						= { .Function = GUIFunc_StartServer },
	[GID_StopServer]						= { .Function = GUIFunc_StopServer },
	[GID_LVSelect]							= { .Function = GUIFunc_LVSelect },
	[GID_Support_RichCursor]				= { .Function = GUIFunc_Support_RichCursor },
	[GID_Support_RRE]						= { .Function = GUIFunc_Support_RRE },
	[GID_Support_ZLib]						= { .Function = GUIFunc_Support_ZLib },
	[GID_Disable_Blanker]					= { .Function = GUIFunc_Disable_Blanker },
	[GID_Send_Clipboard]					= { .Function = GUIFunc_Send_Clipboard },
	[GID_Read_Clipboard]					= { .Function = GUIFunc_Read_Clipboard },
	[GID_Send_Bell]							= { .Function = GUIFunc_Send_Bell },
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
	[GID_TileBufferKB]						= { .Function = GUIFunc_TileBufferKB },
	[GID_Password]							= { .Function = GUIFunc_UpdatePassword },
	[GID_Actions_Program_Start_String]		= { .Function = GUIFunc_UpdateActionProgramStart },
	[GID_Actions_Program_Stop_String]		= { .Function = GUIFunc_UpdateActionProgramStop },
	[GID_Actions_Server_Start_String]		= { .Function = GUIFunc_UpdateActionServerStart },
	[GID_Actions_Server_Stop_String]		= { .Function = GUIFunc_UpdateActionServerStop },
	[GID_Actions_User_Connect_String]		= { .Function = GUIFunc_UpdateActionUserConnect },
	[GID_Actions_User_Disconnect_String]	= { .Function = GUIFunc_UpdateActionUserDisconnect },

					// ------------
					// -- IP List

	[GID_IPAdd]								= { .Function = GUIFunc_IPAdd },
	[GID_IPEdit]							= { .Function = GUIFunc_IPEdit },
	[GID_IPList]							= { .Function = GUIFunc_IPList },
	[GID_IPUp]								= { .Function = GUIFunc_IPUp },
	[GID_IPDown]							= { .Function = GUIFunc_IPDown },
	[GID_IPDelete]							= { .Function = GUIFunc_IPDelete },

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

void RAGUI_HandleMainWindow( struct Config *cfg )
{
void (*Func)(struct Config *);
U32 result;
S32 theend;
U16 code;
S32 id;

//	DebugPrintF( "RAGUI_HandleMainWindow\n" );

	theend = FALSE;

	while( GUIObjects[ GID_Window ] )
	{
		result = IDoMethod( GUIObjects[ GID_Window ], WM_HANDLEINPUT, & code );

		if ( result == WMHI_LASTMSG )
		{
			break;
		}

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
				if ( IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_Main].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_Main].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_Main].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
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
		RAGUI_CloseMainWindow( cfg );
	}
}

// --
// Note this is running in Main Task
// Called once pr. second

void RAGUI_TimerTick_Main( struct Config *cfg )
{
	if (( GUIObjects[ GID_Window ] )
	&&	( ActivePage == PageID_Server_Stats )
	&&	( cfg->cfg_WinData[WIN_Main].WindowAdr ))
	{
		// Refresh Server stats (online time)
		Page_Refresh_Stats( cfg );
	}
}

// --

#endif // GUI_RA
