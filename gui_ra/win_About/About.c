
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

Object *About_GUIObjects[GID_LAST];

// --

void VARARGS68K About_SetTags( struct Config *cfg, PTR object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_About].WindowAdr )
	{
		SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_About].WindowAdr,
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

static PTR myLink( S32 id, STR txt )
{
Object *o;

	o = NewObject( ButtonClass, NULL,
		GA_ID,					id,
		GA_Text,				txt,
		GA_RelVerify,			TRUE,
		BUTTON_SoftStyle,		FSF_UNDERLINED,
		BUTTON_BevelStyle,		BVS_NONE,
		BUTTON_Transparent,		TRUE,
		BUTTON_Justification,	BCJ_CENTER,
	End;

	return( o );
}

static PTR myText( STR txt )
{
Object *o;

	o = NewObject( ButtonClass, NULL,
		GA_Text,				txt,
		GA_ReadOnly,			TRUE,
		BUTTON_BevelStyle,		BVS_NONE,
		BUTTON_Transparent,		TRUE,
		BUTTON_Justification,	BCJ_CENTER,
	End;

	return( o );
}

Object *About_Page_User( struct Config *cfg UNUSED )
{
char verbuf[256];
Object *o;

	sprintf( verbuf, "Version %d.%d (%s)", VERSION, REVISION, DATE );

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,								LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,								NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,								NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,							LAYOUT_ORIENT_HORIZ,

			LAYOUT_AddChild,							NewObject( LayoutClass, NULL,
			End,

			LAYOUT_AddChild,							NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,
				LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
//					LAYOUT_AddChild,					myText( "" ),
					LAYOUT_AddChild,					myText( "rVNCd - Rene's VNC daemon" ),
					LAYOUT_AddChild,					myText( verbuf ),
					LAYOUT_AddChild,					myText( "" ),
					LAYOUT_AddChild,					myText( "Programmed by:" ),
					LAYOUT_AddChild,					myText( "Rene W. Olsen" ),
					LAYOUT_AddChild,					myText( "" ),
					LAYOUT_AddChild,					myText( "Thanks to my beta testers" ),
					LAYOUT_AddChild,					myText( "" ),
				End,
				LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

					LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
						LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,

						LAYOUT_AddChild,				myText( "Paul J. Sadlik" ),
						LAYOUT_AddChild,				myText( "Bill Borsari" ),
					End,
					LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
						LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,

						LAYOUT_AddChild,				myText( "Val Marti" ),
						LAYOUT_AddChild,				myText( "Mike" ),
					End,
				End,
				LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
					LAYOUT_AddChild,					myText( "" ),
					LAYOUT_AddChild,					myText( "This software is distributed under the" ),
					LAYOUT_AddChild,					myText( "GNU General Public License version 3 (GPLv3)." ),
					LAYOUT_AddChild,					myText( "" ),
				End,


				LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

					LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
						LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
						LAYOUT_AddChild,				myText( "Buy Me a Coffee" ),
//						LAYOUT_AddChild,				myImage(),
						LAYOUT_AddChild,				GUIObjects[ GID_BMC ] = myLink( GID_BMC, "buymeacoffee.com/rolsen74" ),
					End,
					LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
						LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
						LAYOUT_AddChild,				myText( "source code" ),
						LAYOUT_AddChild,				GUIObjects[ GID_SRC ] = myLink( GID_SRC, "github.com/rolsen74" ),
					End,
				End,

			End,
			CHILD_WeightedHeight,						0,

			LAYOUT_AddChild,							NewObject( LayoutClass, NULL,
			End,

		End,
		CHILD_WeightedHeight,							0,

		LAYOUT_AddChild,								NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void RAGUI_BusyAboutWindow( struct Config *cfg, S32 val )
{
	if ( val )
	{
		/**/ cfg->cfg_WinData[WIN_About].Busy++;

		if ( cfg->cfg_WinData[WIN_About].Busy == 1 )
		{
			mySetTags( cfg, GUIObjects[ GID_Window ],
				WA_BusyPointer, TRUE,
				TAG_END
			);
		}
	}
	else
	{
		if ( cfg->cfg_WinData[WIN_About].Busy > 0 )
		{
			 cfg->cfg_WinData[WIN_About].Busy--;

			if ( cfg->cfg_WinData[WIN_About].Busy == 0 )
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

S32 RAGUI_OpenAboutWindow( struct Config *cfg UNUSED )
{
//struct ColumnInfo *ci;
//struct CommandAbout *ck;
S32 error;
//S32 pos;

	// --

	error = FALSE;

	if ( cfg->cfg_ProgramDisableGUI )
	{
		Log_PrintF( cfg, LOGTYPE_Warning, "Program: GUI Disabled can't open" );
		goto bailout;
	}

	if ( GUIObjects[ GID_Window ] )
	{
		if ( cfg->cfg_WinData[WIN_About].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_About].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_About].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
			}
		}
		else
		{
			WindowToFront( cfg->cfg_WinData[WIN_About].WindowAdr );
			ActivateWindow( cfg->cfg_WinData[WIN_About].WindowAdr );
		}
		goto bailout;
	}

	error = TRUE;

	// --

	GUIObjects[ GID_Window ] = NewObject( WindowClass, NULL,
		WA_Activate,								TRUE,
		WA_CloseGadget,								TRUE,
		WA_DepthGadget,								TRUE,
		WA_DragBar,									TRUE,
		WA_SizeGadget,								TRUE,
		WA_Title,									"rVNCd - About Infomation",
		WA_BusyPointer,								cfg->cfg_WinData[WIN_About].Busy > 0,

		( cfg->cfg_WinData[WIN_About].Width == 0 ) ?
		TAG_IGNORE : WA_Left, cfg->cfg_WinData[WIN_About].XPos,

		( cfg->cfg_WinData[WIN_About].Width == 0 ) ?
		TAG_IGNORE : WA_Top, cfg->cfg_WinData[WIN_About].YPos,

		WA_Width, ( cfg->cfg_WinData[WIN_About].Width != 0 ) ?
		cfg->cfg_WinData[WIN_About].Width : 430,

		WA_Height, ( cfg->cfg_WinData[WIN_About].Width != 0 ) ?
		cfg->cfg_WinData[WIN_About].Height : 400,

		( cfg->cfg_WinData[WIN_About].Width != 0 ) ?
		TAG_IGNORE : WINDOW_Position, WPOS_CENTERSCREEN,

//		WA_PubScreen,								gs->up_PubScreen,
		WINDOW_AppPort,								& WinAppPort.vmp_MsgPort,
		WINDOW_SharedPort,							& WinMsgPort.vmp_MsgPort,
		WINDOW_PopupGadget,							TRUE,
		WINDOW_Icon,								ProgramIcon,
		WINDOW_IconTitle,							"rVNCd About",
		WINDOW_IconNoDispose,						TRUE,
		WINDOW_IconifyGadget,						TRUE,
//		WINDOW_MenuStrip,							MainMenuStrip,
//		WINDOW_MenuUserData,						WGUD_HOOK,
		WINDOW_ParentGroup,							GUIObjects[ GID_Root ] = NewObject( LayoutClass, NULL,

			LAYOUT_AddChild,						About_Page_User( cfg ),

		End,
	End;

	if ( ! GUIObjects[ GID_Window ] )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating About GUI Object" );
		goto bailout;
	}

	// --

	if ( cfg->cfg_WinData[WIN_About].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_About].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_About].WindowAdr == NULL )
		{
			goto bailout;
		}
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

void RAGUI_CloseAboutWindow( struct Config *cfg )
{
struct Window *win;

	if ( ! GUIObjects[ GID_Window ] )
	{
		goto bailout;
	}

	win = cfg->cfg_WinData[WIN_About].WindowAdr;

	if ( win )
	{
		cfg->cfg_WinData[WIN_About].XPos = win->LeftEdge;
		cfg->cfg_WinData[WIN_About].YPos = win->TopEdge;
		cfg->cfg_WinData[WIN_About].Width = win->Width - win->BorderLeft - win->BorderRight;
		cfg->cfg_WinData[WIN_About].Height = win->Height - win->BorderTop - win->BorderBottom;
	}

	DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_About].WindowAdr = NULL;

bailout:

	return;
}

// --

void RAGUI_HandleAboutWindow( struct Config *cfg )
{
U32 result;
U16 code;
BPTR h;
S32 theend;

    theend = FALSE;

	while(( result = IDoMethod( GUIObjects[ GID_Window ], WM_HANDLEINPUT, &code )) != WMHI_LASTMSG )
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
				cfg->cfg_WinData[WIN_About].Status = WINSTAT_Iconifyed;
				if ( IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_About].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_About].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_About].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
				break;
			}

			case WMHI_RAWKEY:
			{
				switch( result & WMHI_KEYMASK )
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
					case GID_BMC:
					{
						h = Open( "URL:https://www.buymeacoffee.com/rolsen74", MODE_OLDFILE );

						if ( h )
						{
							Close( h );
						}
						break;
					}

					case GID_SRC:
					{
						h = Open( "URL:https://github.com/rolsen74", MODE_OLDFILE );

						if ( h )
						{
							Close( h );
						}
						break;
					}

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
		cfg->cfg_WinData[WIN_About].Status = WINSTAT_Close;
		RAGUI_CloseAboutWindow( cfg );
	}
}

// --

#endif // GUI_RA
