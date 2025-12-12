
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

Object *IPEdit_GUIObjects[GID_LAST];

static struct TextFont *	DefaultFontAdr		= NULL;
static struct TextAttr		DefaultFontTextAttr;
static char					IPEdit_StrBuffer1[8];
static char					IPEdit_StrBuffer2[8];
static char					IPEdit_StrBuffer3[8];
static char					IPEdit_StrBuffer4[8];
static U32					IPEdit_Type;
static S32					StringData[4];
static S32					StringIDs[4] = { GID_Str1, GID_Str2, GID_Str3, GID_Str4 };

// --

static const STR IPTypes[] =
{
	"Allow",
	"Block",
	NULL
};

// --

static S32 RAGUI_OpenFont( struct Config *cfg );
static void RAGUI_CloseFont( struct Config *cfg );
static S32 myIPisValid( struct Config *cfg );

// --

void VARARGS68K IPEdit_SetTags( struct Config *cfg, PTR object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_IPEdit].WindowAdr )
	{
		SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_IPEdit].WindowAdr,
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

void RAGUI_BusyIPEditWindow( struct Config *cfg, S32 val )
{
	if ( val )
	{
		/**/ cfg->cfg_WinData[WIN_IPEdit].Busy++;

		if ( cfg->cfg_WinData[WIN_IPEdit].Busy == 1 )
		{
			mySetTags( cfg, GUIObjects[ GID_Window ],
				WA_BusyPointer, TRUE,
				TAG_END
			);
		}
	}
	else
	{
		if ( cfg->cfg_WinData[WIN_IPEdit].Busy > 0 )
		{
			 cfg->cfg_WinData[WIN_IPEdit].Busy--;

			if ( cfg->cfg_WinData[WIN_IPEdit].Busy == 0 )
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

S32 RAGUI_OpenIPEditWindow( struct Config *cfg UNUSED )
{
struct IPNode *node;
S32 error;

	// --

	error = FALSE;

	if ( cfg->cfg_ProgramDisableGUI )
	{
		Log_PrintF( cfg, LOGTYPE_Warning, "Program: GUI Disabled can't open" );
		goto bailout;
	}

	if ( GUIObjects[ GID_Window ] )
	{
		if ( cfg->cfg_WinData[WIN_IPEdit].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_IPEdit].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_IPEdit].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
			}
		}
		else
		{
			WindowToFront( cfg->cfg_WinData[WIN_IPEdit].WindowAdr );
			ActivateWindow( cfg->cfg_WinData[WIN_IPEdit].WindowAdr );
		}
		goto bailout;
	}

	error = TRUE;

	// --

	if ( RAGUI_OpenFont( cfg ))
	{
		goto bailout;
	}

	// --

	if ( cfg->cfg_IPEditNode )
	{
		node = (PTR) cfg->cfg_IPEditNode->ln_Name;
	}
	else
	{
		node = NULL;
	}

	if ( node )
	{
		IPEdit_Type = ( node->ipn_Type == IPT_Block ) ? 1 : 0;

		if ( node->ipn_A == -1 )
		{
			sprintf( IPEdit_StrBuffer1, "*" );
		}
		else
		{
			sprintf( IPEdit_StrBuffer1, "%d", node->ipn_A );
		}

		if ( node->ipn_B == -1 )
		{
			sprintf( IPEdit_StrBuffer2, "*" );
		}
		else
		{
			sprintf( IPEdit_StrBuffer2, "%d", node->ipn_B );
		}

		if ( node->ipn_C == -1 )
		{
			sprintf( IPEdit_StrBuffer3, "*" );
		}
		else
		{
			sprintf( IPEdit_StrBuffer3, "%d", node->ipn_C );
		}

		if ( node->ipn_D == -1 )
		{
			sprintf( IPEdit_StrBuffer4, "*" );
		}
		else
		{
			sprintf( IPEdit_StrBuffer4, "%d", node->ipn_D );
		}
	}
	else
	{
		IPEdit_Type = 0;	// Include
		IPEdit_StrBuffer1[0] = 0;
		IPEdit_StrBuffer2[0] = 0;
		IPEdit_StrBuffer3[0] = 0;
		IPEdit_StrBuffer4[0] = 0;
	}

	// --

	GUIObjects[ GID_Window ] = NewObject( WindowClass, NULL,
		WA_Activate,								TRUE,
		WA_CloseGadget,								TRUE,
		WA_DepthGadget,								TRUE,
		WA_DragBar,									TRUE,
		WA_SizeGadget,								TRUE,
		WA_Title,									"rVNCd - IP",
		WA_BusyPointer,								cfg->cfg_WinData[WIN_IPEdit].Busy > 0,

		( cfg->cfg_WinData[WIN_IPEdit].Width == 0 ) ?
		TAG_IGNORE : WA_Left, cfg->cfg_WinData[WIN_IPEdit].XPos,

		( cfg->cfg_WinData[WIN_IPEdit].Width == 0 ) ?
		TAG_IGNORE : WA_Top, cfg->cfg_WinData[WIN_IPEdit].YPos,

		WA_Width, ( cfg->cfg_WinData[WIN_IPEdit].Width != 0 ) ?
		cfg->cfg_WinData[WIN_IPEdit].Width : 350,

		WA_Height, ( cfg->cfg_WinData[WIN_IPEdit].Width != 0 ) ?
		cfg->cfg_WinData[WIN_IPEdit].Height : 80,

		( cfg->cfg_WinData[WIN_IPEdit].Width != 0 ) ?
		TAG_IGNORE : WINDOW_Position, WPOS_CENTERSCREEN,

//		WA_PubScreen,								gs->up_PubScreen,
		WINDOW_AppPort,								& WinAppPort.vmp_MsgPort,
		WINDOW_SharedPort,							& WinMsgPort.vmp_MsgPort,
		WINDOW_PopupGadget,							TRUE,
		WINDOW_Icon,								ProgramIcon,
		WINDOW_IconTitle,							"rVNCd IP",
		WINDOW_IconNoDispose,						TRUE,
		WINDOW_IconifyGadget,						TRUE,
//		WINDOW_MenuStrip,							MainMenuStrip,
//		WINDOW_MenuUserData,						WGUD_HOOK,
//		WINDOW_Position,							WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,							GUIObjects[ GID_Root ] = NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
			End,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_BevelStyle,					BVS_SBAR_VERT,
			End,
			CHILD_MinHeight,						3+5,
			CHILD_MaxHeight,						3+5,

//			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
//			End,
//			CHILD_MinHeight,						5,
//			CHILD_MaxHeight,						5,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,					GUIObjects[ GID_Type ] = NewObject( ChooserClass, NULL,
					GA_ID,							GID_Type,
					GA_RelVerify,					TRUE,
					CHOOSER_LabelArray,				IPTypes,
					CHOOSER_Selected,				IPEdit_Type,
				End,
				CHILD_MinWidth,						150,
				CHILD_WeightedWidth,				0,
				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				End,
			End,
			CHILD_Label,							NewObject( LabelClass, NULL,
				LABEL_Text,							"Type",
			End,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
			End,
			CHILD_MinHeight,						5,
			CHILD_MaxHeight,						5,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Str1 ] = NewObject( StringClass, NULL,
					GA_ID,							GID_Str1,
					GA_RelVerify,					TRUE,
					GA_TabCycle,					TRUE,
					STRINGA_MaxChars,				5,
					STRINGA_TextVal,				IPEdit_StrBuffer1,
				End,
				CHILD_WeightedWidth,				25,

				LAYOUT_AddChild,					GUIObjects[ GID_Str2 ] = NewObject( StringClass, NULL,
					GA_ID,							GID_Str2,
					GA_RelVerify,					TRUE,
					GA_TabCycle,					TRUE,
					STRINGA_MaxChars,				5,
					STRINGA_TextVal,				IPEdit_StrBuffer2,
				End,
				CHILD_WeightedWidth,				25,

				LAYOUT_AddChild,					GUIObjects[ GID_Str3 ] = NewObject( StringClass, NULL,
					GA_ID,							GID_Str3,
					GA_RelVerify,					TRUE,
					GA_TabCycle,					TRUE,
					STRINGA_MaxChars,				5,
					STRINGA_TextVal,				IPEdit_StrBuffer3,
				End,
				CHILD_WeightedWidth,				25,

				LAYOUT_AddChild,					GUIObjects[ GID_Str4 ] = NewObject( StringClass, NULL,
					GA_ID,							GID_Str4,
					GA_RelVerify,					TRUE,
					GA_TabCycle,					TRUE,
					STRINGA_MaxChars,				5,
					STRINGA_TextVal,				IPEdit_StrBuffer4,
				End,
				CHILD_WeightedWidth,				25,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				End,
			End,
			CHILD_WeightedHeight,					0,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				End,

				LAYOUT_AddChild,					NewObject( ButtonClass, NULL,
					GA_Text,						"* = wildcard",
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_NONE,
					BUTTON_Transparent,				TRUE,
					BUTTON_Justification,			BCJ_CENTER,
				End,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				End,
			End,
			CHILD_WeightedHeight,					0,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_BevelStyle,					BVS_SBAR_VERT,
			End,
			CHILD_MinHeight,						3,
			CHILD_MaxHeight,						3,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
			End,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,				30,

				LAYOUT_AddChild,					GUIObjects[ GID_Update ] = NewObject( ButtonClass, NULL,
					GA_ID,							GID_Update,
					GA_Text,						( cfg->cfg_IPEditStat == IPES_Add ) ? "Add" : "Update",
					GA_RelVerify,					TRUE,
					GA_Disabled,					myIPisValid( cfg ) ? FALSE : TRUE,
					BUTTON_Justification,			BCJ_CENTER,
				End,
				CHILD_WeightedWidth,				40,

				LAYOUT_AddChild,					GUIObjects[ GID_Cancel ] = NewObject( ButtonClass, NULL,
					GA_ID,							GID_Cancel,
					GA_Text,						"Cancel",
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
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating IPEditLogger GUI Object" );
		goto bailout;
	}

	// --

	if ( cfg->cfg_WinData[WIN_IPEdit].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_IPEdit].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_IPEdit].WindowAdr == NULL )
		{
			goto bailout;
		}

//		cfg->cfg_WinData[WIN_IPEdit].WindowAdr->UserData = (PTR) RAGUI_HandleMainWindow;

//		GetAttrs( GUIObjects[ GID_Window ], 
//			WINDOW_SigMask, & IPEditWindowBits,
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

void RAGUI_CloseIPEditWindow( struct Config *cfg )
{
struct Window *win;

	if ( ! GUIObjects[ GID_Window ] )
	{
		goto bailout;
	}

	win = cfg->cfg_WinData[WIN_IPEdit].WindowAdr;

	if ( win )
	{
		cfg->cfg_WinData[WIN_IPEdit].XPos = win->LeftEdge;
		cfg->cfg_WinData[WIN_IPEdit].YPos = win->TopEdge;
		cfg->cfg_WinData[WIN_IPEdit].Width = win->Width - win->BorderLeft - win->BorderRight;
		cfg->cfg_WinData[WIN_IPEdit].Height = win->Height - win->BorderTop - win->BorderBottom;
	}

	DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_IPEdit].WindowAdr = NULL;

	RAGUI_CloseFont( cfg );

bailout:

	return;
}

// --

static void myAdd( struct Config *cfg )
{
struct IPNode *node;
U32 type;

	if ( ! myIPisValid( cfg ))
	{
//		SHELLBUF_PRINTF( "%s:%04d: Error 1\n", __FILE__, __LINE__ );
		return;
	}

	node = mem_AllocClr( sizeof( struct IPNode ));

	if ( ! node )
	{
//		SHELLBUF_PRINTF( "%s:%04d: Error 2\n", __FILE__, __LINE__ );
		return;
	}

	type = 0;

	GetAttrs( GUIObjects[ GID_Type ],
		CHOOSER_Selected, & type,
		TAG_END
	);

	/**/ if ( type == 0 )	node->ipn_Type = IPT_Allow;
	else if ( type == 1 )	node->ipn_Type = IPT_Block;
	else					node->ipn_Type = IPT_Unknown;

	node->ipn_A = StringData[0];
	node->ipn_B = StringData[1];
	node->ipn_C = StringData[2];
	node->ipn_D = StringData[3];

	ObtainSemaphore( & cfg->IPSema );
	List_AddTail( & cfg->IPList, (PTR) node );
	ReleaseSemaphore( & cfg->IPSema );

	myGUI_Main_UpdateIPList( cfg );
	Page_Refresh_IP( cfg );
}

// --

static void myEdit( struct Config *cfg )
{
struct IPNode *node;
U32 type;

	if ( ! myIPisValid( cfg ))
	{
//		SHELLBUF_PRINTF( "%s:%04d: Error 1\n", __FILE__, __LINE__ );
		return;
	}

	if ( cfg->cfg_IPEditNode )
	{
		node = (PTR) cfg->cfg_IPEditNode->ln_Name;
	}
	else
	{
		node = NULL;
	}

	if ( ! node )
	{
//		SHELLBUF_PRINTF( "%s:%04d: Error 2\n", __FILE__, __LINE__ );
		return;
	}

	type = 0;

	GetAttrs( GUIObjects[ GID_Type ],
		CHOOSER_Selected, & type,
		TAG_END
	);

	/**/ if ( type == 0 )	node->ipn_Type = IPT_Allow;
	else if ( type == 1 )	node->ipn_Type = IPT_Block;
	else					node->ipn_Type = IPT_Unknown;

	node->ipn_A = StringData[0];
	node->ipn_B = StringData[1];
	node->ipn_C = StringData[2];
	node->ipn_D = StringData[3];

	myGUI_Main_UpdateIPList( cfg );
	Page_Refresh_IP( cfg );
}

// --

void RAGUI_HandleIPEditWindow( struct Config *cfg )
{
U32 result;
U16 code;
S32 theend;
S32 update;

	theend = FALSE;
	update = FALSE;

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
				cfg->cfg_WinData[WIN_IPEdit].Status = WINSTAT_Iconifyed;
				if ( IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_IPEdit].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_IPEdit].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_IPEdit].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
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
						if ( myIPisValid( cfg ))
						{
							update = TRUE;
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
						mySetTags( cfg, GUIObjects[ GID_Update ],
							GA_Disabled, myIPisValid( cfg ) ? FALSE : TRUE,
							TAG_END
						);
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
		if ( update )
		{
			if ( cfg->cfg_IPEditStat == IPES_Add )
			{
				myAdd( cfg );
			}

			if ( cfg->cfg_IPEditStat == IPES_Edit )
			{
				myEdit( cfg );
			}
		}

		if ( cfg->cfg_IPEditStat != IPES_Unused )
		{
			if ( cfg->cfg_WinData[WIN_Main].BusyWin )
			{
				 cfg->cfg_WinData[WIN_Main].BusyWin( cfg, FALSE );
			}
			cfg->cfg_IPEditStat = IPES_Unused;
		}

		cfg->cfg_WinData[WIN_IPEdit].Status = WINSTAT_Close;
		RAGUI_CloseIPEditWindow( cfg );
	}
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

static S32 myIPisValid( struct Config *cfg UNUSED )
{
STR str;
S32 valid;
S32 val;
S32 cnt;
S32 id;
S32 pos;
S32 c;

	valid = FALSE;

	// --  --

	for( cnt=0 ; cnt<4 ; cnt++ )
	{
		id = StringIDs[cnt];

		str	= NULL;

		GetAttrs( GUIObjects[ id ],
			STRINGA_TextVal, & str,
			TAG_END
		);

		if (( ! str ) || ( ! *str ))
		{
			goto bailout;
		}

		if (( str[0] != '*' ) || ( str[1] != 0 ))
		{
			pos = 0;
			val = 0;

			while( TRUE )
			{
				c = str[pos++];

				if ( ! c )
				{
					break;
				}

				if (( c < 0x30 ) ||( c > 0x39 ))
				{
					goto bailout;
				}

				val = ( val * 10 ) + ( c - 0x30 );
			}

			if (( val < 0 ) || ( val > 255 ))
			{
				goto bailout;
			}

			StringData[cnt] = val;
		}
		else
		{
			StringData[cnt] = -1;
		}
	}

	// --  --

	valid = TRUE;

bailout:

	return( valid );
}

// --

#endif // GUI_RA
