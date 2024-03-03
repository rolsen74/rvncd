 
/*
 * Copyright (c) 2023-2024 Rene W. Olsen < renewolsen @ gmail . com >
 *
 * This software is released under the GNU General Public License, version 3.
 * For the full text of the license, please visit:
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * You can also find a copy of the license in the LICENSE file included with this software.
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

	GID_Program_Verbose,
	GID_Program_AutoStart,
	GID_Program_Disable_ARexx,
	GID_Program_Disable_CxBroker,
	GID_Program_Disable_GUI,
	GID_Program_Open_GUI,

	GID_Log_Program_Start,
	GID_Log_Program_Stop,
	GID_Log_Server_Start,
	GID_Log_Server_Stop,
	GID_Log_User_Connect,
	GID_Log_User_Disconnect,
	GID_Log_Infos,
	GID_Log_Warnings,
	GID_Log_Errors,

	GID_WhiteList,
	GID_WhiteDelete,
	GID_WhiteEdit,
	GID_WhiteDown,
	GID_WhiteUp,

	GID_BlackList,
	GID_BlackDelete,
	GID_BlackEdit,
	GID_BlackDown,
	GID_BlackUp,

	GID_Screen_DelayInteger,
	GID_Screen_DelayScroller,

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

	MENUID_Quit,
	MENUID_Load,
	MENUID_Save,
	MENUID_SaveAS,
	MENUID_SaveDefault,
	MENUID_Reset,
	MENUID_Last
};

// --

struct LVStruct
{
	const char *	Title;
};

static const struct LVStruct LVMain[] =
{
{ "Main" },
{ "Program" },
{ "Server" },
{ "Connection" },
{ "Actions" },
{ "Screen" },
{ "Stats" },
{ "Log" },
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

static struct List				MainListHeader;
static APTR						MainMenuStrip		= NULL;

static struct List				MainWhiteList;
static struct List				MainBlackList;

static struct List				StatusList;
static int						StatusListCount		= 0;

static char						WorkBuffer[MAX_ACTIONBUFFER];
static Object *					GUIObjects[GID_LAST];

static struct FileRequester	*	AslHandle			= NULL;

// --

static int  myGUI_CreateMenus( struct Config *cfg );
static void myGUI_DeleteMenus( struct Config *cfg );
static int  myGUI_MenuPick( struct Config *cfg );

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

static void Main_CheckSettings( struct Config *cfg )
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

		// Check other things

		changed = 0;
		break;
	}

update:

	if ( cfg->cfg_SettingChanged != changed )
	{
		cfg->cfg_SettingChanged = changed;

		mySetTags( cfg, GUIObjects[ GID_NeedPage ],
			PAGE_Current, ( cfg->cfg_SettingChanged ) ? 1 : 0,
			TAG_END
		);

		IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_RETHINK );
	}
}

// --

static Object *Page_Main( struct Config *cfg UNUSED )
{
Object *o;

	// Proberly show a logo here
	o = IIntuition->NewObject( LayoutClass, NULL,
	End;

	return( o );
}

// --

static const char *ActionPages[] =
{
	"Program",
	"Server",
	"User",
	NULL
};

static const char *ActionTypes[] =
{
	"Cmd",
	NULL
};

static Object *Page_Action_Programs( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Program Start",
			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Start_Enable ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,					GID_Actions_Program_Start_Enable,
					GA_Selected,			( cfg->cfg_ActionsProgramStartEnable ) ? 1 : 0,
					GA_RelVerify,			TRUE,
				End,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Start_Type ] = IIntuition->NewObject( ChooserClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsProgramStartEnable == 0,
					CHOOSER_LabelArray,		ActionTypes,
					CHOOSER_Selected,		0,
//					GA_RelVerify,			TRUE,
//					GA_Selected,			( DoVerbose ) ? 1 : 0,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Start_String ] = IIntuition->NewObject( StringClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsProgramStartEnable == 0,
					GA_ID,					GID_Actions_Program_Start_String,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					STRINGA_WorkBuffer,		WorkBuffer,
					STRINGA_MaxChars,		MAX_ACTIONBUFFER,
					STRINGA_TextVal,		ActionBuffer_ProgramStart,
				End,
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Program Stop",
			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Stop_Enable ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,					GID_Actions_Program_Stop_Enable,
					GA_Selected,			( cfg->cfg_ActionsProgramStopEnable ) ? 1 : 0,
					GA_RelVerify,			TRUE,
				End,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Stop_Type ] = IIntuition->NewObject( ChooserClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsProgramStopEnable == 0,
					CHOOSER_LabelArray,		ActionTypes,
					CHOOSER_Selected,		0,
//					GA_RelVerify,			TRUE,
//					GA_Selected,			( DoVerbose ) ? 1 : 0,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Stop_String ] = IIntuition->NewObject( StringClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsProgramStopEnable == 0,
					GA_ID,					GID_Actions_Program_Stop_String,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					STRINGA_WorkBuffer,		WorkBuffer,
					STRINGA_MaxChars,		MAX_ACTIONBUFFER,
					STRINGA_TextVal,		ActionBuffer_ProgramStop,
				End,
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

static Object *Page_Action_Server( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Server Start",
			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Server_Start_Enable ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,					GID_Actions_Server_Start_Enable,
					GA_Selected,			( cfg->cfg_ActionsServerStartEnable ) ? 1 : 0,
					GA_RelVerify,			TRUE,
				End,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Server_Start_Type ] = IIntuition->NewObject( ChooserClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsServerStartEnable == 0,
					CHOOSER_LabelArray,		ActionTypes,
					CHOOSER_Selected,		0,
//					GA_RelVerify,			TRUE,
//					GA_Selected,			( DoVerbose ) ? 1 : 0,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Server_Start_String ] = IIntuition->NewObject( StringClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsServerStartEnable == 0,
					GA_ID,					GID_Actions_Server_Start_String,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					STRINGA_WorkBuffer,		WorkBuffer,
					STRINGA_MaxChars,		MAX_ACTIONBUFFER,
					STRINGA_TextVal,		ActionBuffer_ServerStart,
				End,
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Server Stop",
			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Server_Stop_Enable ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,					GID_Actions_Server_Stop_Enable,
					GA_Selected,			( cfg->cfg_ActionsServerStopEnable ) ? 1 : 0,
					GA_RelVerify,			TRUE,
				End,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Server_Stop_Type ] = IIntuition->NewObject( ChooserClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsServerStopEnable == 0,
					CHOOSER_LabelArray,		ActionTypes,
					CHOOSER_Selected,		0,
//					GA_RelVerify,			TRUE,
//					GA_Selected,			( DoVerbose ) ? 1 : 0,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Server_Stop_String ] = IIntuition->NewObject( StringClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsServerStopEnable == 0,
					GA_ID,					GID_Actions_Server_Stop_String,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					STRINGA_WorkBuffer,		WorkBuffer,
					STRINGA_MaxChars,		MAX_ACTIONBUFFER,
					STRINGA_TextVal,		ActionBuffer_ServerStop,
				End,
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

static Object *Page_Action_User( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"User Connect",
			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Connect_Enable ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,					GID_Actions_User_Connect_Enable,
					GA_Selected,			( cfg->cfg_ActionsUserConnectEnable ) ? 1 : 0,
					GA_RelVerify,			TRUE,
				End,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Connect_Type ] = IIntuition->NewObject( ChooserClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsUserConnectEnable == 0,
					CHOOSER_LabelArray,		ActionTypes,
					CHOOSER_Selected,		0,
//					GA_RelVerify,			TRUE,
//					GA_Selected,			( DoVerbose ) ? 1 : 0,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Connect_String ] = IIntuition->NewObject( StringClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsUserConnectEnable == 0,
					GA_ID,					GID_Actions_User_Connect_String,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					STRINGA_WorkBuffer,		WorkBuffer,
					STRINGA_MaxChars,		MAX_ACTIONBUFFER,
					STRINGA_TextVal,		ActionBuffer_UserConnect,
				End,
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"User Disconnect",
			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Disconnect_Enable ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,					GID_Actions_User_Disconnect_Enable,
					GA_Selected,			( cfg->cfg_ActionsUserDisconnectEnable ) ? 1 : 0,
					GA_RelVerify,			TRUE,
				End,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Disconnect_Type ] = IIntuition->NewObject( ChooserClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsUserDisconnectEnable == 0,
					CHOOSER_LabelArray,		ActionTypes,
					CHOOSER_Selected,		0,
//					GA_RelVerify,			TRUE,
//					GA_Selected,			( DoVerbose ) ? 1 : 0,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Disconnect_String ] = IIntuition->NewObject( StringClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsUserDisconnectEnable == 0,
					GA_ID,					GID_Actions_User_Disconnect_String, 
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					STRINGA_WorkBuffer,		WorkBuffer,
					STRINGA_MaxChars,		MAX_ACTIONBUFFER,
					STRINGA_TextVal,		ActionBuffer_UserDisconnect,
				End,
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

static Object *Page_Actions( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( ClickTabClass, NULL,
			GA_Text,						ActionPages,
			CLICKTAB_Current,				0,
			CLICKTAB_PageGroup,				IIntuition->NewObject( PageClass, NULL,
				PAGE_Add,					Page_Action_Programs( cfg ),
				PAGE_Add,					Page_Action_Server( cfg ),
				PAGE_Add,					Page_Action_User( cfg ),
			End,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

static Object *Page_Stats( struct Config *cfg )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,				GUIObjects[ GID_Stats_Accecpted ] = IIntuition->NewObject( ButtonClass, NULL,
				GA_Text,					"------------",
				GA_ReadOnly,				TRUE,
				BUTTON_BevelStyle,			BVS_THIN,
				BUTTON_Integer,				cfg->Connect_Accecpted,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Accecpted",
			End,

			LAYOUT_AddChild,				GUIObjects[ GID_Stats_Rejected ] = IIntuition->NewObject( ButtonClass, NULL,
				GA_Text,					"------------",
				GA_ReadOnly,				TRUE,
				BUTTON_BevelStyle,			BVS_THIN,
				BUTTON_Integer,				cfg->Connect_Rejected,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Rejected",
			End,

			LAYOUT_AddChild,				GUIObjects[ GID_Stats_Whitelisted ] = IIntuition->NewObject( ButtonClass, NULL,
				GA_Text,					"------------",
				GA_ReadOnly,				TRUE,
				BUTTON_BevelStyle,			BVS_THIN,
				BUTTON_Integer,				cfg->Connect_WhiteListed,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Whitelisted",
			End,

			LAYOUT_AddChild,				GUIObjects[ GID_Stats_Blacklisted ] = IIntuition->NewObject( ButtonClass, NULL,
				GA_Text,					"------------",
				GA_ReadOnly,				TRUE,
				BUTTON_BevelStyle,			BVS_THIN,
				BUTTON_Integer,				cfg->Connect_BlackListed,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Blacklisted",
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

static struct TagItem DelaySliderTags[] =
{
{	SCROLLER_Top, INTEGER_Number },
{	TAG_END, 0 }
};

static struct TagItem DelayIntegerTags[] =
{
{	INTEGER_Number, SCROLLER_Top },
{	TAG_END, 0 }
};

static Object *Page_Screen( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,

		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,
				LAYOUT_AddChild,			GUIObjects[ GID_Screen_DelayInteger ] = IIntuition->NewObject( IntegerClass, NULL,
					GA_ID,					GID_Screen_DelayInteger,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					INTEGER_Arrows,			TRUE,
					INTEGER_MaxChars,		4,
					INTEGER_Minimum,		0,
					INTEGER_Maximum,		500,
					INTEGER_Number,			cfg->GfxRead_DelayFrequency,
				End,

				LAYOUT_AddChild,			GUIObjects[ GID_Screen_DelayScroller ] = IIntuition->NewObject( ScrollerClass, NULL,
					GA_ID,					GID_Screen_DelayScroller,
					SCROLLER_Top,			cfg->GfxRead_DelayFrequency,
					SCROLLER_Total,			500,
					SCROLLER_Visible,		1,
					SCROLLER_Orientation,	SORIENT_HORIZ,
					SCROLLER_Arrows,		FALSE,
				End,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Delay Frequency",
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
			End,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	if ( o )
	{
		mySetTags( cfg, GUIObjects[ GID_Screen_DelayInteger ],
			ICA_TARGET,	GUIObjects[ GID_Screen_DelayScroller ],
			ICA_MAP,	DelayIntegerTags,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Screen_DelayScroller ],
			ICA_TARGET,	GUIObjects[ GID_Screen_DelayInteger ],
			ICA_MAP,	DelaySliderTags,
			TAG_END
		);
	}

	return( o );
}

// --

static Object *Page_Program( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,		BVS_GROUP,

			LAYOUT_AddChild,		GUIObjects[ GID_Program_Verbose ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,				GID_Program_Verbose,
				GA_RelVerify,		TRUE,
				GA_Selected,		( DoVerbose ) ? 1 : 0,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Verbose",
			End,
		End,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,		BVS_GROUP,
			LAYOUT_Label,			"Program Startup",

			LAYOUT_AddChild,		GUIObjects[ GID_Program_AutoStart ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,				GID_Program_AutoStart,
				GA_RelVerify,		TRUE,
				GA_Selected,		( cfg->cfg_Disk_Settings2.AutoStart ) ? 1 : 0,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Server Autostart",
			End,

			LAYOUT_AddChild,		GUIObjects[ GID_Program_Disable_ARexx ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,				GID_Program_Disable_ARexx,
				GA_RelVerify,		TRUE,
				GA_Selected,		( cfg->cfg_ProgramDisableARexx ) ? 1 : 0,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Disable ARexx",
			End,

			LAYOUT_AddChild,		GUIObjects[ GID_Program_Disable_CxBroker ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,				GID_Program_Disable_CxBroker,
				GA_RelVerify,		TRUE,
				GA_Selected,		( cfg->cfg_ProgramDisableCxBroker ) ? 1 : 0,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Disable CxBroker",
			End,

			LAYOUT_AddChild,		GUIObjects[ GID_Program_Disable_GUI ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,				GID_Program_Disable_GUI,
				GA_RelVerify,		TRUE,
				GA_Selected,		( cfg->cfg_ProgramDisableGUI ) ? 1 : 0,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Disable GUI",
			End,
		End,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

static Object *Page_Server( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,		BVS_GROUP,
				LAYOUT_Label,			"Server",

				LAYOUT_AddChild,		GUIObjects[ GID_Port ] = IIntuition->NewObject( IntegerClass, NULL,
					GA_ID,				GID_Port,
					GA_RelVerify,		TRUE,
					GA_TabCycle,		TRUE,
					INTEGER_Minimum,	0,
					INTEGER_Maximum,	65535,
					INTEGER_Number,		cfg->cfg_Disk_Settings.Port,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Port",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_ServerName ] = IIntuition->NewObject( StringClass, NULL,
					GA_ID,				GID_ServerName,
					GA_RelVerify,		TRUE,
					GA_TabCycle,		TRUE,
					STRINGA_WorkBuffer,	WorkBuffer,
					STRINGA_MaxChars,	MAX_SERVERNAME + 1,
					STRINGA_TextVal,	cfg->cfg_Disk_Settings.Name,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Name",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Password ] = IIntuition->NewObject( StringClass, NULL,
					GA_ID,				GID_Password,
					GA_RelVerify,		TRUE,
					GA_TabCycle,		TRUE,
					STRINGA_WorkBuffer,	WorkBuffer,
					STRINGA_MaxChars,	MAX_SERVERPASSWORD + 1,
					STRINGA_TextVal,	cfg->cfg_Disk_Settings.Password,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Password",
				End,
			End,

			LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,		LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,		BVS_GROUP,

					LAYOUT_AddChild,		GUIObjects[ GID_Disable_Blanker ] = IIntuition->NewObject( CheckBoxClass, NULL,
						GA_ID,				GID_Disable_Blanker,
						GA_RelVerify,		TRUE,
						GA_Selected,		( cfg->cfg_Disk_Settings.DisableBlanker ) ? 1 : 0,
					End,
					CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,			"Disable Blanker",
					End,

					LAYOUT_AddChild,		GUIObjects[ GID_Send_Clipboard ] = IIntuition->NewObject( CheckBoxClass, NULL,
						GA_ID,				GID_Send_Clipboard,
						GA_RelVerify,		TRUE,
						GA_Selected,		( cfg->cfg_Disk_Settings.SendClipboard ) ? 1 : 0,
					End,
					CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,			"Send Clipboard",
					End,

					LAYOUT_AddChild,		GUIObjects[ GID_Read_Clipboard ] = IIntuition->NewObject( CheckBoxClass, NULL,
						GA_ID,				GID_Read_Clipboard,
						GA_RelVerify,		TRUE,
						GA_Selected,		( cfg->cfg_Disk_Settings.ReadClipboard ) ? 1 : 0,
					End,
					CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,			"Read Clipboard",
					End,

					LAYOUT_AddChild,		IIntuition->NewObject( LayoutClass, NULL,
					End,
				End,

				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,		BVS_GROUP,
					LAYOUT_Label,			"Server Support",

					LAYOUT_AddChild,		GUIObjects[ GID_Support_ZLib ] = IIntuition->NewObject( CheckBoxClass, NULL,
						GA_ID,				GID_Support_ZLib,
						GA_RelVerify,		TRUE,
						GA_Selected,		( cfg->cfg_Disk_Settings.ZLib ) ? 1 : 0,
					End,
					CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,			"ZLib",
					End,

					LAYOUT_AddChild,		GUIObjects[ GID_Support_RichCursor ] = IIntuition->NewObject( CheckBoxClass, NULL,
						GA_ID,				GID_Support_RichCursor,
						GA_RelVerify,		TRUE,
						GA_Selected,		( cfg->cfg_Disk_Settings.RichCursor ) ? 1 : 0,
					End,
					CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,			"Rich Cursor",
					End,

					LAYOUT_AddChild,		IIntuition->NewObject( LayoutClass, NULL,
					End,
				End,
			End,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

#if 0
[Server]
; Port = 12345
 Name = "Rene's VNC Server"
 EncZLIB = 1
 GfxRead_TileSize = 64
 ScanMethod = 0
 DisableBlanker = 1
 DelayFrequency = 150
 SendClipboard = 1
 ReadClipboard = 1
; AutoStart = 0
; Password = "123456"
; PointerFile = "TBImages:picturegrey"
#endif

	return( o );
}

// --

static Object *Page_Log( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
		End,

#if 0
	uint8					cfg_LogEnable;
	uint8					cfg_LogProgramStart;
	uint8					cfg_LogProgramStop;
	uint8					cfg_LogServerStart;
	uint8					cfg_LogServerStop;
	uint8					cfg_LogUserConnect;
#endif

		LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,

			LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,		BVS_GROUP,
				LAYOUT_Label,			"Log Events",

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Program_Start ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Program_Start,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogProgramStart ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Program Start",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Program_Stop ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Program_Stop,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogProgramStop ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Program Stop",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Server_Start ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Server_Start,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogServerStart ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Server Start",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Server_Stop ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Server_Stop,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogServerStart ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Server Stop",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_User_Connect ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_User_Connect,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogUserConnect ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"User Connect",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_User_Disconnect ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_User_Disconnect,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogUserDisconnect ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"User Disconnect",
				End,
			End,

			LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,		BVS_GROUP,
				LAYOUT_Label,			"Log Types",

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Infos ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Infos,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_ProgramLogInfos ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Infos",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Warnings ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Warnings,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_ProgramLogWarnings ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Warnings",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Errors ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Errors,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_ProgramLogErrors ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Errors",
				End,

				LAYOUT_AddChild,		IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,
		End,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

#if 0
[Log]
; LogFile = "Logdata"
; Enable = 0
 ProgramStart = 0
 ProgramStop = 0
 ServerStart = 1
 ServerStop = 1
 UserConnect = 1
#endif

	return( o );
}

// --

static void myGUI_UpdateWhiteList( struct Config *cfg )
{
struct IPNode *node;
struct Node *n;
char str[64];
int len;

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	node = (APTR) IExec->GetHead( & cfg->WhiteList );

	while( node )
	{
		str[0] = 0;
		len = 0;

		if ( node->ipn_A == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", node->ipn_A );
		}

		len = strlen( str );

		if ( node->ipn_B == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", node->ipn_B );
		}

		len = strlen( str );

		if ( node->ipn_C == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", node->ipn_C );
		}

		len = strlen( str );

		if ( node->ipn_D == -1 )
		{
			sprintf( & str[len], "*" );
		}
		else
		{
			sprintf( & str[len], "%d", node->ipn_D );
		}

		#if 0
		if ( cfgnr < 0 )
		{
			// Log_PrintF( "[WhiteList] Adding IP %s", str );
		}
		else
		{
			// Log_PrintF( "[WhiteList] Adding IP %s #%d", str, cfgnr );
		}
		#endif


		n = IListBrowser->AllocListBrowserNode( 1,
			LBNA_Column, 0,
				LBNCA_CopyText, TRUE,
				LBNCA_Text, str,
			TAG_END
		);

		if ( n )
		{
			n->ln_Name = (APTR) node;
			IExec->AddTail( & MainWhiteList, n );
		}

		node = (APTR) IExec->GetSucc( (APTR) node );
	}

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, & MainWhiteList,
		TAG_END
	);
}

// --

static void myGUI_UpdateBlackList( struct Config *cfg )
{
struct IPNode *node;
struct Node *n;
char str[64];
int len;

	mySetTags( cfg, GUIObjects[ GID_BlackList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	node = (APTR) IExec->GetHead( & cfg->BlackList );

	while( node )
	{
		str[0] = 0;
		len = 0;

		if ( node->ipn_A == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", node->ipn_A );
		}

		len = strlen( str );

		if ( node->ipn_B == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", node->ipn_B );
		}

		len = strlen( str );

		if ( node->ipn_C == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", node->ipn_C );
		}

		len = strlen( str );

		if ( node->ipn_D == -1 )
		{
			sprintf( & str[len], "*" );
		}
		else
		{
			sprintf( & str[len], "%d", node->ipn_D );
		}

		n = IListBrowser->AllocListBrowserNode( 1,
			LBNA_Column, 0,
				LBNCA_CopyText, TRUE,
				LBNCA_Text, str,
			TAG_END
		);

		if ( n )
		{
			n->ln_Name = (APTR) node;
			IExec->AddTail( & MainBlackList, n );
		}

		node = (APTR) IExec->GetSucc( (APTR) node );
	}

	mySetTags( cfg, GUIObjects[ GID_BlackList ],
		LISTBROWSER_Labels, & MainBlackList,
		TAG_END
	);
}

// --

static Object *Page_Connection( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,					BVS_GROUP,
			LAYOUT_Label,						"White List",

			LAYOUT_AddChild,					GUIObjects[ GID_WhiteList ] = IIntuition->NewObject( ListBrowserClass, NULL,
				GA_ID,							GID_WhiteList,
				GA_RelVerify,					TRUE,
				LISTBROWSER_ShowSelected,		TRUE,
				LISTBROWSER_AutoFit,			TRUE,
//				LISTBROWSER_Labels,				& MainListHeader,
				LISTBROWSER_MinVisible,			5,
				LISTBROWSER_Striping,			LBS_ROWS,
			End,

			LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,				LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				End,

				LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_EvenSize,			TRUE,
					LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,

					LAYOUT_AddChild,			IIntuition->NewObject( ButtonClass, NULL,
//						GA_ID,					GID_StartServer,
						GA_Text,				"Add",
						GA_Disabled,			TRUE,
//						GA_RelVerify,			TRUE,
//						BUTTON_Justification,	BCJ_CENTER,
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_WhiteEdit ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ID,					GID_WhiteEdit,
						GA_Text,				"Edit",
						GA_Disabled,			TRUE,
						GA_RelVerify,			TRUE,
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_WhiteDelete ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ID,					GID_WhiteDelete,
						GA_Text,				"Delete",
						GA_Disabled,			TRUE,
						GA_RelVerify,			TRUE,
					End,
				End,

				LAYOUT_AddChild,				GUIObjects[ GID_WhiteUp ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,						GID_WhiteUp,
					GA_Disabled,				TRUE,
					GA_RelVerify,				TRUE,
					BUTTON_AutoButton,			BAG_UPARROW,
				End,
				CHILD_WeightedWidth,			0,

				LAYOUT_AddChild,				GUIObjects[ GID_WhiteDown ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,						GID_WhiteDown,
					GA_Disabled,				TRUE,
					GA_RelVerify,				TRUE,
					BUTTON_AutoButton,			BAG_DNARROW,
				End,
				CHILD_WeightedWidth,			0,
			End,
		End,

		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,					BVS_GROUP,
			LAYOUT_Label,						"Black List",

			LAYOUT_AddChild,					GUIObjects[ GID_BlackList ] = IIntuition->NewObject( ListBrowserClass, NULL,
				GA_ID,							GID_BlackList,
				GA_RelVerify,					TRUE,
				LISTBROWSER_ShowSelected,		TRUE,
				LISTBROWSER_AutoFit,			TRUE,
//				LISTBROWSER_Labels,				& MainListHeader,
				LISTBROWSER_MinVisible,			5,
				LISTBROWSER_Striping,			LBS_ROWS,
			End,

			LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,				LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				End,

				LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_EvenSize,			TRUE,
					LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,

					LAYOUT_AddChild,			IIntuition->NewObject( ButtonClass, NULL,
//						GA_ID,					GID_StartServer,
						GA_Text,				"Add",
						GA_Disabled,			TRUE,
//						GA_RelVerify,			TRUE,
//						BUTTON_Justification,	BCJ_CENTER,
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_BlackEdit ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ID,					GID_BlackEdit,
						GA_Text,				"Edit",
						GA_Disabled,			TRUE,
						GA_RelVerify,			TRUE,
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_BlackDelete ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ID,					GID_BlackDelete,
						GA_Text,				"Delete",
						GA_Disabled,			TRUE,
						GA_RelVerify,			TRUE,
					End,

				End,

				LAYOUT_AddChild,				GUIObjects[ GID_BlackUp ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,						GID_BlackUp,
					GA_Disabled,				TRUE,
					GA_RelVerify,				TRUE,
					BUTTON_AutoButton,			BAG_UPARROW,
				End,
				CHILD_WeightedWidth,			0,

				LAYOUT_AddChild,				GUIObjects[ GID_BlackDown ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,						GID_BlackDown,
					GA_Disabled,				TRUE,
					GA_RelVerify,				TRUE,
					BUTTON_AutoButton,			BAG_DNARROW,
				End,
				CHILD_WeightedWidth,			0,
			End,
		End,

		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	if ( o )
	{
		myGUI_UpdateWhiteList( cfg );
		myGUI_UpdateBlackList( cfg );
	}

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
		WINDOW_IconifyGadget,						TRUE,
		WINDOW_IconTitle,							"RVNCd",
		WINDOW_MenuStrip,					    	MainMenuStrip,
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
						PAGE_Add,					Page_Server( cfg ),
						PAGE_Add,					Page_Connection( cfg ),
						PAGE_Add,					Page_Actions( cfg ),
						PAGE_Add,					Page_Screen( cfg ),
						PAGE_Add,					Page_Stats( cfg ),
						PAGE_Add,					Page_Log( cfg ),
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

	if ( GUIObjects[ GID_Window ] == NULL )
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

	if ( GUIObjects[ GID_Window ] == NULL )
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

static void GUIFunc_Support_RichCursor( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Support_RichCursor ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.RichCursor = ( val ) ? 1 : 0 ;

	// --
	Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Support_ZLib( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Support_ZLib ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.ZLib = ( val ) ? 1 : 0 ;

	// --
	Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Disable_Blanker( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Disable_Blanker ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.DisableBlanker = ( val ) ? 1 : 0 ;

	// --
	Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Send_Clipboard( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Send_Clipboard ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.SendClipboard = ( val ) ? 1 : 0 ;

	// --
	Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Read_Clipboard( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Read_Clipboard ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.ReadClipboard = ( val ) ? 1 : 0 ;

	// --
	Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Port( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Port ],
		INTEGER_Number, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.Port = val;

	// --
	Main_CheckSettings( cfg );
}

// --

static void GUIFunc_AutoStart( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_AutoStart ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings2.AutoStart = ( val ) ? 1 : 0 ;

	// --
	Main_CheckSettings( cfg );
}

// --

void GUIFunc_UpdateServerStatus( struct Config *cfg )
{
int stat;

	if ( GUIObjects[ GID_Window ] == NULL )
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

	Main_CheckSettings( cfg );

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

static void GUIFunc_Verbose( struct Config *cfg UNUSED )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_Verbose ],
		GA_Selected, & val,
		TAG_END
	);

	DoVerbose = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_ARexx( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_Disable_ARexx ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramDisableARexx = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_Broker( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_Disable_CxBroker ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramDisableCxBroker = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_GUI( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_Disable_GUI ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramDisableGUI = ( val ) ? 1 : 0 ;
}

// --

static void myGUI_RefreshSettings( struct Config *cfg )
{
	// -- Program

	mySetTags( cfg, GUIObjects[ GID_Program_Verbose ],
		GA_Selected, ( DoVerbose ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Program_AutoStart ],
		GA_Selected, ( cfg->cfg_Disk_Settings2.AutoStart ) ? 1 : 0,
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

	myGUI_UpdateWhiteList( cfg );
	myGUI_UpdateBlackList( cfg );

	// -- Screen

	// -- Log

	mySetTags( cfg, GUIObjects[ GID_Log_Program_Start ],
		GA_Selected, ( cfg->cfg_LogProgramStart ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Program_Stop ],
		GA_Selected, ( cfg->cfg_LogProgramStop ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Server_Start ],
		GA_Selected, ( cfg->cfg_LogServerStart ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Server_Stop ],
		GA_Selected, ( cfg->cfg_LogServerStop ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_User_Connect ],
		GA_Selected, ( cfg->cfg_LogUserConnect ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Infos ],
		GA_Selected, ( cfg->cfg_ProgramLogInfos ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Warnings ],
		GA_Selected, ( cfg->cfg_ProgramLogWarnings ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Errors ],
		GA_Selected, ( cfg->cfg_ProgramLogErrors ) ? 1 : 0,
		TAG_END
	);

	// --

	if ( cfg->cfg_WinData[WIN_Main].WindowAdr )
	{
		IIntuition->RefreshGadgets( (APTR) GUIObjects[ GID_Root ], cfg->cfg_WinData[WIN_Main].WindowAdr, NULL );
	}
}

// --

static void GUIFunc_ProgramStart( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Program_Start ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogProgramStart = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_ProgramStop( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Program_Stop ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogProgramStop = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_ServerStart( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Server_Start ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogServerStart = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_ServerStop( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Server_Stop ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogServerStop = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_UserConnect( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_User_Connect ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogUserConnect = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_UserDisconnect( struct Config *cfg )
{
uint32 val;

	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_User_Disconnect ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogUserDisconnect = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_Infos( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Infos ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramLogInfos = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_Warnings( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Warnings ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramLogWarnings = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_Errors( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Errors ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramLogErrors = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_WhiteList( struct Config *cfg )
{
struct Node *node;
uint32 relevent;
uint32 total;
uint32 pos;

	relevent = 0;
	total = 0;
	node = NULL;
	pos = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_WhiteList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_TotalNodes, & total,
		LISTBROWSER_RelEvent, & relevent,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteDelete ],
		GA_Disabled, ( node == NULL ) ? TRUE : FALSE,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteEdit ],
		GA_Disabled, ( node == NULL ) ? TRUE : FALSE,
		TAG_END
	);

	if ( node == NULL )
	{
		mySetTags( cfg, GUIObjects[ GID_WhiteUp ],
			GA_Disabled, TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_WhiteDown ],
			GA_Disabled, TRUE,
			TAG_END
		);
	}
	else
	{
		mySetTags( cfg, GUIObjects[ GID_WhiteUp ],
			GA_Disabled, ( pos != 0 ) ? FALSE : TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_WhiteDown ],
			GA_Disabled, ( pos < total - 1 ) ? FALSE : TRUE,
			TAG_END
		);
	}
}

// --

static void GUIFunc_WhiteUp( struct Config *cfg )
{
struct Node *prev;
struct Node *node;
struct Node *prev2;
struct Node *node2;
uint32 pos;

	node = NULL;
	pos = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_WhiteList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	if ( node == NULL )
	{
		goto bailout;
	}

	prev = IExec->GetPred( node );

	if ( prev == NULL )
	{
		goto bailout;
	}

	node2 = (APTR) node->ln_Name;
	prev2 = (APTR) prev->ln_Name;

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	// Change LB list
	IExec->Remove( prev );
	IExec->Insert( & MainWhiteList, prev, node );

	// Change IP list
	IExec->Remove( prev2 );
	IExec->Insert( & cfg->WhiteList, prev2, node2 );

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, & MainWhiteList,
		LISTBROWSER_MakeVisible, pos - 1,
		LISTBROWSER_Selected, pos - 1,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteUp ],
		GA_Disabled, ( IExec->GetPred( node ) == 0 ),
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteDown ],
		GA_Disabled, ( IExec->GetSucc( node ) == 0 ),
		TAG_END
	);

bailout:

	return;
}

// --

static void GUIFunc_WhiteDown( struct Config *cfg )
{
struct Node *node;
struct Node *next;
struct Node *node2;
struct Node *next2;
uint32 pos;

	node = NULL;
	pos = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_WhiteList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	if ( node == NULL )
	{
		goto bailout;
	}

	next = IExec->GetSucc( node );

	if ( next == NULL )
	{
		goto bailout;
	}

	node2 = (APTR) node->ln_Name;
	next2 = (APTR) next->ln_Name;

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	// Change LB list
	IExec->Remove( node );
	IExec->Insert( & MainWhiteList, node, next );

	// Change IP list
	IExec->Remove( node2 );
	IExec->Insert( & cfg->WhiteList, node2, next2 );

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, & MainWhiteList,
		LISTBROWSER_MakeVisible, pos + 1,
		LISTBROWSER_Selected, pos + 1,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteUp ],
		GA_Disabled, ( IExec->GetPred( node ) == 0 ),
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteDown ],
		GA_Disabled, ( IExec->GetSucc( node ) == 0 ),
		TAG_END
	);

bailout:

	return;
}

// --

static void GUIFunc_BlackList( struct Config *cfg )
{
struct Node *node;
uint32 relevent;
uint32 total;
uint32 pos;

	relevent = 0;
	total = 0;
	node = NULL;
	pos = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_BlackList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_TotalNodes, & total,
		LISTBROWSER_RelEvent, & relevent,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_BlackDelete ],
		GA_Disabled, ( node == NULL ) ? TRUE : FALSE,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_BlackEdit ],
		GA_Disabled, ( node == NULL ) ? TRUE : FALSE,
		TAG_END
	);

	if ( node == NULL )
	{
		mySetTags( cfg, GUIObjects[ GID_BlackUp ],
			GA_Disabled, TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_BlackDown ],
			GA_Disabled, TRUE,
			TAG_END
		);
	}
	else
	{
		mySetTags( cfg, GUIObjects[ GID_BlackUp ],
			GA_Disabled, ( pos != 0 ) ? FALSE : TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_BlackDown ],
			GA_Disabled, ( pos < total - 1 ) ? FALSE : TRUE,
			TAG_END
		);
	}
}

// --

static void GUIFunc_BlackUp( struct Config *cfg )
{
struct Node *prev;
struct Node *node;
struct Node *prev2;
struct Node *node2;
uint32 pos;

	node = NULL;
	pos = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_BlackList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	if ( node == NULL )
	{
		goto bailout;
	}

	prev = IExec->GetPred( node );

	if ( prev == NULL )
	{
		goto bailout;
	}

	node2 = (APTR) node->ln_Name;
	prev2 = (APTR) prev->ln_Name;

	mySetTags( cfg, GUIObjects[ GID_BlackList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	// Change LB list
	IExec->Remove( prev );
	IExec->Insert( & MainBlackList, prev, node );

	// Change IP list
	IExec->Remove( prev2 );
	IExec->Insert( & cfg->BlackList, prev2, node2 );

	mySetTags( cfg, GUIObjects[ GID_BlackList ],
		LISTBROWSER_Labels, & MainBlackList,
		LISTBROWSER_MakeVisible, pos - 1,
		LISTBROWSER_Selected, pos - 1,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_BlackUp ],
		GA_Disabled, ( IExec->GetPred( node ) == 0 ),
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_BlackDown ],
		GA_Disabled, ( IExec->GetSucc( node ) == 0 ),
		TAG_END
	);

bailout:

	return;
}

// --

static void GUIFunc_BlackDown( struct Config *cfg )
{
struct Node *node;
struct Node *next;
struct Node *node2;
struct Node *next2;
uint32 pos;

	node = NULL;
	pos = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_BlackList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	if ( node == NULL )
	{
		goto bailout;
	}

	next = IExec->GetSucc( node );

	if ( next == NULL )
	{
		goto bailout;
	}

	node2 = (APTR) node->ln_Name;
	next2 = (APTR) next->ln_Name;

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	// Change LB list
	IExec->Remove( node );
	IExec->Insert( & MainBlackList, node, next );

	// Change IP list
	IExec->Remove( node2 );
	IExec->Insert( & cfg->BlackList, node2, next2 );

	mySetTags( cfg, GUIObjects[ GID_BlackList ],
		LISTBROWSER_Labels, & MainBlackList,
		LISTBROWSER_MakeVisible, pos + 1,
		LISTBROWSER_Selected, pos + 1,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_BlackUp ],
		GA_Disabled, ( IExec->GetPred( node ) == 0 ),
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_BlackDown ],
		GA_Disabled, ( IExec->GetSucc( node ) == 0 ),
		TAG_END
	);

bailout:

	return;
}

// --

static void GUIFunc_UpdateDelay( struct Config *cfg )
{
uint32 val;

	IIntuition->GetAttrs( GUIObjects[ GID_Screen_DelayInteger ],
		INTEGER_Number, & val,
		TAG_END
	);

	cfg->GfxRead_DelayFrequency = val;
}


// --

static void GUIFunc_UpdatePassword( struct Config *cfg )
{
char *str;

	str = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_Password ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( cfg->cfg_Disk_Settings.Password, str, MAX_SERVERPASSWORD );		
	}
}

// --

static void GUIFunc_Actions_Program_Start_Enable( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_Program_Start_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ActionsProgramStartEnable = ( val ) ? 1 : 0 ;

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Start_Type ],
		GA_Disabled, val == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Start_String ],
		GA_Disabled, val == 0,
		TAG_END
	);
}

// --

static void GUIFunc_Actions_Program_Stop_Enable( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_Program_Stop_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ActionsProgramStopEnable = ( val ) ? 1 : 0 ;

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Stop_Type ],
		GA_Disabled, val == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Stop_String ],
		GA_Disabled, val == 0,
		TAG_END
	);
}

// --

static void GUIFunc_Actions_Server_Start_Enable( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_Server_Start_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ActionsServerStartEnable = ( val ) ? 1 : 0 ;

	mySetTags( cfg, GUIObjects[ GID_Actions_Server_Start_Type ],
		GA_Disabled, val == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Server_Start_String ],
		GA_Disabled, val == 0,
		TAG_END
	);
}

// --

static void GUIFunc_Actions_Server_Stop_Enable( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_Server_Stop_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ActionsServerStopEnable = ( val ) ? 1 : 0 ;

	mySetTags( cfg, GUIObjects[ GID_Actions_Server_Stop_Type ],
		GA_Disabled, val == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Server_Stop_String ],
		GA_Disabled, val == 0,
		TAG_END
	);
}

// --

static void GUIFunc_Actions_User_Connect_Enable( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_User_Connect_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ActionsUserConnectEnable = ( val ) ? 1 : 0 ;

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Connect_Type ],
		GA_Disabled, val == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Connect_String ],
		GA_Disabled, val == 0,
		TAG_END
	);
}

// --

static void GUIFunc_Actions_User_Disconnect_Enable( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_User_Disconnect_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ActionsUserDisconnectEnable = ( val ) ? 1 : 0 ;

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Disconnect_Type ],
		GA_Disabled, val == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Disconnect_String ],
		GA_Disabled, val == 0,
		TAG_END
	);
}

// --

static void GUIFunc_UpdateServerName( struct Config *cfg )
{
char *str;

	str = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_ServerName ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( cfg->cfg_Disk_Settings.Name, str, MAX_SERVERNAME );		
	}
}

// --

static void GUIFunc_UpdateActionProgramStart( struct Config *cfg UNUSED )
{
char *str;

	str = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_Program_Start_String ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( ActionBuffer_ProgramStart, str, MAX_ACTIONBUFFER-1 );
	}
}

// --

static void GUIFunc_UpdateActionProgramStop( struct Config *cfg UNUSED )
{
char *str;

	str = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_Program_Stop_String ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( ActionBuffer_ProgramStop, str, MAX_ACTIONBUFFER-1 );
	}
}

// --

static void GUIFunc_UpdateActionServerStart( struct Config *cfg UNUSED )
{
char *str;

	str = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_Server_Start_String ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( ActionBuffer_ServerStart, str, MAX_ACTIONBUFFER-1 );
	}
}

// --

static void GUIFunc_UpdateActionServerStop( struct Config *cfg UNUSED )
{
char *str;

	str = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_Server_Stop_String ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( ActionBuffer_ServerStop, str, MAX_ACTIONBUFFER-1 );
	}
}

// --

static void GUIFunc_UpdateActionUserConnect( struct Config *cfg UNUSED )
{
char *str;

	str = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_User_Connect_String ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( ActionBuffer_UserConnect, str, MAX_ACTIONBUFFER-1 );
	}
}

// --

static void GUIFunc_UpdateActionUserDisconnect( struct Config *cfg UNUSED )
{
char *str;

	str = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_Actions_User_Disconnect_String ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( ActionBuffer_UserDisconnect, str, MAX_ACTIONBUFFER-1 );
	}
}

// --

static void myMenu_SaveAS( struct Config *cfg UNUSED )
{
char *filename;
BOOL stat;
int len;
//int err;

	filename = NULL;

	mySetTags( cfg, GUIObjects[ GID_Window ],
		WA_BusyPointer, TRUE,
		TAG_END
	);

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
				TDR_TitleString, "RVNCd",
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
				filename = myASPrintF( "%s%s", AslHandle->fr_Drawer, AslHandle->fr_File );
			}
			else
			{
				filename = myASPrintF( "%s/%s", AslHandle->fr_Drawer, AslHandle->fr_File );
			}
		}
		else
		{
			filename = myASPrintF( "%s", AslHandle->fr_File );
		}

		if ( filename == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
			goto bailout;
		}

		Config_Save( cfg, filename );
	}

bailout:

	mySetTags( cfg, GUIObjects[ GID_Window ],
		WA_BusyPointer, FALSE,
		TAG_END
	);

	if ( filename )
	{
		myFree( filename );
	}

	return;
}

// --

static void myMenu_Load( struct Config *cfg )
{
char *filename;
BOOL stat;
int len;
int err;

	filename = NULL;

	mySetTags( cfg, GUIObjects[ GID_Window ],
		WA_BusyPointer, TRUE,
		TAG_END
	);

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
				TDR_TitleString, "RVNCd",
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
				filename = myASPrintF( "%s%s", AslHandle->fr_Drawer, AslHandle->fr_File );
			}
			else
			{
				filename = myASPrintF( "%s/%s", AslHandle->fr_Drawer, AslHandle->fr_File );
			}
		}
		else
		{
			filename = myASPrintF( "%s", AslHandle->fr_File );
		}

		if ( filename == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
			goto bailout;
		}

		err = Config_ParseFile( cfg, filename );

		if ( err )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Failed to load '%s' Config file", IDOS->FilePart( filename ));
		}
		else
		{
			myGUI_RefreshSettings( cfg );
			Log_PrintF( cfg, LOGTYPE_Info, "Config file '%s' loaded", IDOS->FilePart( filename ) );
		}
	}

bailout:

	mySetTags( cfg, GUIObjects[ GID_Window ],
		WA_BusyPointer, FALSE,
		TAG_END
	);

	if ( filename )
	{
		myFree( filename );
	}

	return;
}

// --

void myGUI_HandleMainWindow( struct Config *cfg )
{
uint32 result;
uint16 code;
BOOL theend;

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
				switch( result & WMHI_GADGETMASK )
				{
					case GID_StartServer:
					{
						GUIFunc_StartServer( cfg );
						break;
					}

					case GID_StopServer:
					{
						GUIFunc_StopServer( cfg );
						break;
					}

					case GID_LVSelect:
					{
						GUIFunc_LVSelect( cfg );
						break;
					}

					case GID_Support_RichCursor:
					{
						GUIFunc_Support_RichCursor( cfg );
						break;
					}

					case GID_Support_ZLib:
					{
						GUIFunc_Support_ZLib( cfg );
						break;
					}

					case GID_Disable_Blanker:
					{
						GUIFunc_Disable_Blanker( cfg );
						break;
					}

					case GID_Send_Clipboard:
					{
						GUIFunc_Send_Clipboard( cfg );
						break;
					}

					case GID_Read_Clipboard:
					{
						GUIFunc_Read_Clipboard( cfg );
						break;
					}

					case GID_Port:
					{
						GUIFunc_Port( cfg );
						break;
					}

					case GID_Program_AutoStart:
					{
						GUIFunc_AutoStart( cfg );
						break;
					}

					case GID_Program_Verbose:
					{
						GUIFunc_Verbose( cfg );
						break;
					}

					case GID_Program_Disable_ARexx:
					{
						GUIFunc_ARexx( cfg );
						break;
					}

					case GID_Program_Disable_CxBroker:
					{
						GUIFunc_Broker( cfg );
						break;
					}

					case GID_Program_Disable_GUI:
					{
						GUIFunc_GUI( cfg );
						break;
					}

					case GID_Log_Program_Start:
					{
						GUIFunc_ProgramStart( cfg );
						break;
					}

					case GID_Log_Program_Stop:
					{
						GUIFunc_ProgramStop( cfg );
						break;
					}

					case GID_Log_Server_Start:
					{
						GUIFunc_ServerStart( cfg );
						break;
					}

					case GID_Log_Server_Stop:
					{
						GUIFunc_ServerStop( cfg );
						break;
					}

					case GID_Log_User_Connect:
					{
						GUIFunc_UserConnect( cfg );
						break;
					}

					case GID_Log_User_Disconnect:
					{
						GUIFunc_UserDisconnect( cfg );
						break;
					}

					case GID_Log_Infos:
					{
						GUIFunc_Infos( cfg );
						break;
					}

					case GID_Log_Warnings:
					{
						GUIFunc_Warnings( cfg );
						break;
					}

					case GID_Log_Errors:
					{
						GUIFunc_Errors( cfg );
						break;
					}

					case GID_WhiteList:
					{
						GUIFunc_WhiteList( cfg );
						break;
					}

					case GID_WhiteUp:
					{
						GUIFunc_WhiteUp( cfg );
						break;
					}

					case GID_WhiteDown:
					{
						GUIFunc_WhiteDown( cfg );
						break;
					}

					case GID_BlackList:
					{
						GUIFunc_BlackList( cfg );
						break;
					}

					case GID_BlackUp:
					{
						GUIFunc_BlackUp( cfg );
						break;
					}

					case GID_BlackDown:
					{
						GUIFunc_BlackDown( cfg );
						break;
					}

					case GID_Screen_DelayInteger:
					case GID_Screen_DelayScroller:
					{
						GUIFunc_UpdateDelay( cfg );
						break;
					}

					case GID_ServerName:
					{
						GUIFunc_UpdateServerName( cfg );
						break;
					}

					case GID_Password:
					{
						GUIFunc_UpdatePassword( cfg );
						break;
					}

					case GID_Actions_Program_Start_Enable:
					{
						GUIFunc_Actions_Program_Start_Enable( cfg );
						break;
					}

					case GID_Actions_Program_Stop_Enable:
					{
						GUIFunc_Actions_Program_Stop_Enable( cfg );
						break;
					}

					case GID_Actions_Server_Start_Enable:
					{
						GUIFunc_Actions_Server_Start_Enable( cfg );
						break;
					}

					case GID_Actions_Server_Stop_Enable:
					{
						GUIFunc_Actions_Server_Stop_Enable( cfg );
						break;
					}

					case GID_Actions_User_Connect_Enable:
					{
						GUIFunc_Actions_User_Connect_Enable( cfg );
						break;
					}

					case GID_Actions_User_Disconnect_Enable:
					{
						GUIFunc_Actions_User_Disconnect_Enable( cfg );
						break;
					}

					case GID_Actions_Program_Start_String:
					{
						GUIFunc_UpdateActionProgramStart( cfg );
						break;
					}

					case GID_Actions_Program_Stop_String:
					{
						GUIFunc_UpdateActionProgramStop( cfg );
						break;
					}

					case GID_Actions_Server_Start_String:
					{
						GUIFunc_UpdateActionServerStart( cfg );
						break;
					}

					case GID_Actions_Server_Stop_String:
					{
						GUIFunc_UpdateActionServerStop( cfg );
						break;
					}

					case GID_Actions_User_Connect_String:
					{
						GUIFunc_UpdateActionUserConnect( cfg );
						break;
					}

					case GID_Actions_User_Disconnect_String:
					{
						GUIFunc_UpdateActionUserDisconnect( cfg );
						break;
					}

//	GID_Actions_Program_Start_Type,
//	GID_Actions_Program_Start_Enable,
//	GID_Actions_Program_Start_String,
//	GID_Actions_Program_Stop_Type,
//	GID_Actions_Program_Stop_Enable,
//	GID_Actions_Program_Stop_String,
//	GID_Actions_Server_Start_Type,
//	GID_Actions_Server_Start_Enable,
//	GID_Actions_Server_Start_String,
//	GID_Actions_Server_Stop_Type,
//	GID_Actions_Server_Stop_Enable,
//	GID_Actions_Server_Stop_String,
//	GID_Actions_User_Connect_Type,
//	GID_Actions_User_Connect_Enable,
//	GID_Actions_User_Connect_String,
//	GID_Actions_User_Disconnect_Type,
//	GID_Actions_User_Disconnect_Enable,
//	GID_Actions_User_Disconnect_String,

					default:
					{
						break;
					}
				}
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

static void myMenu_Reset( struct Config *cfg )
{
	Config_Reset( cfg );

	myGUI_RefreshSettings( cfg );
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
			if ( cfg->UserCount )
			{
				if ( IDOS->TimedDosRequesterTags(
					TDR_Window, cfg->cfg_WinData[WIN_Main].WindowAdr,
					TDR_ImageType, TDRIMAGE_QUESTION,
					TDR_TitleString, "RVNCd",
					TDR_FormatString, "There are %ld user(s) connected\nAre you sure you want to Quit?",
					TDR_GadgetString, "_No|_Yes",
					TDR_Arg1, cfg->UserCount,
					TAG_END ) == 0 )
				{
					ProgramRunning = FALSE;
				}
			}
			else
			{
				ProgramRunning = FALSE;
			}
			break;
		}

		case MENUID_Load:
		{
			myMenu_Load( cfg );
			break;
		}

		case MENUID_Save:
		{
			Config_Save( cfg, cfg->cfg_Config_FileName );
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
	MainMenuStrip = IIntuition->NewObject( NULL,"menuclass",
		MA_Type,				T_ROOT,

		MA_AddChild,			IIntuition->NewObject( NULL,"menuclass",
			MA_Type,			T_MENU,
			MA_Label,			"Project",

// -- 			  MA_AddChild,		  IIntuition->NewObject( NULL,"menuclass",
// -- 				  MA_ID,		  MENUID_SaveDefaultLog,
// -- 				  MA_Type,		  T_ITEM,
// -- 				  MA_Label,		  GSTR(STR_MENU_PROJECT_SAVE_LOG),
// -- 				  MA_Key,		  GSTR(STR_MENU_PROJECT_SAVE_LOG_SHORTCUT),
// -- 			  End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

 			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
 				MA_Type,				T_ITEM,
 				MA_Label,				"Debug",


 				MA_AddChild,			IIntuition->NewObject( NULL,"menuclass",
 					MA_ID,				MENUID_Encodings,
 					MA_Type,			T_ITEM,
					MA_Label,			"Encodings",
//					MA_Key,				GSTR(STR_MENU_VIEW_ALL_SHORTCUT),
 				End,

 				MA_AddChild,			IIntuition->NewObject( NULL,"menuclass",
 					MA_ID,				MENUID_KeyLogger,
 					MA_Type,			T_ITEM,
					MA_Label,			"Key Logger",
//					MA_Key,				GSTR(STR_MENU_VIEW_ALL_SHORTCUT),
 				End,

 				MA_AddChild,			IIntuition->NewObject( NULL,"menuclass",
 					MA_ID,				MENUID_PixelFormat,
 					MA_Type,			T_ITEM,
					MA_Label,			"Pixel Format",
//					MA_Key,				GSTR(STR_MENU_VIEW_ALL_SHORTCUT),
 				End,

 				MA_AddChild,			IIntuition->NewObject( NULL,"menuclass",
 					MA_ID,				MENUID_Session,
 					MA_Type,			T_ITEM,
					MA_Label,			"Session Info",
//					MA_Key,				GSTR(STR_MENU_VIEW_ALL_SHORTCUT),
 				End,


			End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_ID,					MENUID_About,
				MA_Type,				T_ITEM,
				MA_Label,				"About",
				MA_Key,					"?",
			End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_ID,					MENUID_Quit,
				MA_Type,				T_ITEM,
				MA_Label,				"Quit",
				MA_Key,					"Q",
			End,
		End,

		MA_AddChild,					IIntuition->NewObject( NULL,"menuclass",
			MA_Type,					T_MENU,
			MA_Label,					"Preferences",

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_ID,					MENUID_Load,
				MA_Type,				T_ITEM,
				MA_Label,				"Load Settings",
			End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_ID,					MENUID_Save,
				MA_Type,				T_ITEM,
				MA_Label,				"Save Settings",
			End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_ID,					MENUID_SaveAS,
				MA_Type,				T_ITEM,
				MA_Label,				"Save Settings as",
			End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_ID,					MENUID_SaveDefault,
				MA_Type,				T_ITEM,
				MA_Label,				"Save Settings as default",
			End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_Type,				T_ITEM,
				MA_Separator,			TRUE,
			End,

			MA_AddChild,				IIntuition->NewObject( NULL,"menuclass",
				MA_Type,				T_ITEM,
				MA_Label,				"Reset Settings",

				MA_AddChild,			IIntuition->NewObject( NULL,"menuclass",
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

void myGUI_AddLogStringMessage( struct Config *cfg UNUSED, struct CommandLogString *msg )
{
char *str;
APTR n;

	if ( GUIObjects[ GID_Window ] == NULL )
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
};

// --

void myGUI_RefreshMessage( struct Config *cfg, struct CommandRefresh *msg UNUSED )
{
	if ( GUIObjects[ GID_Window ] == NULL )
	{
		return;
	}

	myGUI_RefreshSettings( cfg );
};

// --

void myGUI_RefreshStats( struct Config *cfg, struct CommandRefresh *msg UNUSED )
{
	if ( GUIObjects[ GID_Window ] == NULL )
	{
		return;
	}

	mySetTags( cfg, GUIObjects[ GID_Stats_Accecpted ],
		BUTTON_Integer,	cfg->Connect_Accecpted,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Stats_Rejected ],
		BUTTON_Integer,	cfg->Connect_Rejected,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Stats_Whitelisted ],
		BUTTON_Integer,	cfg->Connect_WhiteListed,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Stats_Blacklisted ],
		BUTTON_Integer,	cfg->Connect_BlackListed,
		TAG_END
	);

};

// --
