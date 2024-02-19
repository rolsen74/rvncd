 
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

	GID_Encoding_Listview,

	GID_LAST
};

// --

static struct List			EncodingsListHeader;
static struct List			EncodingListHeader;

static Object *				GUIObjects[GID_LAST];

static struct TextFont *	DefaultFontAdr		= NULL;
static struct TextAttr		DefaultFontTextAttr;

// --

static struct ColumnInfo EncodingColumnInfo[] =
{
	{ 80, "Code", 0 },
	{ 80, "Description", 0 },
	{ -1, (APTR) -1, -1 }
};

// --

static int myGUI_OpenFont( struct Config *cfg );
static void myGUI_CloseFont( struct Config *cfg );

// --

static void VARARGS68K mySetTags( struct Config *cfg, void *object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_Encodings].WindowAdr )
	{
		ILayout->SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_Encodings].WindowAdr,
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


// --


// --

static Object *Page_Encodings( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,

		LAYOUT_AddChild,					GUIObjects[ GID_Encoding_Listview ] = IIntuition->NewObject( ListBrowserClass, NULL,
			GA_ID,							GID_Encoding_Listview,
			GA_RelVerify,					TRUE,
			GA_TextAttr,					& DefaultFontTextAttr,
			LISTBROWSER_ShowSelected,		TRUE,
			LISTBROWSER_AutoFit,			TRUE,
			LISTBROWSER_Labels,				& EncodingListHeader,
			LISTBROWSER_ColumnInfo,			& EncodingColumnInfo,
			LISTBROWSER_ColumnTitles,		TRUE,
			LISTBROWSER_Striping,			LBS_ROWS,
		End,

	End;

	if ( o )
	{

//		IIntuition->RefreshSetGadgetAttrs( (APTR) o, cfg_WinData[WIN_Encodings].WindowAdr, NULL,
//			LISTBROWSER_Position, LBP_TOP,
//			TAG_END
//		);

	}

	return( o );
}

// --

int myGUI_OpenEncodingsWindow( struct Config *cfg UNUSED )
{
//struct ColumnInfo *ci;
//struct CommandEncodings *ck;
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
		if ( cfg->cfg_WinData[WIN_Encodings].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_Encodings].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_Encodings].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

				if ( cfg->cfg_WinData[WIN_Encodings].WindowAdr )
				{
					myGUI_EncodingMessage( cfg, ActiveEncoding );
				}
			}
		}
		else
		{
			IIntuition->WindowToFront( cfg->cfg_WinData[WIN_Encodings].WindowAdr );
			IIntuition->ActivateWindow( cfg->cfg_WinData[WIN_Encodings].WindowAdr );
		}
		goto bailout;
	}

	IExec->NewList( & EncodingsListHeader );
	IExec->NewList( & EncodingListHeader );

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
		WA_Title,								    "RVNCd - Encodings Infomation",

		( cfg->cfg_WinData[WIN_Encodings].Width == 0 ) ?
		TAG_IGNORE : WA_Left, cfg->cfg_WinData[WIN_Encodings].XPos,

		( cfg->cfg_WinData[WIN_Encodings].Width == 0 ) ?
		TAG_IGNORE : WA_Top, cfg->cfg_WinData[WIN_Encodings].YPos,

		WA_Width, ( cfg->cfg_WinData[WIN_Encodings].Width != 0 ) ?
		cfg->cfg_WinData[WIN_Encodings].Width : 430,

		WA_Height, ( cfg->cfg_WinData[WIN_Encodings].Width != 0 ) ?
		cfg->cfg_WinData[WIN_Encodings].Height : 400,

		( cfg->cfg_WinData[WIN_Encodings].Width != 0 ) ?
		TAG_IGNORE : WINDOW_Position, WPOS_CENTERSCREEN,

//		WA_PubScreen,							    gs->up_PubScreen,
		WINDOW_AppPort,								WinAppPort,
		WINDOW_SharedPort,							WinMsgPort,
		WINDOW_IconifyGadget,						TRUE,
		WINDOW_IconTitle,							"Encodings",
//		WINDOW_MenuStrip,					    	MainMenuStrip,
//		WINDOW_MenuUserData,				    	WGUD_HOOK,
//		WINDOW_Position,						    WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,						    GUIObjects[ GID_Root ] = IIntuition->NewObject( LayoutClass, NULL,

			LAYOUT_AddChild,						Page_Encodings( cfg ),

		End,
	End;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating Encodings GUI Object" );
		goto bailout;
	}

	// --

	if ( cfg->cfg_WinData[WIN_Encodings].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_Encodings].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_Encodings].WindowAdr == NULL )
		{
			goto bailout;
		}

		myGUI_EncodingMessage( cfg, ActiveEncoding );
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

void myGUI_CloseEncodingsWindow( struct Config *cfg )
{
struct Window *win;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		goto bailout;
	}

	win = cfg->cfg_WinData[WIN_Encodings].WindowAdr;

	if ( win )
	{
		cfg->cfg_WinData[WIN_Encodings].XPos = win->LeftEdge;
		cfg->cfg_WinData[WIN_Encodings].YPos = win->TopEdge;
		cfg->cfg_WinData[WIN_Encodings].Width = win->Width - win->BorderLeft - win->BorderRight;
		cfg->cfg_WinData[WIN_Encodings].Height = win->Height - win->BorderTop - win->BorderBottom;
	}

	IIntuition->DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_Encodings].WindowAdr = NULL;

	myGUI_CloseFont( cfg );

bailout:

	return;
}

// --


void myGUI_HandleEncodingsWindow( struct Config *cfg )
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
				cfg->cfg_WinData[WIN_Encodings].Status = WINSTAT_Iconifyed;
				if ( IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_Encodings].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_Encodings].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_Encodings].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
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
		cfg->cfg_WinData[WIN_Encodings].Status = WINSTAT_Close;
		myGUI_CloseEncodingsWindow( cfg );
	}
}

// --

void myGUI_EncodingMessage( struct Config *cfg UNUSED, struct CommandEncoding *msg )
{
char *str;
char buffer[64];
int cnt;
APTR n;

	if ( msg == NULL )
	{
		return;
	}

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		return;
	}

	mySetTags( cfg, GUIObjects[ GID_Encoding_Listview ],
		LISTBROWSER_Labels,	NULL,
		TAG_END
	);

	while(( n = (APTR) IExec->RemHead( & EncodingListHeader )))
	{
		IListBrowser->FreeListBrowserNode( n );
	}

	for( cnt=0 ; cnt<msg->ce_Encodings ; cnt++ )
	{
		sprintf( buffer, "%08lX", msg->ce_Data[cnt] );

		switch( msg->ce_Data[cnt] )
		{
			case 0:		str = "Raw"; break;
			case 1:		str = "CopyRect"; break;
			case 2:		str = "RRE"; break;
			case 4:		str = "CoRRE"; break;
			case 5:		str = "Hextile"; break;
			case 6:		str = "ZLib"; break;
			case 7:		str = "Tight"; break;
			case 8:		str = "Ultra"; break;
			case 9:		str = "ZLibHex"; break;
			case 15:	str = "TRLE"; break;
			case 16:	str = "ZRLE"; break;
			case 17:	str = "ZYWRLE"; break;
			case 0xFFFFFF11:	str = "RichCursor"; break;
			case 0xFFFFFF18:	str = "MousePos"; break;
			case 0xFFFFFF21:	str = "NewFBSize"; break;
			default:			str = ""; break;
		}

		if (( n = IListBrowser->AllocListBrowserNode( 2,
			LBNA_Column, 0,
				LBNCA_CopyText, TRUE,
				LBNCA_Text, buffer,
			LBNA_Column, 1,
				LBNCA_CopyText, FALSE,
				LBNCA_Text, str,
			TAG_END )))
		{
			IExec->AddTail( & EncodingListHeader, n );
		}
	}

	mySetTags( cfg, GUIObjects[ GID_Encoding_Listview ],
		LISTBROWSER_Labels,	& EncodingListHeader,
//		LISTBROWSER_Position, LBP_BOTTOM,
		LISTBROWSER_AutoFit, TRUE,
		TAG_END
	);
}

// --


static int myGUI_OpenFont( struct Config *cfg )
{
int error;

	error = FALSE;

	DefaultFontTextAttr.ta_Name	= "DejaVu Sans Mono.font";
	DefaultFontTextAttr.ta_YSize = 16;

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

