
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

void myGUI_SessionRefresh( struct Config *cfg );

// --

Object *Session_GUIObjects[GID_LAST];

// --

void VARARGS68K Session_SetTags( struct Config *cfg, PTR object, ... )
{
va_list ap;

	va_start( ap, object );

	// Not sure the WinAdr check is needed
	if ( cfg->cfg_WinData[WIN_Session].WindowAdr )
	{
		SetPageGadgetAttrsA( 
			object,
			GUIObjects[ GID_Root ],
			cfg->cfg_WinData[WIN_Session].WindowAdr,
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



Object *Session_Page_User( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,							LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,							NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,							NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
//				LAYOUT_BevelStyle,					BVS_GROUP,

				LAYOUT_AddChild,					GUIObjects[ GID_Session_IP ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"IP Address",
				End,

				LAYOUT_AddChild,					GUIObjects[ GID_Session_Connect ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
//					GA_Text,						"00:00:00 - 00-000-0000",
					BUTTON_BevelStyle,				BVS_THIN,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Connected at",
				End,
				CHILD_MinWidth,						160,

				LAYOUT_AddChild,					GUIObjects[ GID_Session_Duration ] = NewObject( ButtonClass, NULL,
					GA_ReadOnly,					TRUE,
					BUTTON_BevelStyle,				BVS_THIN,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Duration",
				End,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,				LAYOUT_ORIENT_HORIZ,

					LAYOUT_AddChild,				GUIObjects[ GID_Session_Pxl_Min ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_Session_Pxl_Avr ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_Session_Pxl_Max ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
					End,
				End,
				CHILD_Label,						NewObject( LabelClass, NULL,
					LABEL_Text,						"Pixel Speed",
				End,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				End,
				CHILD_MinHeight,					10,
				CHILD_MaxHeight,					10,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,				LAYOUT_ORIENT_HORIZ,

					LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
						LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,

						LAYOUT_AddChild,			GUIObjects[ GID_Session_Pixels ] = NewObject( ButtonClass, NULL,
							GA_ReadOnly,			TRUE,
							BUTTON_BevelStyle,		BVS_THIN,
						End,
						CHILD_Label,				NewObject( LabelClass, NULL,
							LABEL_Text,				"Pixels Send",
						End,

						LAYOUT_AddChild,			GUIObjects[ GID_Session_Read ] = NewObject( ButtonClass, NULL,
							GA_ReadOnly,			TRUE,
							BUTTON_BevelStyle,		BVS_THIN,
						End,
						CHILD_Label,				NewObject( LabelClass, NULL,
							LABEL_Text,				"Bytes Read",
						End,

						LAYOUT_AddChild,			GUIObjects[ GID_Session_Send ] = NewObject( ButtonClass, NULL,
							GA_ReadOnly,			TRUE,
							BUTTON_BevelStyle,		BVS_THIN,
						End,
						CHILD_Label,				NewObject( LabelClass, NULL,
							LABEL_Text,				"Bytes Send",
						End,
					End,

					LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
						LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,

						LAYOUT_AddChild,			GUIObjects[ GID_Session_Load ] = NewObject( ButtonClass, NULL,
							GA_ReadOnly,			TRUE,
							BUTTON_BevelStyle,		BVS_THIN,
						End,
						CHILD_Label,				NewObject( LabelClass, NULL,
							LABEL_Text,				"Load",
						End,

						#if 0
						LAYOUT_AddChild,			GUIObjects[ GID_Session_Pxl_Avr ] = NewObject( ButtonClass, NULL,
							GA_ReadOnly,			TRUE,
							BUTTON_BevelStyle,		BVS_THIN,
						End,
						CHILD_Label,				NewObject( LabelClass, NULL,
							LABEL_Text,				"Avr",
						End,

						LAYOUT_AddChild,			GUIObjects[ GID_Session_Pxl_Max ] = NewObject( ButtonClass, NULL,
							GA_ReadOnly,			TRUE,
							BUTTON_BevelStyle,		BVS_THIN,
						End,
						CHILD_Label,				NewObject( LabelClass, NULL,
							LABEL_Text,				"Max",
						End,
						#endif
					End,
				End,
				CHILD_WeightedHeight,				0,
			End,

			LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,					LAYOUT_ORIENT_HORIZ,
				LAYOUT_BevelStyle,					BVS_GROUP,
				LAYOUT_Label,						"Tile Info",

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
					LAYOUT_AddChild,				GUIObjects[ GID_Session_Tiles ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
					End,
					CHILD_Label,					NewObject( LabelClass, NULL,
						LABEL_Text,					"Total",
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_Session_Rich ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
					End,
					CHILD_Label,					NewObject( LabelClass, NULL,
						LABEL_Text,					"Rich",
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_Session_Raw ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
					End,
					CHILD_Label,					NewObject( LabelClass, NULL,
						LABEL_Text,					"Raw",
					End,
//					LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
//					End,
//					CHILD_WeightedHeight,			0,
				End,
				CHILD_WeightedWidth,				500,

				LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
					LAYOUT_AddChild,				GUIObjects[ GID_Session_RRE ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
					End,
					CHILD_Label,					NewObject( LabelClass, NULL,
						LABEL_Text,					"RRE",
					End,

					LAYOUT_AddChild,				GUIObjects[ GID_Session_ZLib ] = NewObject( ButtonClass, NULL,
						GA_ReadOnly,				TRUE,
						BUTTON_BevelStyle,			BVS_THIN,
					End,
					CHILD_Label,					NewObject( LabelClass, NULL,
						LABEL_Text,					"ZLib",
					End,
					LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
					End,
//					CHILD_WeightedHeight,			0,
				End,
				CHILD_WeightedWidth,				500,
			End,
//			CHILD_WeightedWidth,						0,
			CHILD_WeightedHeight,						0,
		End,

		LAYOUT_AddChild,								NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void RAGUI_BusySessionWindow( struct Config *cfg, S32 val )
{
	if ( val )
	{
		/**/ cfg->cfg_WinData[WIN_Session].Busy++;

		if ( cfg->cfg_WinData[WIN_Session].Busy == 1 )
		{
			mySetTags( cfg, GUIObjects[ GID_Window ],
				WA_BusyPointer, TRUE,
				TAG_END
			);
		}
	}
	else
	{
		if ( cfg->cfg_WinData[WIN_Session].Busy > 0 )
		{
			 cfg->cfg_WinData[WIN_Session].Busy--;

			if ( cfg->cfg_WinData[WIN_Session].Busy == 0 )
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

S32 RAGUI_OpenSessionWindow( struct Config *cfg UNUSED )
{
//struct ColumnInfo *ci;
//struct CommandSession *ck;
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
		if ( cfg->cfg_WinData[WIN_Session].WindowAdr == NULL )
		{
			if ( cfg->cfg_WinData[WIN_Session].Status == WINSTAT_Open )
			{
				cfg->cfg_WinData[WIN_Session].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

				if ( cfg->cfg_WinData[WIN_Session].WindowAdr )
				{
					myGUI_SessionRefresh( cfg );
				}
			}
		}
		else
		{
			WindowToFront( cfg->cfg_WinData[WIN_Session].WindowAdr );
			ActivateWindow( cfg->cfg_WinData[WIN_Session].WindowAdr );
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
		WA_Title,									"rVNCd - Session Infomation",
		WA_BusyPointer,								cfg->cfg_WinData[WIN_Session].Busy > 0,

		( cfg->cfg_WinData[WIN_Session].Width == 0 ) ?
		TAG_IGNORE : WA_Left, cfg->cfg_WinData[WIN_Session].XPos,

		( cfg->cfg_WinData[WIN_Session].Width == 0 ) ?
		TAG_IGNORE : WA_Top, cfg->cfg_WinData[WIN_Session].YPos,

		WA_Width, ( cfg->cfg_WinData[WIN_Session].Width != 0 ) ?
		cfg->cfg_WinData[WIN_Session].Width : 350,

		WA_Height, ( cfg->cfg_WinData[WIN_Session].Width != 0 ) ?
		cfg->cfg_WinData[WIN_Session].Height : 280,

		( cfg->cfg_WinData[WIN_Session].Width != 0 ) ?
		TAG_IGNORE : WINDOW_Position, WPOS_CENTERSCREEN,

//		WA_PubScreen,								gs->up_PubScreen,
		WINDOW_AppPort,								& WinAppPort.vmp_MsgPort,
		WINDOW_SharedPort,							& WinMsgPort.vmp_MsgPort,
		WINDOW_PopupGadget,							TRUE,
		WINDOW_Icon,								ProgramIcon,
		WINDOW_IconTitle,							"rVNCd Session",
		WINDOW_IconNoDispose,						TRUE,
		WINDOW_IconifyGadget,						TRUE,
//		WINDOW_MenuStrip,							MainMenuStrip,
//		WINDOW_MenuUserData,						WGUD_HOOK,
//		WINDOW_Position,							WPOS_CENTERSCREEN,
		WINDOW_ParentGroup,							GUIObjects[ GID_Root ] = NewObject( LayoutClass, NULL,

			LAYOUT_AddChild,						Session_Page_User( cfg ),

		End,
	End;

	if ( ! GUIObjects[ GID_Window ] )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Program: Error creating Session GUI Object" );
		goto bailout;
	}

	// --

	if ( cfg->cfg_WinData[WIN_Session].Status == WINSTAT_Open )
	{
		cfg->cfg_WinData[WIN_Session].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );

		if ( cfg->cfg_WinData[WIN_Session].WindowAdr == NULL )
		{
			goto bailout;
		}

		myGUI_SessionRefresh( cfg );
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

void RAGUI_CloseSessionWindow( struct Config *cfg )
{
struct Window *win;

	if ( ! GUIObjects[ GID_Window ] )
	{
		goto bailout;
	}

	win = cfg->cfg_WinData[WIN_Session].WindowAdr;

	if ( win )
	{
		cfg->cfg_WinData[WIN_Session].XPos = win->LeftEdge;
		cfg->cfg_WinData[WIN_Session].YPos = win->TopEdge;
		cfg->cfg_WinData[WIN_Session].Width = win->Width - win->BorderLeft - win->BorderRight;
		cfg->cfg_WinData[WIN_Session].Height = win->Height - win->BorderTop - win->BorderBottom;
	}

	DisposeObject( GUIObjects[ GID_Window ] );
	GUIObjects[ GID_Window ] = NULL;
	cfg->cfg_WinData[WIN_Session].WindowAdr = NULL;

bailout:

	return;
}

// --

void RAGUI_HandleSessionWindow( struct Config *cfg )
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
				cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Iconifyed;
				if ( IDoMethod( GUIObjects[ GID_Window ], WM_ICONIFY, NULL ))
				{
					cfg->cfg_WinData[WIN_Session].WindowAdr = NULL;
				}
				break;
			}

			case WMHI_UNICONIFY:
			{
				cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Open;
				cfg->cfg_WinData[WIN_Session].WindowAdr = (PTR) IDoMethod( GUIObjects[ GID_Window ], WM_OPEN, NULL );
				break;
			}

			case WMHI_RAWKEY:
			{
				switch( code & WMHI_KEYMASK )
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
		cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Close;
		RAGUI_CloseSessionWindow( cfg );
	}
}

// --

void Session_GetPixels( STR buf, S32 len, U64 val )
{
//	snprintf( buf, len, "%llu pxl/s", val );
	snprintf( buf, len, "%llu", val );
}

// --

void myGUI_SessionRefresh( struct Config *cfg )
{
	if ( cfg->cfg_SessionUsed )
	{
		static char Buf_Tiles_Total[32];
		static char Buf_Tiles_Rich[32];
		static char Buf_Tiles_ZLib[32];
		static char Buf_Tiles_RRE[32];
		static char Buf_Tiles_Raw[32];
		static char Buf_Duration[64];
		static char Buf_Connect[64];
		static char Buf_Pxl_Min[64];
		static char Buf_Pxl_Avr[64];
		static char Buf_Pxl_Max[64];
		static char Buf_Pixels[64];
		static char Buf_Read[32];
		static char Buf_Send[32];
		static char Buf_Load[32];
		static char Buf_IP[64];
		struct VNCTimeVal time_slice;		// Time passes since last
		struct VNCTimeVal time_wait;		// Time used for waiting
		struct VNCTimeVal time_cur;			// Current time
		U64 val_pixels;
		U64 pxl_last;
		U64 pxl_send;
		U32 cnt2;
		U32 cnt1;
		U64 val;
		U64 tile_send;
		S64 val_scaled;
		S64 val_slice;
		S64 val_wait;

		if ( cfg->cfg_ClientRunning )
		{
			cfg->SessionStatus.si_Pixels_Freq--;

			if ( cfg->SessionStatus.si_Pixels_Freq < 0 )
			{
				cfg->SessionStatus.si_Pixels_Freq = 2;

				// --

				GetSysTime( (PTR) & time_slice );
				time_cur		  = time_slice;
				SubTime(	(PTR) & time_slice, (PTR) & cfg->SessionStatus.si_Pixel_Time_Last );

				cfg->SessionStatus.si_Pixel_Time_Last = time_cur;

				if ( cfg->SessionStatus.si_Pixels_Delay > 0 )
				{
					cfg->SessionStatus.si_Pixels_Delay--;
				}
				else
				{
					Disable();
					time_wait = cfg->SessionStatus.si_Pixel_Time_Wait;
					cfg->SessionStatus.si_Pixel_Time_Wait.Microseconds = 0;
					cfg->SessionStatus.si_Pixel_Time_Wait.Seconds = 0;
					Enable();

					if ( CmpTime( (PTR) & time_slice, (PTR) & time_wait ) > 0 )
					{
						time_slice = time_wait;
					}

					val_slice	= ( time_slice.Seconds	* 1000000ULL ) + ( time_slice.Microseconds );
					val_wait	= ( time_wait.Seconds	* 1000000ULL ) + ( time_wait.Microseconds );

					tile_send	= cfg->SessionStatus.si_Tiles_Total;
					cfg->SessionStatus.si_Tiles_Last = tile_send;

					pxl_last 	= cfg->SessionStatus.si_Pixels_Last;
					pxl_send 	= cfg->SessionStatus.si_Pixels_Send;
					cfg->SessionStatus.si_Pixels_Last = pxl_send;

					// --
					val_pixels	= ( pxl_send - pxl_last );
					val_scaled	= ( val_pixels * 1000000ULL ) / ( val_slice );

					cfg->SessionStatus.si_Load = ( val_slice - val_wait ) * 1000 / val_slice;

					// --
					cfg->SessionStatus.si_Pixel_Pos++;
					if ( cfg->SessionStatus.si_Pixel_Pos >= PIXEL_Entries )
					{
						cfg->SessionStatus.si_Pixel_Pos = 0;
					}
					cfg->SessionStatus.si_Pixel_Data[cfg->SessionStatus.si_Pixel_Pos].Valid = TRUE;
					cfg->SessionStatus.si_Pixel_Data[cfg->SessionStatus.si_Pixel_Pos].Normal = val_scaled;
					// --

					cfg->SessionStatus.si_Pixel_Max = 0;
					cfg->SessionStatus.si_Pixel_Avr = 0;
					cfg->SessionStatus.si_Pixel_Min = UINT64_MAX;

					for( cnt1=cnt2=0 ; cnt2<PIXEL_Entries ; cnt2++ )
					{
						if ( cfg->SessionStatus.si_Pixel_Data[cnt2].Valid )
						{
							cnt1++;

							val = cfg->SessionStatus.si_Pixel_Data[cnt2].Normal;

							cfg->SessionStatus.si_Pixel_Avr += val;

							if ( cfg->SessionStatus.si_Pixel_Min > val )	cfg->SessionStatus.si_Pixel_Min = val;
							if ( cfg->SessionStatus.si_Pixel_Max < val )	cfg->SessionStatus.si_Pixel_Max = val;
						}
					}

					cfg->SessionStatus.si_Pixel_Avr /= cnt1;
					// --
				}
			}
		}

		LogTime_GetConnectTime(  & cfg->SessionStatus.si_LogTime, Buf_Connect,  sizeof( Buf_Connect  ));
		LogTime_GetDurationTime( & cfg->SessionStatus.si_LogTime, Buf_Duration, sizeof( Buf_Duration ));
		Session_GetProcent(	Buf_Tiles_Total,	sizeof( Buf_Tiles_Total ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Total );
		Session_GetProcent(	Buf_Tiles_Rich,		sizeof( Buf_Tiles_Rich ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Rich );
		Session_GetProcent(	Buf_Tiles_ZLib,		sizeof( Buf_Tiles_ZLib ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_ZLib );
		Session_GetProcent(	Buf_Tiles_RRE,		sizeof( Buf_Tiles_RRE ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_RRE );
		Session_GetProcent(	Buf_Tiles_Raw,		sizeof( Buf_Tiles_Raw ),	cfg->SessionStatus.si_Tiles_Total, cfg->SessionStatus.si_Tiles_Raw );
		Session_GetBytes(	Buf_Read,			sizeof( Buf_Read ),			cfg->SessionStatus.si_Read_Bytes );
		Session_GetBytes(	Buf_Send,			sizeof( Buf_Send ),			cfg->SessionStatus.si_Send_Bytes );
		Session_GetBytes(	Buf_Pixels,			sizeof( Buf_Pixels ),		cfg->SessionStatus.si_Pixels_Send );
		Session_GetPixels(	Buf_Pxl_Min,		sizeof( Buf_Pxl_Min ),		cfg->SessionStatus.si_Pixel_Min );
		Session_GetPixels(	Buf_Pxl_Avr,		sizeof( Buf_Pxl_Avr ),		cfg->SessionStatus.si_Pixel_Avr );
		Session_GetPixels(	Buf_Pxl_Max,		sizeof( Buf_Pxl_Max ),		cfg->SessionStatus.si_Pixel_Max );

		snprintf( Buf_Load, sizeof( Buf_Load ), "%d.%d%%", 
			cfg->SessionStatus.si_Load / 10 ,
			cfg->SessionStatus.si_Load % 10 );

		snprintf( Buf_IP, sizeof( Buf_IP ), "%d.%d.%d.%d", 
			cfg->SessionStatus.si_IPAddr[0] ,
			cfg->SessionStatus.si_IPAddr[1] ,
			cfg->SessionStatus.si_IPAddr[2] ,
			cfg->SessionStatus.si_IPAddr[3] );

		mySetTags( cfg, GUIObjects[ GID_Session_IP ],
			GA_Text, Buf_IP,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Pixels ],
			GA_Text, Buf_Pixels,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Read ],
			GA_Text, Buf_Read,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Send ],
			GA_Text, Buf_Send,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Pxl_Min ],
			GA_Text, Buf_Pxl_Min,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Pxl_Avr ],
			GA_Text, Buf_Pxl_Avr,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Pxl_Max ],
			GA_Text, Buf_Pxl_Max,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Load ],
			GA_Text, Buf_Load,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Tiles ],
			GA_Text, Buf_Tiles_Total,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Rich ],
			GA_Text, Buf_Tiles_Rich,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Raw ],
			GA_Text, Buf_Tiles_Raw,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_RRE ],
			GA_Text, Buf_Tiles_RRE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_ZLib ],
			GA_Text, Buf_Tiles_ZLib,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Connect ],
			GA_Text, Buf_Connect,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Duration ],
			GA_Text, Buf_Duration,
			TAG_END
		);
	}
	else
	{
		mySetTags( cfg, GUIObjects[ GID_Session_IP ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Pixels ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Read ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Send ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Pxl_Min ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Pxl_Avr ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Pxl_Max ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Load ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Tiles ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Rich ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Raw ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_RRE ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_ZLib ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Connect ],
			GA_Text, "--",
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Session_Duration ],
			GA_Text, "--",
			TAG_END
		);
	}
}

// --

void GUIFunc_SessionMessage( struct Config *cfg, struct CommandSession *msg UNUSED )
{
//	DebugPrintF( "RA: GUIFunc_SessionMessage\n" );

	if ( ! GUIObjects[ GID_Window ] )
	{
		return;
	}

	myGUI_SessionRefresh( cfg );
}

// --
// Note this is running in Main Task
// Called once pr. second

void RAGUI_TimerTick_Session( struct Config *cfg )
{
	if (( GUIObjects[ GID_Window ] ) 
	&&	( cfg->cfg_WinData[WIN_Session].WindowAdr )
	&&	( cfg->UserCount ))
	{
		myGUI_SessionRefresh( cfg );
	}
}

// --

#endif // GUI_RA
