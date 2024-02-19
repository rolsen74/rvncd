 
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

static void myGUI_SessionRefresh( struct Config *cfg );

// --

enum
{
	GID_Root,
	GID_Window,

	GID_Session_IP,
	GID_Session_Read,
	GID_Session_Send,
	GID_Session_Tiles,
	GID_Session_ZLib,
	GID_Session_Raw,
	GID_Session_Connect,
	GID_Session_Duration,

	GID_LAST
};

// --

static Object *	GUIObjects[GID_LAST];

// --

static void VARARGS68K mySetTags( struct Config *cfg, void *object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_Session].WindowAdr )
	{
		ILayout->SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_Session].WindowAdr,
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

static Object *Page_User( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,								LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,								IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,								IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,							LAYOUT_ORIENT_HORIZ,

//			LAYOUT_AddChild,							IIntuition->NewObject( LayoutClass, NULL,
//			End,

			LAYOUT_AddChild,							IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,						BVS_GROUP,

//				LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
//					LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
//					LAYOUT_BevelStyle,					BVS_GROUP,

					LAYOUT_AddChild,					GUIObjects[ GID_Session_IP ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,					TRUE,
						BUTTON_BevelStyle,				BVS_THIN,
					End,
					CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,						"IP Address",
					End,

					LAYOUT_AddChild,					GUIObjects[ GID_Session_Connect ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,					TRUE,
//						GA_Text,						"00:00:00 - 00-000-0000",
						BUTTON_BevelStyle,				BVS_THIN,
					End,
					CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,						"Connected at",
					End,
					CHILD_MinWidth,						160,

					LAYOUT_AddChild,					GUIObjects[ GID_Session_Duration ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,					TRUE,
						BUTTON_BevelStyle,				BVS_THIN,
					End,
					CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,						"Duration",
					End,

					LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
					End,
					CHILD_MinHeight,					10,
					CHILD_MaxHeight,					10,

					LAYOUT_AddChild,					GUIObjects[ GID_Session_Read ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,					TRUE,
						BUTTON_BevelStyle,				BVS_THIN,
					End,
					CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,						"Bytes Read",
					End,

					LAYOUT_AddChild,					GUIObjects[ GID_Session_Send ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,					TRUE,
						BUTTON_BevelStyle,				BVS_THIN,
					End,
					CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,						"Bytes Send",
					End,

					LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
					End,
					CHILD_MinHeight,					10,
					CHILD_MaxHeight,					10,

					LAYOUT_AddChild,					GUIObjects[ GID_Session_Tiles ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,					TRUE,
						BUTTON_BevelStyle,				BVS_THIN,
					End,
					CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,						"Tiles",
					End,

					LAYOUT_AddChild,					GUIObjects[ GID_Session_Raw ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,					TRUE,
						BUTTON_BevelStyle,				BVS_THIN,
					End,
					CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,						"Raw Tiles",
					End,

					LAYOUT_AddChild,					GUIObjects[ GID_Session_ZLib ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,					TRUE,
						BUTTON_BevelStyle,				BVS_THIN,
					End,
					CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,						"ZLib Tiles",
					End,
//				End,
//				CHILD_WeightedWidth,					0,
//				CHILD_WeightedHeight,					0,

			End,
//			CHILD_WeightedWidth,						0,
			CHILD_WeightedHeight,						0,

//			LAYOUT_AddChild,							IIntuition->NewObject( LayoutClass, NULL,
//			End,

		End,

		LAYOUT_AddChild,								IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

int myGUI_OpenSessionWindow( struct Config *cfg UNUSED )
{
//struct ColumnInfo *ci;
//struct CommandSession *ck;
int error;
//int pos;

	// --

	error = FALSE;

	if ( cfg->cfg_ProgramDisableGUI )
	{
		Log_PrintF( cfg, LOGTYPE_Warning, "Program: GUI Disabled can't open" );
		goto bailout;
	}

	if ( GUIObjects[ GID_Window ] )
	{
		if ( cfg->cfg_WinData[WIN_Session].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_Session].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_Session].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

				if ( cfg->cfg_WinData[WIN_Session].WindowAdr )
				{
					myGUI_SessionRefresh( cfg );
				}
			}
		}
		else
		{
			IIntuition->WindowToFront( cfg->cfg_WinData[WIN_Session].WindowAdr );
			IIntuition->ActivateWindow( cfg->cfg_WinData[WIN_Session].WindowAdr );
		}
		goto bailout;
	}

	error = TRUE;

	// --

	GUIObjects[ GID_Window ] = IIntuition->NewObject( WindowClass, NULL,
		WA_Activate,							    TRUE,
		WA_CloseGadget,							    TRUE,
		WA_DepthGadget,							    TRUE,
		WA_DragBar,								    TRUE,
		WA_SizeGadget,							    TRUE,
		WA_Title,								    "RVNCd - Session Infomation",

		( cfg->cfg_WinData[WIN_Session].Width == 0 ) ?
		TAG_IGNORE : WA_Left, cfg->cfg_WinData[WIN_Session].XPos,

		( cfg->cfg_WinData[WIN_Session].Width == 0 ) ?
		TAG_IGNORE : WA_Top, cfg->cfg_WinData[WIN_Session].YPos,

		WA_Width, ( cfg->cfg_WinData[WIN_Session].Width != 0 ) ?
		cfg->cfg_WinData[WIN_Session].Width : 350,

		WA_Height, ( cfg->cfg_WinData[WIN_Session].Width != 0 ) ?
		cfg->cfg_WinData[WIN_Session].Height : 280,

		( cfg->cfg_WinData[WIN_Session].Width != 0 ) ?
		TAG_IGNORE : WINDOW_Position, WPOS_CENTERSCREEN,

//		WA_PubScreen,							    gs->up_PubScreen,
		WINDOW_AppPort,								WinAppPort,
		WINDOW_SharedPort,							WinMsgPort,
		WINDOW_IconifyGadget,						TRUE,
		WINDOW_IconTitle,							"Session",
//		WINDOW_MenuStrip,					    	MainMenuStrip,
//		WINDOW_MenuUserData,				    	WGUD_HOOK,
//		WINDOW_Position,						    WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,						    GUIObjects[ GID_Root ] = IIntuition->NewObject( LayoutClass, NULL,

			LAYOUT_AddChild,						Page_User( cfg ),

		End,
	End;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating Session GUI Object" );
		goto bailout;
	}

	// --

	if ( cfg->cfg_WinData[WIN_Session].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_Session].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_Session].WindowAdr == NULL )
		{
			goto bailout;
		}

		myGUI_SessionRefresh( cfg );
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

void myGUI_CloseSessionWindow( struct Config *cfg )
{
struct Window *win;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		goto bailout;
	}

	win = cfg->cfg_WinData[WIN_Session].WindowAdr;

	if ( win )
	{
		cfg->cfg_WinData[WIN_Session].XPos = win->LeftEdge;
		cfg->cfg_WinData[WIN_Session].YPos = win->TopEdge;
		cfg->cfg_WinData[WIN_Session].Width = win->Width - win->BorderLeft - win->BorderRight;
		cfg->cfg_WinData[WIN_Session].Height = win->Height - win->BorderTop - win->BorderBottom;
	}

	IIntuition->DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_Session].WindowAdr = NULL;

bailout:

	return;
}

// --

void myGUI_HandleSessionWindow( struct Config *cfg )
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

			case WMHI_ICONIFY:
			{
				cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Iconifyed;
				if ( IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_Session].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_Session].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
				break;
			}

			case WMHI_RAWKEY:
			{
				switch( code & WMHI_KEYMASK )
				{
					case 0x45: // ESC
					{
						theend = TRUE;
						break;
					}

					default:
					{
						break;
					}
				}
				break;
			}

			case WMHI_GADGETUP:
			{
				switch( result & WMHI_GADGETMASK )
				{
//					case GID_Session_Refresh:
//					{
//						myGUI_SessionRefresh( cfg );
//						break;
//					}

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
		cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Close;
		myGUI_CloseSessionWindow( cfg );
	}
}

// --

#if 0

struct SessionInfo
{
	// -- Bytes Send/Read in total
	uint64				si_Read_Bytes;
	uint64				si_Send_Bytes;

	// -- Tiles Send
	uint64				si_Tiles_Raw;
	uint64				si_Tiles_ZLib;
	uint64				si_Tiles_Total;

	// -- Time
	struct TimeVal		si_Time_Logon;				// Start Tick
	struct DateStamp	si_Time_Connect;			// Connection Time

	// -- IP Addr
	int					si_IPAddr[4];	
};

#endif

static void myGUI_SessionRefresh( struct Config *cfg )
{
char *buffer;

	buffer = myMalloc( 1024 );

	if (( cfg->cfg_SessionUsed ) && ( buffer ))
	{
		sprintf( buffer, "%d.%d.%d.%d", 
			cfg->SessionStatus.si_IPAddr[0] ,
			cfg->SessionStatus.si_IPAddr[1] ,
			cfg->SessionStatus.si_IPAddr[2] ,
			cfg->SessionStatus.si_IPAddr[3] );

		mySetTags( cfg, GUIObjects[ GID_Session_IP ],
			GA_Text, buffer,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Read ],
			GA_Text, Session_Bytes( buffer, cfg->SessionStatus.si_Read_Bytes ),
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Send ],
			GA_Text, Session_Bytes( buffer, cfg->SessionStatus.si_Send_Bytes ),
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Tiles ],
			GA_Text, Session_Procent( buffer, cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Total ),
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Raw ],
			GA_Text, Session_Procent( buffer, cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Raw ),
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_ZLib ],
			GA_Text, Session_Procent( buffer, cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_ZLib ),
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Connect ],
			GA_Text, Session_Connect_Time( buffer, & cfg->SessionStatus.si_Time_Connect ),
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Duration ],
			GA_Text, Session_Duration( cfg, buffer, & cfg->SessionStatus.si_Time_Logon ),
			TAG_END
		);
	}
	else
	{
		mySetTags( cfg, GUIObjects[ GID_Session_IP ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Read ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Send ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Tiles ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Raw ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_ZLib ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Connect ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Duration ],
			GA_Text, "--",
			TAG_END
		);
	}

	if ( buffer )
	{
		myFree( buffer );
	}
}

// --

void myGUI_TimerTick( struct Config *cfg )
{
	if (( cfg->UserCount == 0 )
	||	( GUIObjects[ GID_Window ] == NULL ) 
	||	( cfg->cfg_WinData[WIN_Session].WindowAdr == NULL ))
	{
		return;
	}

	myGUI_SessionRefresh( cfg );
}

// --

void myGUI_SessionMessage( struct Config *cfg, struct CommandSession *msg UNUSED )
{
	if ( GUIObjects[ GID_Window ] == NULL )
	{
		return;
	}

	myGUI_SessionRefresh( cfg );
}

// --
