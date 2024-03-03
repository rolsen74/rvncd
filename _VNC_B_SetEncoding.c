 
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

/*
** Purpose:
** - Server handles a Mouse event from Client
**
** Returns:
** - True and the socket will be closed
** - False and we continue
*/

#pragma pack(1)

struct EncodingMessage
{
	uint8	em_Type;
	uint8	em_Pad;
	uint16	em_Encodings;
};

#pragma pack(0)

// --

int VNC_SetEncoding( struct Config *cfg )
{
struct CommandEncoding *msg;
struct EncodingMessage em;
struct SocketIFace *ISocket;
struct myEncNode tmp;
uint32 *enc;
void *buf;
int error;
int size;
int cnt2;
int cur;
int cnt;
int rc;

//	printf( "Got : VNC_SetEncoding\n" );

	ISocket = cfg->NetRead_ISocket;

	error = TRUE;

	buf = cfg->NetRead_ReadBuffer;

	enc = NULL;

	size = sizeof( struct EncodingMessage );

	rc = ISocket->recv( cfg->NetRead_ClientSocket, buf, size, MSG_WAITALL );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to read data (%d)", ISocket->Errno() );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "Failed to read data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		cfg->cfg_ServerRunning = FALSE;

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Read_Bytes += rc;

	memcpy( & em, buf, sizeof( struct EncodingMessage ));

	if (( em.em_Type != 2 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%d != %d)", rc, size );
		goto bailout;
	}

	// --

	if ( em.em_Encodings )
	{
		// todo reuse readbuffer if under 4k

		enc = myMalloc( em.em_Encodings * sizeof( uint32 ));

		if ( enc == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory (%ld Bytes)", em.em_Encodings * sizeof( uint32 ));
			goto bailout;
		}

		rc = ISocket->recv( cfg->NetRead_ClientSocket, enc, em.em_Encodings * sizeof( uint32 ), MSG_WAITALL );

		if ( rc == -1 )
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Failed to read data (%d)", ISocket->Errno() );
			}

			Log_PrintF( cfg, LOGTYPE_Error, "Failed to read data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
			goto bailout;
		}

		if ( rc == 0 )
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
			}

			cfg->cfg_ServerRunning = FALSE;

			if ( cfg->cfg_LogUserDisconnect )
			{
				Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
			}
			goto bailout;
		}

		cfg->SessionStatus.si_Read_Bytes += rc;
	}

	// --

	memset( & cfg->NetSend_Encodings1, 0, ENCODE_LAST * sizeof( uint8 ) );
	memset( & cfg->NetSend_Encodings2, 0, ENCODE_LAST * sizeof( struct myEncNode ) );

	for( cnt=0 ; cnt<ENCODE_LAST ; cnt++ )
	{
		cfg->NetSend_Encodings2[cnt].Type = cnt;
	}

	if ( DoVerbose )
	{
		printf( "Client Supported Encodings\n" );
		printf( "\n" );
	}

	for( cnt=0 ; cnt<em.em_Encodings ; cnt++ )
	{
		if ( DoVerbose )
		{
			printf( " Encoding: %08lx", enc[cnt] );
			/**/ if ( enc[cnt] == 0 )	{ printf( " - Raw" ); }
			else if ( enc[cnt] == 1 )	{ printf( " - CopyRect" ); }
			else if ( enc[cnt] == 2 )	{ printf( " - RRE" ); }
			else if ( enc[cnt] == 4 )	{ printf( " - CoRRE" ); }
			else if ( enc[cnt] == 5 )	{ printf( " - Hextile" ); }
			else if ( enc[cnt] == 6 )	{ printf( " - ZLib" ); }
			else if ( enc[cnt] == 7 )	{ printf( " - Tight" ); }
			else if ( enc[cnt] == 8 )	{ printf( " - Ultra" ); }
			else if ( enc[cnt] == 9 )	{ printf( " - ZLibHex" ); }
			else if ( enc[cnt] == 15 )	{ printf( " - TRLE" ); }
			else if ( enc[cnt] == 16 )	{ printf( " - ZRLE" ); }
			else if ( enc[cnt] == 17 )	{ printf( " - ZYWRLE" ); }
			else if ( enc[cnt] == 0xFFFFFF11 )	{ printf( " - RichCursor" ); }
			else if ( enc[cnt] == 0xFFFFFF18 )	{ printf( " - MousePos" ); }
			else if ( enc[cnt] == 0xFFFFFF21 )	{ printf( " - NewFBSize" ); }
			printf( "\n" );
		}
// #define rfbEncodingExtDesktopSize     0xFFFFFECC

		if ( enc[cnt] == 0 ) // Raw
		{
			cfg->NetSend_Encodings1[ENCODE_Raw] = TRUE;
			cfg->NetSend_Encodings2[ENCODE_Raw].Pos = cnt;
			cfg->NetSend_Encodings2[ENCODE_Raw].Enabled = TRUE;
			cfg->NetSend_Encodings2[ENCODE_Raw].EncType = ( enc[cnt] & 0x80000000 ) ? FALSE : TRUE;
			continue;
		}

		if ( enc[cnt] == 6 ) // ZLib
		{
			cfg->NetSend_Encodings1[ENCODE_ZLib] = TRUE;
			cfg->NetSend_Encodings2[ENCODE_ZLib].Pos = cnt;
			cfg->NetSend_Encodings2[ENCODE_ZLib].Enabled = TRUE;
			cfg->NetSend_Encodings2[ENCODE_ZLib].EncType = ( enc[cnt] & 0x80000000 ) ? FALSE : TRUE;
			continue;
		}

		if ( enc[cnt] == 0xFFFFFF11 ) // Rich Cursor
		{
			cfg->NetSend_Encodings1[ENCODE_RichCursor] = TRUE;
			cfg->NetSend_Encodings2[ENCODE_RichCursor].Pos = cnt;
			cfg->NetSend_Encodings2[ENCODE_RichCursor].Enabled = TRUE;
			cfg->NetSend_Encodings2[ENCODE_RichCursor].EncType = ( enc[cnt] & 0x80000000 ) ? FALSE : TRUE;
			cfg->cfg_UpdateCursor = TRUE;
			continue;
		}
	}

	if ( DoVerbose )
	{
		printf( "\n" );
	}

	// Raw is always valid
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

	#if 0
	for( cnt=0 ; cnt<ENCODE_LAST ; cnt++ )
	{
		printf( "Node %2d : Encoding %d, Enabled %d, EncType: %d\n", cnt, 
			cfg->NetSend_Encodings2[cnt].Type, 
			cfg->NetSend_Encodings2[cnt].Enabled,
			cfg->NetSend_Encodings2[cnt].EncType
		);
	}
	#endif

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

	// --

	msg = myCalloc( sizeof( struct CommandEncoding ) + em.em_Encodings * sizeof( uint32 ) );

	if ( msg )
	{
		msg->ce_Command = CMD_Encoding;
		msg->ce_Encodings = em.em_Encodings;

		memcpy( & msg->ce_Data[0], enc, em.em_Encodings * sizeof( uint32 ) );

		IExec->PutMsg( CmdMsgPort, & msg->ce_Message );
	}


	error = FALSE;

bailout:

	if ( enc )
	{
		myFree( enc );
	}

// Log_PrintF( "myRead_Mouse (%d)\n", error );

	return( error );
}

// --
