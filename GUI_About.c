 
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

enum
{
	GID_Root,
	GID_Window,
	GID_BMC,
	GID_SRC,

	GID_LAST
};

// --

static Object *				GUIObjects[GID_LAST];

// --

static APTR myLink( int id, char *txt )
{
Object *o;

	o = IIntuition->NewObject( ButtonClass, NULL,
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

static APTR myText( char *txt )
{
Object *o;

	o = IIntuition->NewObject( ButtonClass, NULL,
		GA_Text,				txt,
		GA_ReadOnly,			TRUE,
		BUTTON_BevelStyle,		BVS_NONE,
		BUTTON_Transparent,		TRUE,
		BUTTON_Justification,	BCJ_CENTER,
	End;

	return( o );
}

static Object *Page_User( struct Config *cfg UNUSED )
{
char verbuf[256];
Object *o;

	sprintf( verbuf, "Version %d.%d (%s)", VERSION, REVISION, DATE );

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,								LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,								IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,								IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,							LAYOUT_ORIENT_HORIZ,

			LAYOUT_AddChild,							IIntuition->NewObject( LayoutClass, NULL,
			End,

			LAYOUT_AddChild,							IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,
				LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
//					LAYOUT_AddChild,					myText( "" ),
					LAYOUT_AddChild,					myText( "RVNCd - Rene's VNC daemon" ),
					LAYOUT_AddChild,					myText( verbuf ),
					LAYOUT_AddChild,					myText( "" ),
					LAYOUT_AddChild,					myText( "Programmed by:" ),
					LAYOUT_AddChild,					myText( "Rene W. Olsen" ),
					LAYOUT_AddChild,					myText( "" ),
					LAYOUT_AddChild,					myText( "Thanks to my beta testers" ),
					LAYOUT_AddChild,					myText( "" ),
				End,
				LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

					LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
						LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,

						LAYOUT_AddChild,				myText( "Paul J. Sadlik" ),
						LAYOUT_AddChild,				myText( "Bill Borsari" ),
					End,
					LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
						LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,

						LAYOUT_AddChild,				myText( "Val Marti" ),
						LAYOUT_AddChild,				myText( "Mike" ),
					End,
				End,
				LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
					LAYOUT_AddChild,					myText( "" ),
					LAYOUT_AddChild,					myText( "This software is distributed under the" ),
					LAYOUT_AddChild,					myText( "GNU General Public License version 3 (GPLv3)." ),
					LAYOUT_AddChild,					myText( "" ),
				End,


				LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

					LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
						LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
						LAYOUT_AddChild,				myText( "Buy Me a Coffee" ),
//						LAYOUT_AddChild,				myImage(),
						LAYOUT_AddChild,				GUIObjects[ GID_BMC ] = myLink( GID_BMC, "buymeacoffee.com/rolsen74" ),
					End,
					LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
						LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
						LAYOUT_AddChild,				myText( "source code" ),
						LAYOUT_AddChild,				GUIObjects[ GID_SRC ] = myLink( GID_SRC, "github.com/rolsen74" ),
					End,
				End,

			End,
			CHILD_WeightedHeight,				    	0,

			LAYOUT_AddChild,							IIntuition->NewObject( LayoutClass, NULL,
			End,

		End,
		CHILD_WeightedHeight,				    		0,

		LAYOUT_AddChild,								IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

int myGUI_OpenAboutWindow( struct Config *cfg UNUSED )
{
//struct ColumnInfo *ci;
//struct CommandAbout *ck;
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
		if ( cfg->cfg_WinData[WIN_About].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_About].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_About].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
			}
		}
		else
		{
			IIntuition->WindowToFront( cfg->cfg_WinData[WIN_About].WindowAdr );
			IIntuition->ActivateWindow( cfg->cfg_WinData[WIN_About].WindowAdr );
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
		WA_Title,								    "RVNCd - About Infomation",

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

//		WA_PubScreen,							    gs->up_PubScreen,
		WINDOW_AppPort,								WinAppPort,
		WINDOW_SharedPort,							WinMsgPort,
		WINDOW_IconifyGadget,						TRUE,
		WINDOW_IconTitle,							"About",
//		WINDOW_MenuStrip,					    	MainMenuStrip,
//		WINDOW_MenuUserData,				    	WGUD_HOOK,
		WINDOW_ParentGroup,						    GUIObjects[ GID_Root ] = IIntuition->NewObject( LayoutClass, NULL,

			LAYOUT_AddChild,						Page_User( cfg ),

		End,
	End;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating About GUI Object" );
		goto bailout;
	}

	// --

	if ( cfg->cfg_WinData[WIN_About].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_About].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_About].WindowAdr == NULL )
		{
			goto bailout;
		}
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

void myGUI_CloseAboutWindow( struct Config *cfg )
{
struct Window *win;

	if ( GUIObjects[ GID_Window ] == NULL )
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

	IIntuition->DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_About].WindowAdr = NULL;

bailout:

	return;
}

// --

void myGUI_HandleAboutWindow( struct Config *cfg )
{
uint32 result;
uint16 code;
BPTR h;
int theend;

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
				cfg->cfg_WinData[WIN_About].Status = WINSTAT_Iconifyed;
				if ( IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_About].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_About].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_About].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
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
						h = IDOS->Open( "URL:https://www.buymeacoffee.com/rolsen74", MODE_OLDFILE );

						if ( h )
						{
							IDOS->Close( h );
						}
						break;
					}

					case GID_SRC:
					{
						h = IDOS->Open( "URL:https://github.com/rolsen74", MODE_OLDFILE );

						if ( h )
						{
							IDOS->Close( h );
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
		myGUI_CloseAboutWindow( cfg );
	}
}

// --
