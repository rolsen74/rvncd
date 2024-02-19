 
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

	GID_Pixel_BPP,
	GID_Pixel_Depth,
	GID_Pixel_Endian,
	GID_Pixel_TrueColor,
	GID_Pixel_MaxRed,
	GID_Pixel_MaxGreen,
	GID_Pixel_MaxBlue,
	GID_Pixel_ShiftRed,
	GID_Pixel_ShiftGreen,
	GID_Pixel_ShiftBlue,

	GID_ScreenMode,
	GID_ScreenWidth,
	GID_ScreenHeight,

	GID_EncMode,
	GID_EncMethod,

	GID_LAST
};

// --

static Object *GUIObjects[GID_LAST];
static struct CommandPxlFmt	PxlFormat;

static void myGUI_PixelRefresh( struct Config *cfg );

// --

static void VARARGS68K mySetTags( struct Config *cfg, void *object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr )
	{
		ILayout->SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_PixelFormat].WindowAdr,
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

static Object *Page_PixelFormat( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,							LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,							IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,							IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,						LAYOUT_ORIENT_HORIZ,

//			LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
//			End,

			LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

				LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,				BVS_GROUP,
					LAYOUT_Label,					"Amiga",

					LAYOUT_AddChild,				GUIObjects[ GID_ScreenMode ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_Text,					"------------",
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
	//					BUTTON_Integer,				PxlFormat.cpf_Format.pm_BitsPerPixel,
					End,
					CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,					"Screen Mode",
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_ScreenWidth ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
	//					BUTTON_Integer,				PxlFormat.cpf_Format.pm_BitsPerPixel,
					End,
					CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,					"Screen Width",
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_ScreenHeight ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
	//					BUTTON_Integer,				PxlFormat.cpf_Format.pm_BitsPerPixel,
					End,
					CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,					"Screen Height",
					End,

					LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
					End,
				End,

				LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,				BVS_GROUP,
					LAYOUT_Label,					"Encoding",

					LAYOUT_AddChild,				GUIObjects[ GID_EncMode ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_Text,					"------------",
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
	//					BUTTON_Integer,				PxlFormat.cpf_Format.pm_BitsPerPixel,
					End,
					CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,					"Enc Mode",
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_EncMethod ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
	//					BUTTON_Integer,				PxlFormat.cpf_Format.pm_BitsPerPixel,
					End,
					CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,					"Enc Method",
					End,

					LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
					End,
				End,
			End,

			LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,					BVS_GROUP,
				LAYOUT_Label,						"Client",

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_BPP ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_Text,						"------------",
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_BitsPerPixel,
				End,
				CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,						"Bits Per Pixel",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_Depth ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_Depth,
				End,
				CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,						"Depth",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_Endian ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_BigEndian,
				End,
				CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,						"Big Endian",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_TrueColor ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_TrueColor,
				End,
				CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,						"True Color",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_MaxRed ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_RedMax,
				End,
				CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,						"Red Max",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_MaxGreen ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_GreenMax,
				End,
				CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,						"Green Max",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_MaxBlue ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_BlueMax,
				End,
				CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,						"Blue Max",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_ShiftRed ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_RedShift,
				End,
				CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,						"Red Shift",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_ShiftGreen ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_GreenShift,
				End,
				CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,						"Green Shift",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_ShiftBlue ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_BlueShift,
				End,
				CHILD_Label,						IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,						"Blue Shift",
				End,
			End,
			CHILD_WeightedHeight,					0,

//			LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
//			End,
		End,

		LAYOUT_AddChild,							IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

int myGUI_OpenPixelWindow( struct Config *cfg UNUSED )
{
//struct ColumnInfo *ci;
//struct CommandPixel *ck;
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
		if ( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_PixelFormat].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_PixelFormat].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

				if ( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr )
				{
					myGUI_PixelRefresh( cfg );
				}
			}
		}
		else
		{
			IIntuition->WindowToFront( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr );
			IIntuition->ActivateWindow( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr );
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
		WA_Title,								    "RVNCd - Pixel Infomation",

		( cfg->cfg_WinData[WIN_PixelFormat].Width == 0 ) ?
		TAG_IGNORE : WA_Left, cfg->cfg_WinData[WIN_PixelFormat].XPos,

		( cfg->cfg_WinData[WIN_PixelFormat].Width == 0 ) ?
		TAG_IGNORE : WA_Top, cfg->cfg_WinData[WIN_PixelFormat].YPos,

		WA_Width, ( cfg->cfg_WinData[WIN_PixelFormat].Width != 0 ) ?
		cfg->cfg_WinData[WIN_PixelFormat].Width : 450,

		WA_Height, ( cfg->cfg_WinData[WIN_PixelFormat].Width != 0 ) ?
		cfg->cfg_WinData[WIN_PixelFormat].Height : 300,

		( cfg->cfg_WinData[WIN_PixelFormat].Width != 0 ) ?
		TAG_IGNORE : WINDOW_Position, WPOS_CENTERSCREEN,

//		WA_PubScreen,							    gs->up_PubScreen,
		WINDOW_AppPort,								WinAppPort,
		WINDOW_SharedPort,							WinMsgPort,
		WINDOW_IconifyGadget,						TRUE,
		WINDOW_IconTitle,							"Pixel Format",
//		WINDOW_MenuStrip,					    	MainMenuStrip,
//		WINDOW_MenuUserData,				    	WGUD_HOOK,
//		WINDOW_Position,						    WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,						    GUIObjects[ GID_Root ] = IIntuition->NewObject( LayoutClass, NULL,

			LAYOUT_AddChild,						Page_PixelFormat( cfg ),

		End,
	End;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating Pixel GUI Object" );
		goto bailout;
	}

	// --

	if ( cfg->cfg_WinData[WIN_PixelFormat].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_PixelFormat].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr == NULL )
		{
			goto bailout;
		}

		myGUI_PixelRefresh( cfg );
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

void myGUI_ClosePixelWindow( struct Config *cfg )
{
struct Window *win;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		goto bailout;
	}

	win = cfg->cfg_WinData[WIN_PixelFormat].WindowAdr;

	if ( win )
	{
		cfg->cfg_WinData[WIN_PixelFormat].XPos = win->LeftEdge;
		cfg->cfg_WinData[WIN_PixelFormat].YPos = win->TopEdge;
		cfg->cfg_WinData[WIN_PixelFormat].Width = win->Width - win->BorderLeft - win->BorderRight;
		cfg->cfg_WinData[WIN_PixelFormat].Height = win->Height - win->BorderTop - win->BorderBottom;
	}

	IIntuition->DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_PixelFormat].WindowAdr = NULL;

bailout:

	return;
}

// --

void myGUI_HandlePixelWindow( struct Config *cfg )
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
				cfg->cfg_WinData[WIN_PixelFormat].Status = WINSTAT_Iconifyed;
				if ( IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_PixelFormat].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_PixelFormat].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_PixelFormat].WindowAdr = (APTR) IIntuition->IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
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
//					case GID_Pixel_Refresh:
//					{
//						myGUI_PixelRefresh( cfg );
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
		cfg->cfg_WinData[WIN_PixelFormat].Status = WINSTAT_Close;
		myGUI_ClosePixelWindow( cfg );
	}
}

// --

static char *ModeNames[] =
{
/* 00 */ "None",
/* 01 */ "CLUT",
/* 02 */ "R8G8B8",
/* 03 */ "B8G8R8",
/* 04 */ "R5G6B5 PC",
/* 05 */ "R5G5B5 PC",
/* 06 */ "A8R8G8B8",
/* 07 */ "A8B8G8R8",
/* 08 */ "R8G8B8A8",
/* 09 */ "B8G8R8A8",
/* 10 */ "R5G6B5",
/* 11 */ "R5G5B5",
/* 12 */ "B5G6R5 PC",
/* 13 */ "B5G5R5 PC",
/* 14 */ "YUV522 CGX",
/* 15 */ "YUV411",
/* 16 */ "YUV422 PA",
/* 17 */ "YUV422",
/* 18 */ "YUV422 PC",
/* 19 */ "YUV420 P",
/* 20 */ "YUV410 P",
/* 21 */ "ALPHA 8",
};

static void myGUI_PixelRefresh( struct Config *cfg )
{
	mySetTags( cfg, GUIObjects[ GID_Pixel_BPP ],
		BUTTON_Integer, PxlFormat.cpf_Format.pm_BitsPerPixel,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Pixel_Depth ],
		BUTTON_Integer, PxlFormat.cpf_Format.pm_Depth,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Pixel_Endian ],
		BUTTON_Integer, PxlFormat.cpf_Format.pm_BigEndian,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Pixel_TrueColor ],
		BUTTON_Integer, PxlFormat.cpf_Format.pm_TrueColor,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Pixel_MaxRed ],
		BUTTON_Integer, PxlFormat.cpf_Format.pm_RedMax,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Pixel_MaxGreen ],
		BUTTON_Integer, PxlFormat.cpf_Format.pm_GreenMax,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Pixel_MaxBlue ],
		BUTTON_Integer, PxlFormat.cpf_Format.pm_BlueMax,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Pixel_ShiftRed ],
		BUTTON_Integer, PxlFormat.cpf_Format.pm_RedShift,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Pixel_ShiftGreen ],
		BUTTON_Integer, PxlFormat.cpf_Format.pm_GreenShift,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Pixel_ShiftBlue ],
		BUTTON_Integer, PxlFormat.cpf_Format.pm_BlueShift,
		TAG_END
	);

	// --

	mySetTags( cfg, GUIObjects[ GID_ScreenMode ],
		GA_Text, ( cfg->GfxRead_Screen_Format < 22 ) ? ModeNames[ cfg->GfxRead_Screen_Format ] : "---",
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_ScreenWidth ],
		BUTTON_Integer, cfg->GfxRead_Screen_Width,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_ScreenHeight ],
		BUTTON_Integer, cfg->GfxRead_Screen_Height,
		TAG_END
	);

	// --

	mySetTags( cfg, GUIObjects[ GID_EncMode ],
		GA_Text, ( cfg->GfxRead_Encode_Format < 22 ) ? ModeNames[ cfg->GfxRead_Encode_Format ] : "---",
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_EncMethod ],
		GA_Text, ( cfg->GfxRead_Encode_FuncName ) ? cfg->GfxRead_Encode_FuncName : "---",
		TAG_END
	);

}

// --

void myGUI_PxlFmtMessage( struct Config *cfg, struct CommandPxlFmt *msg )
{
	PxlFormat = *msg;

	if ( GUIObjects[ GID_Window ] == NULL )
	{
		return;
	}

	myGUI_PixelRefresh( cfg );

};

void myGUI_PxlFmtRefresh( struct Config *cfg )
{
	if ( GUIObjects[ GID_Window ] == NULL )
	{
		return;
	}

	myGUI_PixelRefresh( cfg );

};

// --
