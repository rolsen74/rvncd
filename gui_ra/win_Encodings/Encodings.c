
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

// --

static struct VNCList		EncodingsListHeader;
static struct VNCList		EncodingListHeader;

Object *Encodings_GUIObjects[GID_LAST];

static struct TextFont *	DefaultFontAdr		= NULL;
static struct TextAttr		DefaultFontTextAttr;

// --

static struct ColumnInfo EncodingColumnInfo[] =
{
	{ 80, "Code", 0 },
	{ 80, "Description", 0 },
	{ -1, (PTR) -1, -1 }
};

// --

static S32 RAGUI_OpenFont( struct Config *cfg );
static void RAGUI_CloseFont( struct Config *cfg );

// --

void VARARGS68K Encodings_SetTags( struct Config *cfg, PTR object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_Encodings].WindowAdr )
	{
		SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_Encodings].WindowAdr,
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

Object *Page_Encodings( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,

		LAYOUT_AddChild,					GUIObjects[ GID_Encoding_Listview ] = NewObject( ListBrowserClass, NULL,
			GA_ID,							GID_Encoding_Listview,
			GA_RelVerify,					TRUE,
			GA_TextAttr,					& DefaultFontTextAttr,
			LISTBROWSER_ShowSelected,		TRUE,
			LISTBROWSER_AutoFit,			TRUE,
			LISTBROWSER_Labels,				& EncodingListHeader.vl_List,
			LISTBROWSER_ColumnInfo,			& EncodingColumnInfo,
			LISTBROWSER_ColumnTitles,		TRUE,
			LISTBROWSER_Striping,			LBS_ROWS,
		End,

	End;

	if ( o )
	{

//		RefreshSetGadgetAttrs( (PTR) o, cfg_WinData[WIN_Encodings].WindowAdr, NULL,
//			LISTBROWSER_Position, LBP_TOP,
//			TAG_END
//		);

	}

	return( o );
}

// --

void RAGUI_BusyEncodingsWindow( struct Config *cfg, S32 val )
{
	if ( val )
	{
		/**/ cfg->cfg_WinData[WIN_Encodings].Busy++;

		if ( cfg->cfg_WinData[WIN_Encodings].Busy == 1 )
		{
			mySetTags( cfg, GUIObjects[ GID_Window ],
				WA_BusyPointer, TRUE,
				TAG_END
			);
		}
	}
	else
	{
		if ( cfg->cfg_WinData[WIN_Encodings].Busy > 0 )
		{
			 cfg->cfg_WinData[WIN_Encodings].Busy--;

			if ( cfg->cfg_WinData[WIN_Encodings].Busy == 0 )
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

S32 RAGUI_OpenEncodingsWindow( struct Config *cfg UNUSED )
{
//struct ColumnInfo *ci;
//struct CommandEncodings *ck;
S32 error;
//S32 pos;

	// --

	error = FALSE;

	if ( cfg->cfg_Program_DisableGUI )
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
				cfg->cfg_WinData[WIN_Encodings].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

				if ( cfg->cfg_WinData[WIN_Encodings].WindowAdr )
				{
					myGUI_EncodingMessage( cfg, ActiveEncoding );
				}
			}
		}
		else
		{
			WindowToFront( cfg->cfg_WinData[WIN_Encodings].WindowAdr );
			ActivateWindow( cfg->cfg_WinData[WIN_Encodings].WindowAdr );
		}
		goto bailout;
	}

	List_Init( & EncodingsListHeader );
	List_Init( & EncodingListHeader );

	error = TRUE;

	// --

	if ( RAGUI_OpenFont( cfg ))
	{
		goto bailout;
	}

	// --

	GUIObjects[ GID_Window ] = NewObject( WindowClass, NULL,
		WA_Activate,								TRUE,
		WA_CloseGadget,								TRUE,
		WA_DepthGadget,								TRUE,
		WA_DragBar,									TRUE,
		WA_SizeGadget,								TRUE,
		WA_Title,									"rVNCd - Encodings Infomation",
		WA_BusyPointer,								cfg->cfg_WinData[WIN_Encodings].Busy > 0,

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

//		WA_PubScreen,								gs->up_PubScreen,
		WINDOW_AppPort,								& WinAppPort.vmp_MsgPort,
		WINDOW_SharedPort,							& WinMsgPort.vmp_MsgPort,
		WINDOW_PopupGadget,							TRUE,
		WINDOW_Icon,								ProgramIcon,
		WINDOW_IconTitle,							"rVNCd Encodings",
		WINDOW_IconNoDispose,						TRUE,
		WINDOW_IconifyGadget,						TRUE,
//		WINDOW_MenuStrip,							MainMenuStrip,
//		WINDOW_MenuUserData,						WGUD_HOOK,
//		WINDOW_Position,							WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,							GUIObjects[ GID_Root ] = NewObject( LayoutClass, NULL,

			LAYOUT_AddChild,						Page_Encodings( cfg ),

		End,
	End;

	if ( ! GUIObjects[ GID_Window ] )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating Encodings GUI Object" );
		goto bailout;
	}

	// --

	if ( cfg->cfg_WinData[WIN_Encodings].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_Encodings].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_Encodings].WindowAdr == NULL )
		{
			goto bailout;
		}

		myGUI_EncodingMessage( cfg, ActiveEncoding );
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

void RAGUI_CloseEncodingsWindow( struct Config *cfg )
{
struct Window *win;

	if ( ! GUIObjects[ GID_Window ] )
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

	DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_Encodings].WindowAdr = NULL;

	RAGUI_CloseFont( cfg );

bailout:

	return;
}

// --


void RAGUI_HandleEncodingsWindow( struct Config *cfg )
{
U32 result;
U16 code;
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
				cfg->cfg_WinData[WIN_Encodings].Status = WINSTAT_Iconifyed;
				if ( IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_Encodings].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_Encodings].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_Encodings].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
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
		RAGUI_CloseEncodingsWindow( cfg );
	}
}

// --

void myGUI_EncodingMessage( struct Config *cfg UNUSED, struct CommandEncoding *msg )
{
STR str;
char buffer[64];
S32 cnt;
PTR n;

	if ( ! msg )
	{
		return;
	}

	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	mySetTags( cfg, GUIObjects[ GID_Encoding_Listview ],
		LISTBROWSER_Labels,	NULL,
		TAG_END
	);

	while(( n = List_RemHead( & EncodingListHeader )))
	{
		FreeListBrowserNode( n );
	}

	for( cnt=0 ; cnt<msg->ce_Encodings ; cnt++ )
	{
		sprintf( buffer, "%08" PRIX32, msg->ce_Data[cnt] );

		switch( msg->ce_Data[cnt] )
		{
			case 0:				str = "Raw"; break;
			case 1:				str = "CopyRect"; break;
			case 2:				str = "RRE"; break;
			case 4:				str = "CoRRE"; break;
			case 5:				str = "Hextile"; break;
			case 6:				str = "ZLib"; break;
			case 7:				str = "Tight"; break;
			case 8:				str = "Ultra"; break;
			case 9:				str = "ZLibHex"; break;
			case 15:			str = "TRLE"; break;
			case 16:			str = "ZRLE"; break;
			case 17:			str = "ZYWRLE"; break;
			case 0xFFFFFF11:	str = "RichCursor"; break;
			case 0xFFFFFF18:	str = "MousePos"; break;
			case 0xFFFFFF21:	str = "NewFBSize"; break;
			default:			str = ""; break;
		}

		if (( n = AllocListBrowserNode( 2,
			LBNA_Column, 0,
				LBNCA_CopyText, TRUE,
				LBNCA_Text, buffer,
			LBNA_Column, 1,
				LBNCA_CopyText, FALSE,
				LBNCA_Text, str,
			TAG_END )))
		{
			List_AddTail( & EncodingListHeader, n );
		}
	}

	mySetTags( cfg, GUIObjects[ GID_Encoding_Listview ],
		LISTBROWSER_Labels,	& EncodingListHeader.vl_List,
//		LISTBROWSER_Position, LBP_BOTTOM,
		LISTBROWSER_AutoFit, TRUE,
		TAG_END
	);
}

// --


static S32 RAGUI_OpenFont( struct Config *cfg )
{
S32 error;

	error = FALSE;

	DefaultFontTextAttr.ta_Name	= "DejaVu Sans Mono.font";
	DefaultFontTextAttr.ta_YSize = 16;

	DefaultFontAdr = OpenDiskFont( & DefaultFontTextAttr );

	if ( DefaultFontAdr )
	{
		goto bailout;
	}

	// This is a rom font .. unlikely to ever fail
	DefaultFontTextAttr.ta_Name	= "topaz.font";
	DefaultFontTextAttr.ta_YSize = 11;
	
	DefaultFontAdr = OpenDiskFont( & DefaultFontTextAttr );

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

static void RAGUI_CloseFont( struct Config *cfg UNUSED )
{
	if ( DefaultFontAdr )
	{
		CloseFont( DefaultFontAdr );
		DefaultFontAdr = NULL;
	}
}

// --
