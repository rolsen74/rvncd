
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if 0

struct MouseMessage
{
	U8	mm_Type;
	U8	mm_Buttons;
	S16	mm_X;
	S16	mm_Y;
};

#endif

// --

static U32 myMarkDirty( struct Config *cfg, U32 pos2 )
{
U32 tilenr;
S32 posx;
S32 posy;
S32 mx2;
S32 my2;
S32 mx;
S32 my;
S32 mw;
S32 mh;
S32 ts;
S32 x;
S32 y;

	tilenr = -1U;

	x = ( pos2 & 0x0000ffff ) >>  0;
	y = ( pos2 & 0xffff0000 ) >> 16;

	mx = ( x > 0 ) ? x : 0 ;
	my = ( y > 0 ) ? y : 0 ;
	mw = ( x > 0 ) ? cfg->cfg_PointerWidth  : cfg->cfg_PointerWidth  + x ;
	mh = ( y > 0 ) ? cfg->cfg_PointerHeight : cfg->cfg_PointerHeight + y ;

	ts = cfg->GfxRead_Screen_TileSize;

	my2 = ( my / ts ) * ts;

	while( TRUE )
	{
		posy = my2;

		if ( posy >= my + mh )
		{
			break;
		}

		if ( posy >= cfg->GfxRead_Screen_PageHeight )
		{
			break;
		}

		mx2 = ( mx / ts ) * ts;

		while( TRUE )
		{
			posx = mx2;

			if ( posx >= mx + mw )
			{
				break;
			}

			if ( posx >= cfg->GfxRead_Screen_PageWidth )
			{
				break;
			}

			tilenr  = ( posy / ts ) * ( cfg->GfxRead_Screen_TileWidth );
			tilenr += ( posx / ts );
			cfg->GfxRead_Screen_TileArrayBuffer[tilenr] = 0x80;

			mx2 += ts;
		}

		my2 += ts;
	}

	return( tilenr );
}

U32 myMarkDirtyXY( struct Config *cfg, S32 x, S32 y )
{
U32 tilenr;

	tilenr = myMarkDirty( cfg, (( y & 0xffff ) << 16 ) | ( x & 0xffff ));

//	#ifdef DEBUG
//	DebugPrintF( "Marked Tilenr #%lu (dirty)\n", tilenr );
//	#endif

	return( tilenr );
}

// --

S32 VNC_Mouse( struct Config *cfg )
{
struct IEPointerPixel Pointer;
struct MouseMessage *mm;
struct InputEvent Event;
struct IOStdReq *IOReq;
U32 pos;
S32 hardcursor;
S32 error;
S32 code;
S32 size;
S32 rc;
S32 v1;
S32 v2;

	mm = cfg->NetRead_ReadBuffer;

	error = TRUE;

	size = sizeof( struct MouseMessage );

	rc = Func_NetRead( cfg, mm, size, MSG_WAITALL );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	if (( mm->mm_Type != 5 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%ld != %ld)", rc, size );
		goto bailout;
	}

	// -- Check if our Active screen is Front Most

	IOReq = cfg->NetRead_InputIOReq;

	// -- Handle Mouse Position

	pos = (( mm->mm_Y << 16 ) | ( mm->mm_X ));

	if ( cfg->NetRead_OldMousePos != pos )
	{
		cfg->cfg_CurMouseX = mm->mm_X;
		cfg->cfg_CurMouseY = mm->mm_Y;

		Pointer.iepp_Screen		= cfg->GfxRead_Screen_Adr;
		Pointer.iepp_Position.X	= mm->mm_X;	// - xadjust;
		Pointer.iepp_Position.Y	= mm->mm_Y;	// - yadjust;

		if ( cfg->GfxRead_Screen_ViewMode == VIEWMODE_View )
		{
			if ( cfg->GfxRead_Screen_Adr->ViewPort.DxOffset < 0 )
			{
				Pointer.iepp_Position.X -= cfg->GfxRead_Screen_Adr->ViewPort.DxOffset;
			}

			if ( cfg->GfxRead_Screen_Adr->ViewPort.DyOffset < 0 )
			{
				Pointer.iepp_Position.Y -= cfg->GfxRead_Screen_Adr->ViewPort.DyOffset;
			}
		}

		memset( & Event, 0, sizeof( Event ));

		Event.ie_Class			= IECLASS_NEWPOINTERPOS;
		Event.ie_SubClass		= IESUBCLASS_PIXEL;
		Event.ie_Code			= IECODE_NOBUTTON;
		Event.ie_Qualifier		= cfg->NetRead_Qualifier;
		Event.ie_EventAddress	= & Pointer;

		IOReq->io_Command		= IND_WRITEEVENT;
		IOReq->io_Data			= & Event;
		IOReq->io_Length		= sizeof( Event );
		IOReq->io_Flags			= 0;

		DoIO( (PTR) IOReq );
	
		// are we Using Hardware Cursor ..
		hardcursor = (( cfg->cfg_Active_Settings.RichCursor ) && ( cfg->cfg_ServerSupportsCursor ) && ( cfg->NetSend_Encodings1[ ENCODE_RichCursor ] ));

		if ( ! hardcursor )
		{
			myMarkDirty( cfg, cfg->NetRead_OldMousePos );
			myMarkDirty( cfg, pos );
		}

		// -- Set new Pos
		cfg->NetRead_OldMousePos = pos;
	}

	// -- Handle Mouse Buttons

	if ( cfg->NetRead_OldMouseButtons != ( mm->mm_Buttons & 31 ))
	{
		//  1 = Left
		//  2 = Middle
		//  4 = Right
		//  8 = scroll up
		// 16 = scroll down

		// -- Left Mouse Button

		v1 = mm->mm_Buttons & 1;
		v2 = cfg->NetRead_OldMouseButtons & 1;

		if ( v1 )
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_LEFTBUTTON;
			code = IECODE_LBUTTON;
		}
		else
		{
			cfg->NetRead_Qualifier &= ~IEQUALIFIER_LEFTBUTTON;
			code = IECODE_LBUTTON | IECODE_UP_PREFIX;
		}

		if ( v1 != v2 )
		{
			memset( & Event, 0, sizeof( Event ));

			Event.ie_Class			= IECLASS_RAWMOUSE;
			Event.ie_Qualifier		= cfg->NetRead_Qualifier;
			Event.ie_Code			= code;

			IOReq->io_Command		= IND_WRITEEVENT;
			IOReq->io_Data			= & Event;
			IOReq->io_Length		= sizeof( Event );
			IOReq->io_Flags			= 0;

			DoIO( (PTR) IOReq );
		}

		// -- Middle Mouse Button

		v1 = mm->mm_Buttons & 2;
		v2 = cfg->NetRead_OldMouseButtons & 2;

		if ( v1 )
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_MIDBUTTON;
			code = IECODE_MBUTTON;
		}
		else
		{
			cfg->NetRead_Qualifier &= ~IEQUALIFIER_MIDBUTTON;
			code = IECODE_MBUTTON | IECODE_UP_PREFIX;
		}

		if ( v1 != v2 )
		{
			memset( & Event, 0, sizeof( Event ));

			Event.ie_Class			= IECLASS_RAWMOUSE;
			Event.ie_Qualifier		= cfg->NetRead_Qualifier;
			Event.ie_Code			= code;

			IOReq->io_Command		= IND_WRITEEVENT;
			IOReq->io_Data			= & Event;
			IOReq->io_Length		= sizeof( Event );
			IOReq->io_Flags			= 0;

			DoIO( (PTR) IOReq );
		}

		// -- Right Mouse Button

		v1 = mm->mm_Buttons & 4;
		v2 = cfg->NetRead_OldMouseButtons & 4;

		if ( v1 )
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_RBUTTON;
			code = IECODE_RBUTTON;
		}
		else
		{
			cfg->NetRead_Qualifier &= ~IEQUALIFIER_RBUTTON;
			code = IECODE_RBUTTON | IECODE_UP_PREFIX;
		}

		if ( v1 != v2 )
		{
			memset( & Event, 0, sizeof( Event ));

			Event.ie_Class			= IECLASS_RAWMOUSE;
			Event.ie_Qualifier		= cfg->NetRead_Qualifier;
			Event.ie_Code			= code;

			IOReq->io_Command		= IND_WRITEEVENT;
			IOReq->io_Data			= & Event;
			IOReq->io_Length		= sizeof( Event );
			IOReq->io_Flags			= 0;

			DoIO( (PTR) IOReq );
		}

		// -- Scroll Wheel

		#ifdef _AOS4_
		if ( mm->mm_Buttons & ( 8 | 16 ))
		{
			memset( & Event, 0, sizeof( Event ));

			Event.ie_Class			= IECLASS_MOUSEWHEEL;
			Event.ie_Qualifier		= cfg->NetRead_Qualifier;
			Event.ie_Y				= ( mm->mm_Buttons & 8 ) ? -1 : 1 ;

			IOReq->io_Command		= IND_WRITEEVENT;
			IOReq->io_Data			= & Event;
			IOReq->io_Length		= sizeof( Event );
			IOReq->io_Flags			= 0;

			DoIO( (PTR) IOReq );
		}
		#endif

		cfg->NetRead_OldMouseButtons = ( mm->mm_Buttons & 31 );
	}

	error = FALSE;

bailout:

	return( error );
}

// --
