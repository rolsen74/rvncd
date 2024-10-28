
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

enum
{
	GID_Root,
	GID_Window,
	GID_Str1,
	GID_Str2,
	GID_Str3,
	GID_Str4,
	GID_Update,
	GID_Cancel,
	GID_LAST
};

// --

static Object *				GUIObjects[GID_LAST];

static struct TextFont *	DefaultFontAdr		= NULL;
static struct TextAttr		DefaultFontTextAttr;
static char					Quit_StrBuffer1[8];
static char					Quit_StrBuffer2[8];
static char					Quit_StrBuffer3[8];
static char					Quit_StrBuffer4[8];

// --

static int myGUI_OpenFont( struct Config *cfg );
static void myGUI_CloseFont( struct Config *cfg );
//static int myIPisValid( struct Config *cfg );

// --

static void VARARGS68K mySetTags( struct Config *cfg, void *object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_Quit].WindowAdr )
	{
		ILayout->SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_Quit].WindowAdr,
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

void myGUI_BusyQuitWindow( struct Config *cfg, int val )
{
	if ( val )
	{
		/**/ cfg->cfg_WinData[WIN_Quit].Busy++;

		if ( cfg->cfg_WinData[WIN_Quit].Busy == 1 )
		{
			mySetTags( cfg, GUIObjects[ GID_Window ],
				WA_BusyPointer, TRUE,
				TAG_END
			);
		}
	}
	else
	{
		if ( cfg->cfg_WinData[WIN_Quit].Busy > 0 )
		{
			 cfg->cfg_WinData[WIN_Quit].Busy--;

			if ( cfg->cfg_WinData[WIN_Quit].Busy == 0 )
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

int myGUI_OpenQuitWindow( struct Config *cfg UNUSED )
{
//struct IPNode *node;
//struct CommandQuit *ck;
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
		if ( cfg->cfg_WinData[WIN_Quit].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_Quit].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_Quit].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
			}
		}
		else
		{
			IIntuition->WindowToFront( cfg->cfg_WinData[WIN_Quit].WindowAdr );
			IIntuition->ActivateWindow( cfg->cfg_WinData[WIN_Quit].WindowAdr );
		}
		goto bailout;
	}

	error = TRUE;

	// --

	if ( myGUI_OpenFont( cfg ))
	{
		goto bailout;
	}

	// --

	// --

	GUIObjects[ GID_Window ] = IIntuition->NewObject( WindowClass, NULL,
		WA_Activate,							    TRUE,
		WA_CloseGadget,							    TRUE,
		WA_DepthGadget,							    TRUE,
		WA_DragBar,								    TRUE,
		WA_SizeGadget,							    TRUE,
		WA_Title,								    "RVNCd - Quit",
		WA_BusyPointer,								cfg->cfg_WinData[WIN_Quit].Busy > 0,

		( cfg->cfg_WinData[WIN_Quit].Width == 0 ) ?
		TAG_IGNORE : WA_Left, cfg->cfg_WinData[WIN_Quit].XPos,

		( cfg->cfg_WinData[WIN_Quit].Width == 0 ) ?
		TAG_IGNORE : WA_Top, cfg->cfg_WinData[WIN_Quit].YPos,

		WA_Width, ( cfg->cfg_WinData[WIN_Quit].Width != 0 ) ?
		cfg->cfg_WinData[WIN_Quit].Width : 350,

		WA_Height, ( cfg->cfg_WinData[WIN_Quit].Width != 0 ) ?
		cfg->cfg_WinData[WIN_Quit].Height : 80,

		( cfg->cfg_WinData[WIN_Quit].Width != 0 ) ?
		TAG_IGNORE : WINDOW_Position, WPOS_CENTERSCREEN,

//		WA_PubScreen,							    gs->up_PubScreen,
		WINDOW_AppPort,								WinAppPort,
		WINDOW_SharedPort,							WinMsgPort,
		WINDOW_PopupGadget,							TRUE,
		WINDOW_Icon,								ProgramIcon,
		WINDOW_IconTitle,							"rVNCd Quit",
		WINDOW_IconNoDispose,						TRUE,
		WINDOW_IconifyGadget,						TRUE,
//		WINDOW_MenuStrip,					    	MainMenuStrip,
//		WINDOW_MenuUserData,				    	WGUD_HOOK,
//		WINDOW_Position,						    WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,						    GUIObjects[ GID_Root ] = IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,					    IIntuition->NewObject( LayoutClass, NULL,
			End,

			LAYOUT_AddChild,					    IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_BevelStyle,					BVS_SBAR_VERT,
			End,
			CHILD_MinHeight,						3,
			CHILD_MaxHeight,						3,

			LAYOUT_AddChild,					    IIntuition->NewObject( LayoutClass, NULL,
			End,
			CHILD_MinHeight,						5,
			CHILD_MaxHeight,						5,

			LAYOUT_AddChild,					    IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Str1 ] = IIntuition->NewObject( StringClass, NULL,
					GA_ID,							GID_Str1,
					GA_RelVerify,					TRUE,
					GA_TabCycle,					TRUE,
					STRINGA_MaxChars,				5,
					STRINGA_TextVal,				Quit_StrBuffer1,
				End,
				CHILD_WeightedWidth,				25,

				LAYOUT_AddChild,					GUIObjects[ GID_Str2 ] = IIntuition->NewObject( StringClass, NULL,
					GA_ID,							GID_Str2,
					GA_RelVerify,					TRUE,
					GA_TabCycle,					TRUE,
					STRINGA_MaxChars,				5,
					STRINGA_TextVal,				Quit_StrBuffer2,
				End,
				CHILD_WeightedWidth,				25,

				LAYOUT_AddChild,					GUIObjects[ GID_Str3 ] = IIntuition->NewObject( StringClass, NULL,
					GA_ID,							GID_Str3,
					GA_RelVerify,					TRUE,
					GA_TabCycle,					TRUE,
					STRINGA_MaxChars,				5,
					STRINGA_TextVal,				Quit_StrBuffer3,
				End,
				CHILD_WeightedWidth,				25,

				LAYOUT_AddChild,					GUIObjects[ GID_Str4 ] = IIntuition->NewObject( StringClass, NULL,
					GA_ID,							GID_Str4,
					GA_RelVerify,					TRUE,
					GA_TabCycle,					TRUE,
					STRINGA_MaxChars,				5,
					STRINGA_TextVal,				Quit_StrBuffer4,
				End,
				CHILD_WeightedWidth,				25,

				LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,
			CHILD_WeightedHeight,				    0,

			LAYOUT_AddChild,					    IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,				    IIntuition->NewObject( LayoutClass, NULL,
				End,

				LAYOUT_AddChild,				    IIntuition->NewObject( ButtonClass, NULL,
					GA_Text,						"* = wildcard",
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_NONE,
					BUTTON_Transparent,				TRUE,
					BUTTON_Justification,			BCJ_CENTER,
				End,

				LAYOUT_AddChild,				    IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,
			CHILD_WeightedHeight,				    0,

			LAYOUT_AddChild,					    IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_BevelStyle,					BVS_SBAR_VERT,
			End,
			CHILD_MinHeight,						3,
			CHILD_MaxHeight,						3,

			LAYOUT_AddChild,					    IIntuition->NewObject( LayoutClass, NULL,
			End,

			LAYOUT_AddChild,					    IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,				    IIntuition->NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,				30,

				LAYOUT_AddChild,				    GUIObjects[ GID_Update ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,						    GID_Update,
					GA_Text,					    "Update",
					GA_RelVerify,				    TRUE,
					GA_Disabled,					0, //myIPisValid( cfg ) ? FALSE : TRUE,
					BUTTON_Justification,		    BCJ_CENTER,
				End,
				CHILD_WeightedWidth,				40,

				LAYOUT_AddChild,				    GUIObjects[ GID_Cancel ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,						    GID_Cancel,
					GA_Text,					    "Cancel",
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

	if ( ! GUIObjects[ GID_Window ] )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating QuitLogger GUI Object" );
		goto bailout;
	}

	// --

	if ( cfg->cfg_WinData[WIN_Quit].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_Quit].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_Quit].WindowAdr == NULL )
		{
			goto bailout;
		}

//		cfg->cfg_WinData[WIN_Quit].WindowAdr->UserData = (APTR) myGUI_HandleMainWindow;

//		IIntuition->GetAttrs( GUIObjects[ GID_Window ], 
//			WINDOW_SigMask, & QuitWindowBits,
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

void myGUI_CloseQuitWindow( struct Config *cfg )
{
struct Window *win;

	if ( ! GUIObjects[ GID_Window ] )
	{
		goto bailout;
	}

	win = cfg->cfg_WinData[WIN_Quit].WindowAdr;

	if ( win )
	{
		cfg->cfg_WinData[WIN_Quit].XPos = win->LeftEdge;
		cfg->cfg_WinData[WIN_Quit].YPos = win->TopEdge;
		cfg->cfg_WinData[WIN_Quit].Width = win->Width - win->BorderLeft - win->BorderRight;
		cfg->cfg_WinData[WIN_Quit].Height = win->Height - win->BorderTop - win->BorderBottom;
	}

	IIntuition->DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_Quit].WindowAdr = NULL;

	myGUI_CloseFont( cfg );

bailout:

	return;
}

// --

void myGUI_HandleQuitWindow( struct Config *cfg )
{
uint32 result;
uint16 code;
int theend;
//int update;

	theend = FALSE;
//	update = FALSE;

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
				cfg->cfg_WinData[WIN_Quit].Status = WINSTAT_Iconifyed;
				if ( IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_Quit].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_Quit].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_Quit].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
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
					case GID_Update:
					{
						{
//							update = TRUE;
							theend = TRUE;
						}
						break;
					}

					case GID_Cancel:
					{
						theend = TRUE;
						break;
					}

					case GID_Str1:
					case GID_Str2:
					case GID_Str3:
					case GID_Str4:
					{
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
		cfg->cfg_WinData[WIN_Quit].Status = WINSTAT_Close;
		myGUI_CloseQuitWindow( cfg );
	}
}

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
