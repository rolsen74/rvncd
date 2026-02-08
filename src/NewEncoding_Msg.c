
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

enum UpdateStat NewEncoding_Msg( struct Config *cfg, struct UpdateNode *un )
{
struct CommandEncoding *msg;
struct myEncNode tmp;
enum UpdateStat stat;
U32 *enc;
U32 cnt2;
U32 cnt;
U32 cur;

	stat = US_Error;

	enc = un->un_Buffer;

	// --

	memset( & cfg->NetSend_Encodings1, 0, ENCODE_LAST * sizeof( U8 ));
	memset( & cfg->NetSend_Encodings2, 0, ENCODE_LAST * sizeof( struct myEncNode ));

	for( cnt=0 ; cnt<ENCODE_LAST ; cnt++ )
	{
		cfg->NetSend_Encodings2[cnt].Type = cnt;
	}

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF( "Client Supported Encodings\n\n" );
	}

	cfg->cfg_LastRect = FALSE;

	for( cnt=0 ; cnt<un->un_Data.encoding.em_Encodings ; cnt++ )
	{
		if ( DoVerbose > 0 )
		{
			#if 1
			#define DO_PRT		SHELLBUF_PRINTF
			#define DO_PRT1		SHELLBUF_PRINTF1
			#else
			#define DO_PRT		DEBUGPRINTF
			#define DO_PRT1		DEBUGPRINTF
			#endif

			DO_PRT1( " Encoding: %08" PRIx32 "", enc[cnt] );
			/**/ if ( enc[cnt] == 0 )			{ DO_PRT( " - Raw" ); }
			else if ( enc[cnt] == 1 )			{ DO_PRT( " - Copy Rect" ); }
			else if ( enc[cnt] == 2 )			{ DO_PRT( " - RRE" ); }
			else if ( enc[cnt] == 4 )			{ DO_PRT( " - CoRRE" ); }
			else if ( enc[cnt] == 5 )			{ DO_PRT( " - Hextile" ); }
			else if ( enc[cnt] == 6 )			{ DO_PRT( " - ZLib" ); }
			else if ( enc[cnt] == 7 )			{ DO_PRT( " - Tight" ); }
			else if ( enc[cnt] == 8 )			{ DO_PRT( " - ZlibHex" ); }
			else if ( enc[cnt] == 9 )			{ DO_PRT( " - Ultra" ); }
			else if ( enc[cnt] == 10 )			{ DO_PRT( " - Ultra2" ); }
			else if ( enc[cnt] == 15 )			{ DO_PRT( " - TRLE" ); }
			else if ( enc[cnt] == 16 )			{ DO_PRT( " - ZRLE" ); }
			else if ( enc[cnt] == 17 )			{ DO_PRT( " - ZYWRLE" ); }
			else if ( enc[cnt] == 22 )			{ DO_PRT( " - ZRLE2" ); }
			else if ( enc[cnt] == 24 )			{ DO_PRT( " - RealVNC" ); }
			else if ( enc[cnt] == 0xC0A1E5CE )	{ DO_PRT( " - Extended Clipboard" ); }
			else if ( enc[cnt] == 0xFFFF0001 )	{ DO_PRT( " - Cache Enable" ); }
			else if ( enc[cnt] == 0xFFFF8000 )	{ DO_PRT( " - Server State" ); }
			else if ( enc[cnt] == 0xFFFF8001 )	{ DO_PRT( " - Enable Keep Alive" ); }
			else if ( enc[cnt] == 0xFFFF8002 )	{ DO_PRT( " - FTProtocol Version" ); }
			else if ( enc[cnt] == 0xFFFF8003 )	{ DO_PRT( " - Session" ); }
			else if ( enc[cnt] == 0xFFFFFECC )	{ DO_PRT( " - Extended Desktop Size" ); }
			else if ( enc[cnt] == 0xFFFFFF10 )	{ DO_PRT( " - X Cursor" ); }
			else if ( enc[cnt] == 0xFFFFFF11 )	{ DO_PRT( " - Rich Cursor" ); }
			else if ( enc[cnt] == 0xFFFFFF18 )	{ DO_PRT( " - Mouse Pos" ); }
			else if ( enc[cnt] == 0xFFFFFF20 )	{ DO_PRT( " - Last Rect" ); }
			else if ( enc[cnt] == 0xFFFFFF21 )	{ DO_PRT( " - New FB Size" ); }
			DO_PRT( "\n" );
		}

		if ( enc[cnt] == 0 ) // Raw
		{
			cfg->NetSend_Encodings1[ENCODE_Raw] = TRUE;
			cfg->NetSend_Encodings2[ENCODE_Raw].Pos = cnt;
			cfg->NetSend_Encodings2[ENCODE_Raw].Enabled = TRUE;
			cfg->NetSend_Encodings2[ENCODE_Raw].EncType = ( enc[cnt] & 0x80000000 ) ? FALSE : TRUE;
			continue;
		}

		if ( enc[cnt] == 2 ) // RRE
		{
			cfg->NetSend_Encodings1[ENCODE_RRE] = TRUE;
			cfg->NetSend_Encodings2[ENCODE_RRE].Pos = cnt;
			cfg->NetSend_Encodings2[ENCODE_RRE].Enabled = TRUE;
			cfg->NetSend_Encodings2[ENCODE_RRE].EncType = ( enc[cnt] & 0x80000000 ) ? FALSE : TRUE;
			continue;
		}

		#ifdef HAVE_ZLIB
		if ( enc[cnt] == 6 ) // ZLib
		{
			cfg->NetSend_Encodings1[ENCODE_ZLib] = TRUE;
			cfg->NetSend_Encodings2[ENCODE_ZLib].Pos = cnt;
			cfg->NetSend_Encodings2[ENCODE_ZLib].Enabled = TRUE;
			cfg->NetSend_Encodings2[ENCODE_ZLib].EncType = ( enc[cnt] & 0x80000000 ) ? FALSE : TRUE;
			continue;
		}
		#endif // HAVE_ZLIB

		if ( enc[cnt] == 0xFFFFFF11 ) // Rich Cursor
		{
			cfg->NetSend_Encodings1[ENCODE_RichCursor] = TRUE;
			cfg->NetSend_Encodings2[ENCODE_RichCursor].Pos = cnt;
			cfg->NetSend_Encodings2[ENCODE_RichCursor].Enabled = TRUE;
			cfg->NetSend_Encodings2[ENCODE_RichCursor].EncType = ( enc[cnt] & 0x80000000 ) ? FALSE : TRUE;
			cfg->cfg_UpdateCursor = TRUE;
			continue;
		}

		if ( enc[cnt] == 0xFFFFFF20 ) // Last Rect
		{
			cfg->cfg_LastRect = TRUE;
			continue;
		}
	}

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF( "\n" );
	}

	// Raw must always be valid
	if ( ! cfg->NetSend_Encodings1[ENCODE_Raw] )
	{
		cfg->NetSend_Encodings1[ENCODE_Raw] = TRUE;
		cfg->NetSend_Encodings2[ENCODE_Raw].Pos = cnt;
		cfg->NetSend_Encodings2[ENCODE_Raw].Enabled = TRUE;
		cfg->NetSend_Encodings2[ENCODE_Raw].EncType = TRUE;
	}

	// -- Sort Encodings

	for( cnt=0 ; cnt<ENCODE_LAST ; cnt++ )
	{
		cur = cnt;

		for( cnt2=cnt+1 ; cnt2<ENCODE_LAST ; cnt2++ )
		{
			if ( cfg->NetSend_Encodings2[ cur ].Enabled != cfg->NetSend_Encodings2[ cnt2 ].Enabled )
			{
				if ( cfg->NetSend_Encodings2[ cur ].Enabled < cfg->NetSend_Encodings2[ cnt2 ].Enabled )
				{
					cur = cnt2;
				}
			}
			else if ( cfg->NetSend_Encodings2[ cur ].EncType != cfg->NetSend_Encodings2[ cnt2 ].EncType )
			{
				if ( cfg->NetSend_Encodings2[ cur ].EncType < cfg->NetSend_Encodings2[ cnt2 ].EncType )
				{
					cur = cnt2;
				}
			}
			else if ( cfg->NetSend_Encodings2[ cur ].Pos > cfg->NetSend_Encodings2[ cnt2 ].Pos )
			{
				cur = cnt2;
			}
		}

		if ( cnt != cur )
		{
			tmp = cfg->NetSend_Encodings2[ cnt ];
			/***/ cfg->NetSend_Encodings2[ cnt ] = cfg->NetSend_Encodings2[ cur ];
			/***/ cfg->NetSend_Encodings2[ cur ] = tmp;
		}
	}

	// --
	// Update GUI

	msg = mem_AllocClr( sizeof( struct CommandEncoding ) + un->un_Data.encoding.em_Encodings * sizeof( U32 ));

	if ( msg )
	{
		msg->ce_Command = CMD_Encoding;
		msg->ce_Encodings = un->un_Data.encoding.em_Encodings;

		memcpy( & msg->ce_Data[0], enc, un->un_Data.encoding.em_Encodings * sizeof( U32 ) );

		MsgPort_PutMsg( & CmdMsgPort, & msg->ce_Message );
	}

	// --

	stat = US_Okay;			// No wait

	return( stat );
}

// --





#if 0

// -- Pauls client 1

Encoding: 00000007 - Tight
Encoding: fffffecb
Encoding: fffffefe
Encoding: fffffefb
Encoding: fffffecc
Encoding: ffffff21 - NewFBSize
Encoding: fffffecd
Encoding: ffffff20
Encoding: 574d5669
Encoding: fffffefd
Encoding: fffffec6
Encoding: ffffff11 - RichCursor
Encoding: ffffff10
Encoding: fffffeff
Encoding: 00000010 - ZRLE
Encoding: 00000005 - Hextile
Encoding: 00000002 - RRE
Encoding: 00000001 - CopyRect
Encoding: 00000000 - Raw

// -- Pauls client 2

 Encoding: 00000007 - Tight
 Encoding: 00000010 - ZRLE
 Encoding: 00000009 - ZLibHex
 Encoding: ffff0009
 Encoding: 00000001 - CopyRect
 Encoding: 00000005 - Hextile
 Encoding: 00000006 - ZLib
 Encoding: 00000004 - CoRRE
 Encoding: 00000002 - RRE
 Encoding: 00000000 - Raw
 Encoding: ffffff03
 Encoding: ffffffe5
 Encoding: ffffff10
 Encoding: ffffff11 - RichCursor
 Encoding: ffffff18 - MousePos
 Encoding: fffe0000
 Encoding: ffffff21 - NewFBSize
 Encoding: fffffecc
 Encoding: ffffff20
 Encoding: fffe0001
 Encoding: fffe0002
 Encoding: fffe0003
 Encoding: fffffecb
 Encoding: fffffefe
 Encoding: c0a1e5ce

// -- Pauls client 3?

 Encoding: 00000007 - Tight
 Encoding: 00000010 - ZRLE
 Encoding: 00000006 - ZLib
 Encoding: 00000008 - Ultra
 Encoding: 00000005 - Hextile
 Encoding: 00000004 - CoRRE
 Encoding: 00000002 - RRE
 Encoding: 00000000 - Raw
 Encoding: 00000001 - CopyRect
 Encoding: ffffff10
 Encoding: ffffff11 - RichCursor
 Encoding: ffffff18 - MousePos
 Encoding: ffffff20
 Encoding: ffffff21 - NewFBSize

// -- RealVNC

VNC_SetEncoding
Type ......... : 2
Pad .......... : 0
Encodings .... : 13
Encoding: 00000018
Encoding: 0000000F - TRLE
Encoding: 00000005 - Hextile
Encoding: 00000010 - ZRLE
Encoding: 00000016
Encoding: 00000015
Encoding: 00000006 - ZLib
Encoding: 00000002 - RRE
Encoding: 00000000 - Raw
Encoding: 00000001 - CopyRect
Encoding: FFFFFEC6
Encoding: FFFFFF11 - RichCursor
Encoding: FFFFFF21 - NewFBSize

// -- UltraVNC

VNC_SetEncoding
Type ......... : 2
Pad .......... : 0
Encodings .... : 32
Encoding: 00000010 - ZRLE
Encoding: 0000001D
Encoding: 0000001B
Encoding: 0000001A
Encoding: 00000019
Encoding: 00000013
Encoding: 00000012
Encoding: 00000011 - ZYWRLE
Encoding: 0000000A
Encoding: 00000009 - ZLibHex
Encoding: 00000008 - Ultra
Encoding: 00000007 - Tight
Encoding: 00000006 - ZLib
Encoding: 00000005 - Hextile
Encoding: 00000004 - CoRRE
Encoding: 00000002 - RRE
Encoding: 00000001 - CopyRect
Encoding: 00000000 - Raw
Encoding: FFFFFF06
Encoding: FFFFFF11 - RichCursor
Encoding: FFFFFF18
Encoding: FFFFFFE8
Encoding: FFFF000B
Encoding: FFFFFF20 - LastRect
Encoding: FFFFFF21 - NewFBSize
Encoding: FFFFFECC
Encoding: FFFF8000
Encoding: FFFF8001
Encoding: FFFF8004
Encoding: FFFF8002
Encoding: FFFF8003
Encoding: C0A1E5CE

// -- TightVNC

VNC_SetEncoding
Type ......... : 2
Pad .......... : 0
Encodings .... : 11
Encoding: 00000007 - Tight
Encoding: 00000001 - CopyRect
Encoding: 00000010 - ZRLE
Encoding: 00000005 - Hextile
Encoding: 00000002 - RRE
Encoding: 00000000 - Raw
Encoding: FFFFFFE6 - Quality Level for JPEG
Encoding: FFFFFF21 - NewFBSize
Encoding: FFFFFF20 - LastRect
Encoding: FFFFFF18 - PointerPos
Encoding: FFFFFF11 - RichCursor

// -- MobaXterm

Encoding: 00000007 - Tight
Encoding: 00000005 - Hextile
Encoding: 00000001 - CopyRect
Encoding: 00000000 - Raw
Encoding: FFFFFF01 - Compression Level FF00 - FF09
Encoding: FFFFFF10 - XCursor
Encoding: FFFFFF11 - RichCursor
Encoding: FFFFFF18 - MousePos
Encoding: FFFFFD00 - Subsampling Level FD00 - FD05
Encoding: FFFFFF20 - LastRect
Encoding: FFFFFF21 - NewFBSize

#endif
