
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if 0

struct PixelMessage
{
	U8	pm_Type;
	U8	pm_Pad;
	U8	pm_Pad2;
	U8	pm_Pad3;
	U8	pm_BitsPerPixel;
	U8	pm_Depth;
	U8	pm_BigEndian;
	U8	pm_TrueColor;
	U16	pm_RedMax;
	U16	pm_GreenMax;
	U16	pm_BlueMax;
	U8	pm_RedShift;
	U8	pm_GreenShift;
	U8	pm_BlueShift;
	U8	pm_Pad4;
	U8	pm_Pad5;
	U8	pm_Pad6;
};

#endif

// --

enum UpdateStat NewPixelFmt_Msg( struct Config *cfg, struct UpdateNode *un )
{
struct PixelMessage *buf;
struct PixelMessage *pm;
enum UpdateStat stat;

	stat = US_Error;

	//
	// Ultra VNC client, like to send SetPixelFormat and SetEncoding over
	// and over at random times .. so we only reactive if something change
	//

	buf = & un->un_Data.pixel;
	buf->pm_Pad		= 0;
	buf->pm_Pad2	= 0;
	buf->pm_Pad3	= 0;
	buf->pm_Pad4	= 0;
	buf->pm_Pad5	= 0;
	buf->pm_Pad6	= 0;

	pm = & cfg->GfxRead_Encode_ActivePixel;

	if ( memcmp( pm, buf, sizeof( struct PixelMessage )))
	{
		if ( DoVerbose > 0 )
		{
			SHELLBUF_LOCK();
//			SHELLBUF_PRINTF( "Updating PixelFormat\n\n" );
			SHELLBUF_PRINTF( "Client Request PixelFormat\n" );
			SHELLBUF_PRINTF( "\n" );
			SHELLBUF_PRINTF1( " BitsPerPixel.: %" PRId32 "\n", (S32) buf->pm_BitsPerPixel );
//			SHELLBUF_PRINTF1( " Pad..........: %" PRId32 "\n", (S32) buf->pm_Pad );
//			SHELLBUF_PRINTF1( " Pad2.........: %" PRId32 "\n", (S32) buf->pm_Pad2 );
//			SHELLBUF_PRINTF1( " Pad3.........: %" PRId32 "\n", (S32) buf->pm_Pad3 );
			SHELLBUF_PRINTF1( " Depth........: %" PRId32 "\n", (S32) buf->pm_Depth );
			SHELLBUF_PRINTF1( " BigEndian....: %" PRId32 "\n", (S32) buf->pm_BigEndian );
			SHELLBUF_PRINTF1( " TrueColor....: %" PRId32 "\n", (S32) buf->pm_TrueColor );
			SHELLBUF_PRINTF1( " RedMax.......: %" PRId32 "\n", (S32) buf->pm_RedMax );
			SHELLBUF_PRINTF1( " GreenMax.....: %" PRId32 "\n", (S32) buf->pm_GreenMax );
			SHELLBUF_PRINTF1( " BlueMax......: %" PRId32 "\n", (S32) buf->pm_BlueMax );
			SHELLBUF_PRINTF1( " RedShift.....: %" PRId32 "\n", (S32) buf->pm_RedShift );
			SHELLBUF_PRINTF1( " GreenShift...: %" PRId32 "\n", (S32) buf->pm_GreenShift );
			SHELLBUF_PRINTF1( " BlueShift....: %" PRId32 "\n", (S32) buf->pm_BlueShift );
//			SHELLBUF_PRINTF1( " Pad4.........: %" PRId32 "\n", (S32) buf->pm_Pad4 );
//			SHELLBUF_PRINTF1( " Pad5.........: %" PRId32 "\n", (S32) buf->pm_Pad5 );
//			SHELLBUF_PRINTF1( " Pad6.........: %" PRId32 "\n", (S32) buf->pm_Pad6 );
			SHELLBUF_PRINTF( "\n" );
			SHELLBUF_UNLOCK();
//			DEBUGPRINTF( DebugPrintF( "Updating PixelFormat\n\n" );
		}

		mySetEncoding_Message( cfg, buf, TRUE );
	}
	else
	{
		if ( DoVerbose > 1 )
		{
			SHELLBUF_PRINTF( "Keeping PixelFormat (Nothing changed)\n\n" );
		}
	}

	stat = US_Okay;			// No wait

	return( stat );
}

// --
