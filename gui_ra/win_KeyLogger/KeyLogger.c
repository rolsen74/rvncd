
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

static struct VNCList			KeyListHeader;
static S32					KeyListCount		= 0;

Object *KeyLogger_GUIObjects[GID_LAST];

static struct TextFont *	DefaultFontAdr		= NULL;
static struct TextAttr		DefaultFontTextAttr;

// --

static S32 RAGUI_OpenFont( struct Config *cfg );
static void RAGUI_CloseFont( struct Config *cfg );

// --

void VARARGS68K KeyLogger_SetTags( struct Config *cfg, PTR object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_KeyLogger].WindowAdr )
	{
		SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_KeyLogger].WindowAdr,
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

void RAGUI_BusyKeyWindow( struct Config *cfg, S32 val )
{
	if ( val )
	{
		/**/ cfg->cfg_WinData[WIN_KeyLogger].Busy++;

		if ( cfg->cfg_WinData[WIN_KeyLogger].Busy == 1 )
		{
			mySetTags( cfg, GUIObjects[ GID_Window ],
				WA_BusyPointer, TRUE,
				TAG_END
			);
		}
	}
	else
	{
		if ( cfg->cfg_WinData[WIN_KeyLogger].Busy > 0 )
		{
			 cfg->cfg_WinData[WIN_KeyLogger].Busy--;

			if ( cfg->cfg_WinData[WIN_KeyLogger].Busy == 0 )
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

S32 RAGUI_OpenKeyWindow( struct Config *cfg UNUSED )
{
struct CommandKey *ck;
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
		if ( cfg->cfg_WinData[WIN_KeyLogger].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_KeyLogger].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_KeyLogger].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
			}
		}
		else
		{
			WindowToFront( cfg->cfg_WinData[WIN_KeyLogger].WindowAdr );
			ActivateWindow( cfg->cfg_WinData[WIN_KeyLogger].WindowAdr );
		}
		goto bailout;
	}

	List_Init( & KeyListHeader );

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
		WA_Title,									"rVNCd - Key Logger",
		WA_BusyPointer,								cfg->cfg_WinData[WIN_KeyLogger].Busy > 0,

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

//		WA_PubScreen,								gs->up_PubScreen,
		WINDOW_AppPort,								& WinAppPort.vmp_MsgPort,
		WINDOW_SharedPort,							& WinMsgPort.vmp_MsgPort,
		WINDOW_PopupGadget,							TRUE,
		WINDOW_Icon,								ProgramIcon,
		WINDOW_IconTitle,							"rVNCd Key",
		WINDOW_IconNoDispose,						TRUE,
		WINDOW_IconifyGadget,						TRUE,
//		WINDOW_MenuStrip,							MainMenuStrip,
//		WINDOW_MenuUserData,						WGUD_HOOK,
//		WINDOW_Position,							WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,							GUIObjects[ GID_Root ] = NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,						GUIObjects[ GID_ListView ] = NewObject( ListBrowserClass, NULL,
				GA_ID,								GID_ListView,
				GA_RelVerify,						TRUE,
				LISTBROWSER_ShowSelected,			FALSE,
				LISTBROWSER_AutoFit,				TRUE,
				LISTBROWSER_Labels,					& KeyListHeader.vl_List,
				LISTBROWSER_Striping,				LBS_ROWS,
			End,
			CHILD_WeightedHeight,					25,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,				30,

				LAYOUT_AddChild,					GUIObjects[ GID_Clear ] = NewObject( ButtonClass, NULL,
					GA_ID,							GID_Clear,
					GA_Text,						"Clear List",
					GA_RelVerify,					TRUE,
					BUTTON_Justification,			BCJ_CENTER,
				End,
				CHILD_WeightedWidth,				40,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,				30,
			End,
			CHILD_WeightedHeight,					0,
		End,
	End;

	if ( ! GUIObjects[ GID_Window ] )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating KeyLogger GUI Object" );
		goto bailout;
	}

	// --

	ck = List_GetHead( & KeyLoggerList );

	while( ck )
	{
		myGUI_AddKeyMessage( cfg, ck );

		ck = List_GetNext( ck );
	}

	mySetTags( cfg, GUIObjects[ GID_ListView ],
		LISTBROWSER_Position, LBP_BOTTOM,
		TAG_END
	);

	// --

	if ( cfg->cfg_WinData[WIN_KeyLogger].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_KeyLogger].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_KeyLogger].WindowAdr == NULL )
		{
			goto bailout;
		}

//		cfg->cfg_WinData[WIN_KeyLogger].WindowAdr->UserData = (PTR) RAGUI_HandleMainWindow;

//		GetAttrs( GUIObjects[ GID_Window ], 
//			WINDOW_SigMask, & KeyWindowBits,
//			TAG_END
//		);
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

void RAGUI_CloseKeyWindow( struct Config *cfg )
{
struct Window *win;

	if ( ! GUIObjects[ GID_Window ] )
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

	DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_KeyLogger].WindowAdr = NULL;

	RAGUI_CloseFont( cfg );

bailout:

	return;
}

// --

void myGUI_ClearList( struct Config *cfg UNUSED )
{
PTR n;

	mySetTags( cfg, GUIObjects[ GID_ListView ],
		LISTBROWSER_Labels,	NULL,
		TAG_END
	);

	while(( n = List_RemHead( & KeyListHeader )))
	{
		FreeListBrowserNode( n );
	}

	KeyListCount = 0;

	mySetTags( cfg, GUIObjects[ GID_ListView ],
		LISTBROWSER_Labels,	& KeyListHeader.vl_List,
		LISTBROWSER_Position, LBP_BOTTOM,
		TAG_END
	);
}

// --

void RAGUI_HandleKeyWindow( struct Config *cfg )
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
				cfg->cfg_WinData[WIN_KeyLogger].Status = WINSTAT_Iconifyed;
				if ( IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_KeyLogger].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_KeyLogger].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_KeyLogger].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
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
		RAGUI_CloseKeyWindow( cfg );
	}
}

// --

void myGUI_AddKeyMessage( struct Config *cfg UNUSED, struct CommandKey *msg )
{
PTR img;
PTR n;

	if ( ! GUIObjects[ GID_Window ] )
	{
		goto bailout;
	}

	mySetTags( cfg, GUIObjects[ GID_ListView ],
		LISTBROWSER_Labels,	NULL,
		TAG_END
	);

	img = NewObject( LabelClass, NULL,
		IA_Font, & DefaultFontTextAttr,
		LABEL_Underscore, ~0,
		LABEL_Text, msg->ck_Client,
		LABEL_Text, "\n",
		LABEL_Text, msg->ck_Amiga,
		TAG_END
	);

	n = AllocListBrowserNode( 1,
		LBNA_Column, 0,
			LBNCA_Image, img,
		TAG_END
	);

	if ( n )
	{
		List_AddTail( & KeyListHeader, n );
		KeyListCount++;

		while( MAX_KeyListNodes < KeyListCount )
		{
			n = List_RemHead( & KeyListHeader );
			FreeListBrowserNode( n );
			KeyListCount--;
		}
	}

	mySetTags( cfg, GUIObjects[ GID_ListView ],
		LISTBROWSER_Labels,	& KeyListHeader.vl_List,
		LISTBROWSER_Position, LBP_BOTTOM,
		TAG_END
	);

bailout:

	return;
}

// --

static S32 RAGUI_OpenFont( struct Config *cfg )
{
S32 error;

	error = FALSE;

	DefaultFontTextAttr.ta_Name	= "DejaVu Sans Mono.font";
	DefaultFontTextAttr.ta_YSize = 14;
	
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

#endif // GUI_RA
