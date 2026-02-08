
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

// --

Object *Pixel_GUIObjects[GID_LAST];
static struct CommandPxlFmt	PxlFormat;

void myGUI_PixelRefresh( struct Config *cfg );

// --

void VARARGS68K Pixel_SetTags( struct Config *cfg, PTR object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr )
	{
		SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_PixelFormat].WindowAdr,
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

Object *Page_PixelFormat( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,							LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,							NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,							NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,						LAYOUT_ORIENT_HORIZ,

//			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
//			End,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,				BVS_GROUP,
					LAYOUT_Label,					"Amiga",

					LAYOUT_AddChild,				GUIObjects[ GID_ScreenMode ] = NewObject( ButtonClass, NULL,
						GA_Text,					"------------",
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
	//					BUTTON_Integer,				PxlFormat.cpf_Format.pm_BitsPerPixel,
					End,
					CHILD_Label,					NewObject( LabelClass, NULL,
						LABEL_Text,					"Screen Mode",
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_ScreenWidth ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
	//					BUTTON_Integer,				PxlFormat.cpf_Format.pm_BitsPerPixel,
					End,
					CHILD_Label,					NewObject( LabelClass, NULL,
						LABEL_Text,					"Screen Width",
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_ScreenHeight ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
	//					BUTTON_Integer,				PxlFormat.cpf_Format.pm_BitsPerPixel,
					End,
					CHILD_Label,					NewObject( LabelClass, NULL,
						LABEL_Text,					"Screen Height",
					End,

					LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
					End,
				End,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,				BVS_GROUP,
					LAYOUT_Label,					"Encoding",

					LAYOUT_AddChild,				GUIObjects[ GID_EncMode ] = NewObject( ButtonClass, NULL,
						GA_Text,					"------------",
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
	//					BUTTON_Integer,				PxlFormat.cpf_Format.pm_BitsPerPixel,
					End,
					CHILD_Label,					NewObject( LabelClass, NULL,
						LABEL_Text,					"Enc Mode",
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_EncMethod ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
	//					BUTTON_Integer,				PxlFormat.cpf_Format.pm_BitsPerPixel,
					End,
					CHILD_Label,					NewObject( LabelClass, NULL,
						LABEL_Text,					"Enc Method",
					End,

					LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
					End,
				End,
			End,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,					BVS_GROUP,
				LAYOUT_Label,						"Client",

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_BPP ] = NewObject( ButtonClass, NULL,
					GA_Text,						"------------",
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_BitsPerPixel,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Bits Per Pixel",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_Depth ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_Depth,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Depth",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_Endian ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_BigEndian,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Big Endian",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_TrueColor ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_TrueColor,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"True Color",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_MaxRed ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_RedMax,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Red Max",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_MaxGreen ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_GreenMax,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Green Max",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_MaxBlue ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_BlueMax,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Blue Max",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_ShiftRed ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_RedShift,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Red Shift",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_ShiftGreen ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_GreenShift,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Green Shift",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Pixel_ShiftBlue ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
					BUTTON_Integer,					PxlFormat.cpf_Format.pm_BlueShift,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Blue Shift",
				End,
			End,
			CHILD_WeightedHeight,					0,

//			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
//			End,
		End,

		LAYOUT_AddChild,							NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void RAGUI_BusyPixelWindow( struct Config *cfg, S32 val )
{
	if ( val )
	{
		/**/ cfg->cfg_WinData[WIN_PixelFormat].Busy++;

		if ( cfg->cfg_WinData[WIN_PixelFormat].Busy == 1 )
		{
			mySetTags( cfg, GUIObjects[ GID_Window ],
				WA_BusyPointer, TRUE,
				TAG_END
			);
		}
	}
	else
	{
		if ( cfg->cfg_WinData[WIN_PixelFormat].Busy > 0 )
		{
			 cfg->cfg_WinData[WIN_PixelFormat].Busy--;

			if ( cfg->cfg_WinData[WIN_PixelFormat].Busy == 0 )
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

S32 RAGUI_OpenPixelWindow( struct Config *cfg UNUSED )
{
//struct ColumnInfo *ci;
//struct CommandPixel *ck;
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
		if ( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_PixelFormat].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_PixelFormat].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

				if ( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr )
				{
					myGUI_PixelRefresh( cfg );
				}
			}
		}
		else
		{
			WindowToFront( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr );
			ActivateWindow( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr );
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
		WA_Title,									"rVNCd - Pixel Infomation",
		WA_BusyPointer,								cfg->cfg_WinData[WIN_PixelFormat].Busy > 0,

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

//		WA_PubScreen,								gs->up_PubScreen,
		WINDOW_AppPort,								& WinAppPort.vmp_MsgPort,
		WINDOW_SharedPort,							& WinMsgPort.vmp_MsgPort,
		WINDOW_PopupGadget,							TRUE,
		WINDOW_Icon,								ProgramIcon,
		WINDOW_IconTitle,							"rVNCd Pixel",
		WINDOW_IconNoDispose,						TRUE,
		WINDOW_IconifyGadget,						TRUE,
//		WINDOW_MenuStrip,							MainMenuStrip,
//		WINDOW_MenuUserData,						WGUD_HOOK,
//		WINDOW_Position,							WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,							GUIObjects[ GID_Root ] = NewObject( LayoutClass, NULL,

			LAYOUT_AddChild,						Page_PixelFormat( cfg ),

		End,
	End;

	if ( ! GUIObjects[ GID_Window ] )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating Pixel GUI Object" );
		goto bailout;
	}

	// --

	if ( cfg->cfg_WinData[WIN_PixelFormat].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_PixelFormat].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_PixelFormat].WindowAdr == NULL )
		{
			goto bailout;
		}

		myGUI_PixelRefresh( cfg );
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

void RAGUI_ClosePixelWindow( struct Config *cfg )
{
struct Window *win;

	if ( ! GUIObjects[ GID_Window ] )
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

	DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_PixelFormat].WindowAdr = NULL;

bailout:

	return;
}

// --

void RAGUI_HandlePixelWindow( struct Config *cfg )
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
				cfg->cfg_WinData[WIN_PixelFormat].Status = WINSTAT_Iconifyed;
				if ( IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_PixelFormat].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_PixelFormat].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_PixelFormat].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
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
		RAGUI_ClosePixelWindow( cfg );
	}
}

// --

static STR ModeNames[VNCPix_Last] =
{
	[VNCPix_Unknown]	= "Unknown",
	[VNCPix_R5G6B5]		= "R5G6B5",
	[VNCPix_R5G6B5PC]	= "R5G6B5PC",
	[VNCPix_A8R8G8B8]	= "A8R8G8B8",
	[VNCPix_B8G8R8A8]	= "B8G8R8A8",
};

void myGUI_PixelRefresh( struct Config *cfg )
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
		GA_Text, ( cfg->GfxRead_Screen_Format < VNCPix_Last ) ? ModeNames[ cfg->GfxRead_Screen_Format ] : "---",
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_ScreenWidth ],
		BUTTON_Integer, cfg->GfxRead_Screen_PageWidth,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_ScreenHeight ],
		BUTTON_Integer, cfg->GfxRead_Screen_PageHeight,
		TAG_END
	);

	// --

	mySetTags( cfg, GUIObjects[ GID_EncMode ],
		GA_Text, ( cfg->GfxRead_Encode_Format < VNCPix_Last ) ? ModeNames[ cfg->GfxRead_Encode_Format ] : "---",
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_EncMethod ],
		GA_Text, ( cfg->GfxRead_Encode_FuncName ) ? cfg->GfxRead_Encode_FuncName : "---",
		TAG_END
	);

}

// --

void GUIFunc_PxlFmtMessage( struct Config *cfg, struct CommandPxlFmt *msg )
{
	PxlFormat = *msg;

	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	myGUI_PixelRefresh( cfg );
}

void myGUI_PxlFmtRefresh( struct Config *cfg )
{
	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	myGUI_PixelRefresh( cfg );
}

// --
