 
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
	GID_ListView,
	GID_Clear,
	GID_LAST
};

// --

static struct List			KeyListHeader;
static int					KeyListCount		= 0;

static Object *				GUIObjects[GID_LAST];

static struct TextFont *	DefaultFontAdr		= NULL;
static struct TextAttr		DefaultFontTextAttr;

// --

static int myGUI_OpenFont( struct Config *cfg );
static void myGUI_CloseFont( struct Config *cfg );

// --

static void VARARGS68K mySetTags( struct Config *cfg, void *object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_KeyLogger].WindowAdr )
	{
		ILayout->SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_KeyLogger].WindowAdr,
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

int myGUI_OpenKeyWindow( struct Config *cfg UNUSED )
{
struct CommandKey *ck;
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
		if ( cfg->cfg_WinData[WIN_KeyLogger].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_KeyLogger].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_KeyLogger].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
			}
		}
		else
		{
			IIntuition->WindowToFront( cfg->cfg_WinData[WIN_KeyLogger].WindowAdr );
			IIntuition->ActivateWindow( cfg->cfg_WinData[WIN_KeyLogger].WindowAdr );
		}
		goto bailout;
	}

	IExec->NewList( & KeyListHeader );

	error = TRUE;

	// --

	if ( myGUI_OpenFont( cfg ))
	{
		goto bailout;
	}

	// --

	GUIObjects[ GID_Window ] = IIntuition->NewObject( WindowClass, NULL,
		WA_Activate,							    TRUE,
		WA_CloseGadget,							    TRUE,
		WA_DepthGadget,							    TRUE,
		WA_DragBar,								    TRUE,
		WA_SizeGadget,							    TRUE,
		WA_Title,								    "RVNCd - Key Logger",

		( cfg->cfg_WinData[WIN_KeyLogger].Width == 0 ) ?
		TAG_IGNORE : WA_Left, cfg->cfg_WinData[WIN_KeyLogger].XPos,

		( cfg->cfg_WinData[WIN_KeyLogger].Width == 0 ) ?
		TAG_IGNORE : WA_Top, cfg->cfg_WinData[WIN_KeyLogger].YPos,

		WA_Width, ( cfg->cfg_WinData[WIN_KeyLogger].Width != 0 ) ?
		cfg->cfg_WinData[WIN_KeyLogger].Width : 430,

		WA_Height, ( cfg->cfg_WinData[WIN_KeyLogger].Width != 0 ) ?
		cfg->cfg_WinData[WIN_KeyLogger].Height : 400,

		( cfg->cfg_WinData[WIN_KeyLogger].Width != 0 ) ?
		TAG_IGNORE : WINDOW_Position, WPOS_CENTERSCREEN,

//		WA_PubScreen,							    gs->up_PubScreen,
		WINDOW_AppPort,								WinAppPort,
		WINDOW_SharedPort,							WinMsgPort,
		WINDOW_IconifyGadget,						TRUE,
		WINDOW_IconTitle,							"KeyLogger",
//		WINDOW_MenuStrip,					    	MainMenuStrip,
//		WINDOW_MenuUserData,				    	WGUD_HOOK,
//		WINDOW_Position,						    WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,						    GUIObjects[ GID_Root ] = IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,						GUIObjects[ GID_ListView ] = IIntuition->NewObject( ListBrowserClass, NULL,
				GA_ID,								GID_ListView,
				GA_RelVerify,						TRUE,
				LISTBROWSER_ShowSelected,			FALSE,
				LISTBROWSER_AutoFit,				TRUE,
				LISTBROWSER_Labels,					& KeyListHeader,
				LISTBROWSER_Striping,				LBS_ROWS,
			End,
			CHILD_WeightedHeight,					25,

			LAYOUT_AddChild,					    IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,				    IIntuition->NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,				30,

				LAYOUT_AddChild,				    GUIObjects[ GID_Clear ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,						    GID_Clear,
					GA_Text,					    "Clear List",
					GA_RelVerify,				    TRUE,
					BUTTON_Justification,		    BCJ_CENTER,
				End,
				CHILD_WeightedWidth,				40,

				LAYOUT_AddChild,				    IIntuition->NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,				30,
			End,
			CHILD_WeightedHeight,				    0,
		End,
	End;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating KeyLogger GUI Object" );
		goto bailout;
	}

	// --

	ck = (APTR) IExec->GetHead( & KeyLoggerList );

	while( ck )
	{
		myGUI_AddKeyMessage( cfg, ck );

		ck = (APTR) IExec->GetSucc( (APTR) ck );
	}

	mySetTags( cfg, GUIObjects[ GID_ListView ],
		LISTBROWSER_Position, LBP_BOTTOM,
		TAG_END
	);

	// --

	if ( cfg->cfg_WinData[WIN_KeyLogger].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_KeyLogger].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_KeyLogger].WindowAdr == NULL )
		{
			goto bailout;
		}

//		cfg->cfg_WinData[WIN_KeyLogger].WindowAdr->UserData = (APTR) myGUI_HandleMainWindow;

//		IIntuition->GetAttrs( GUIObjects[ GID_Window ], 
//			WINDOW_SigMask, & KeyWindowBits,
//			TAG_END
//		);
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

void myGUI_CloseKeyWindow( struct Config *cfg )
{
struct Window *win;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		goto bailout;
	}

	win = cfg->cfg_WinData[WIN_KeyLogger].WindowAdr;

	if ( win )
	{
		cfg->cfg_WinData[WIN_KeyLogger].XPos = win->LeftEdge;
		cfg->cfg_WinData[WIN_KeyLogger].YPos = win->TopEdge;
		cfg->cfg_WinData[WIN_KeyLogger].Width = win->Width - win->BorderLeft - win->BorderRight;
		cfg->cfg_WinData[WIN_KeyLogger].Height = win->Height - win->BorderTop - win->BorderBottom;
	}

	IIntuition->DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_KeyLogger].WindowAdr = NULL;

	myGUI_CloseFont( cfg );

bailout:

	return;
}

// --

static void myGUI_ClearList( struct Config *cfg UNUSED )
{
APTR n;

	mySetTags( cfg, GUIObjects[ GID_ListView ],
		LISTBROWSER_Labels,	NULL,
		TAG_END
	);

	while(( n = (APTR) IExec->RemHead( & KeyListHeader )))
	{
		IListBrowser->FreeListBrowserNode( n );
	}

	KeyListCount = 0;

	mySetTags( cfg, GUIObjects[ GID_ListView ],
		LISTBROWSER_Labels,	& KeyListHeader,
		LISTBROWSER_Position, LBP_BOTTOM,
		TAG_END
	);
};

// --

void myGUI_HandleKeyWindow( struct Config *cfg )
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
				cfg->cfg_WinData[WIN_KeyLogger].Status = WINSTAT_Iconifyed;
				if ( IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_KeyLogger].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_KeyLogger].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_KeyLogger].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
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
					case GID_Clear:
					{
						myGUI_ClearList( cfg );
						break;
					}

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
		cfg->cfg_WinData[WIN_KeyLogger].Status = WINSTAT_Close;
		myGUI_CloseKeyWindow( cfg );
	}
}

// --

void myGUI_AddKeyMessage( struct Config *cfg UNUSED, struct CommandKey *msg )
{
APTR img;
APTR n;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		goto bailout;
	}

	mySetTags( cfg, GUIObjects[ GID_ListView ],
		LISTBROWSER_Labels,	NULL,
		TAG_END
	);

	img = IIntuition->NewObject( LabelClass, NULL,
		IA_Font, & DefaultFontTextAttr,
		LABEL_Underscore, ~0,
		LABEL_Text, msg->ck_Client,
		LABEL_Text, "\n",
		LABEL_Text, msg->ck_Amiga,
		TAG_END
	);

	n = IListBrowser->AllocListBrowserNode( 1,
		LBNA_Column, 0,
			LBNCA_Image, img,
		TAG_END
	);

	if ( n )
	{
		IExec->AddTail( & KeyListHeader, n );
		KeyListCount++;

		while( MAX_KeyListNodes < KeyListCount )
		{
			n = IExec->RemHead( & KeyListHeader );
			IListBrowser->FreeListBrowserNode( n );
			KeyListCount--;
		}
	}

	mySetTags( cfg, GUIObjects[ GID_ListView ],
		LISTBROWSER_Labels,	& KeyListHeader,
		LISTBROWSER_Position, LBP_BOTTOM,
		TAG_END
	);

bailout:

	return;
};

// --

static int myGUI_OpenFont( struct Config *cfg )
{
int error;

	error = FALSE;

	DefaultFontTextAttr.ta_Name	= "DejaVu Sans Mono.font";
	DefaultFontTextAttr.ta_YSize = 14;
	
	DefaultFontAdr = IDiskfont->OpenDiskFont( & DefaultFontTextAttr );

	if ( DefaultFontAdr )
	{
		goto bailout;
	}

	// This is a rom font .. unlikely to ever fail
	DefaultFontTextAttr.ta_Name	= "topaz.font";
	DefaultFontTextAttr.ta_YSize = 11;
	
	DefaultFontAdr = IDiskfont->OpenDiskFont( & DefaultFontTextAttr );

	if ( DefaultFontAdr )
	{
		goto bailout;
	}

	Log_PrintF( cfg, LOGTYPE_Error, "Program: Error opening font" );

	error = TRUE;

bailout:

	return( error );
}

// --

static void myGUI_CloseFont( struct Config *cfg UNUSED )
{
	if ( DefaultFontAdr )
	{
		IGraphics->CloseFont( DefaultFontAdr );
		DefaultFontAdr = NULL;
	}
}

// --
