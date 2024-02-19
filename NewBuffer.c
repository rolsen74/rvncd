 
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

static int			myNewFill(			struct Config *cfg, int tile );
static inline int	myNewFull(			struct Config *cfg );

// --

#pragma pack(1)

struct BufferMessage
{
	uint8	bm_Type;
	uint8	bm_Pad;
	uint16	bm_Rects;
	// Rects
};

#pragma pack(0)

// --

static int myNewFill( struct Config *cfg, int tile )
{
int datalen;
int cnt;

	datalen = -1;

	for( cnt=0 ; cnt<ENCODE_LAST ; cnt++ )
	{
		if ( ! cfg->NetSend_Encodings2[ cnt ].Enabled )
		{
			continue;
		}

		switch( cfg->NetSend_Encodings2[ cnt ].Type )
		{
			case ENCODE_Raw:
			{
				datalen = myEnc_Raw( cfg, tile );
				break;
			}

			case ENCODE_ZLib:
			{
				if ( cfg->cfg_Active_Settings.ZLib )
				{
					datalen = myEnc_ZLib( cfg, tile );
				}
				break;
			}

			default:
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = myASPrintF( "Internal: Unsupported Tile Encoding (%d)", cfg->NetSend_Encodings2[ cnt ].Type );
				}

				printf( "Unknown Encoding (%d)\n", cfg->NetSend_Encodings2[ cnt ].Type );
				break;
			}
		}

		if ( datalen != -1 )
		{
			// Done
			break;
		}
	}

	return( datalen );
}

// --

static inline int myNewFull( struct Config *cfg )
{
struct SocketIFace *ISocket;
int stat;
int cnt;
int len;
int rc;

// IExec->DebugPrintF( "myNewFull\n" );

	stat = UPDATESTAT_Error;

	ISocket = cfg->NetSend_ISocket;

	// --

	for( cnt=0 ; cnt<cfg->GfxRead_Screen_Tiles ; cnt++ )
	{
		len = myNewFill( cfg, cnt );

// printf( "Send Tile Len: %d\n", len );

		if ( len < 0 )
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Internal: myNewFill failed" );
			}

			Log_PrintF( cfg, LOGTYPE_Error, "Buffer error" );
			goto bailout;
		}

		if ( len <= cfg->NetSend_SendBufferSize )
		{
			rc = ISocket->send( cfg->NetSend_ClientSocket, cfg->NetSend_SendBuffer, len, 0 );

			if ( rc == -1 )
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = myASPrintF( "Failed to send data (%d)", ISocket->Errno() );
				}

				Log_PrintF( cfg, LOGTYPE_Error, "Failed to send data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
				goto bailout;
			}

			if ( rc == 0 )
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
				}

				if ( cfg->cfg_LogUserDisconnect )
				{
					Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
				}
				goto bailout;
			}
		}
		else
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Internal: myNewFill failed" );
			}

			rc = -1;
			Log_PrintF( cfg, LOGTYPE_Error, "Buffer overflow" );
			goto bailout;
		}

//		IExec->ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );

//		rc = ISocket->send( cfg->NetSend_ClientSocket, cfg->NetSend_SendBuffer, len, 0 );

//		IExec->ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

		cfg->SessionStatus.si_Send_Bytes += rc;
		cfg->GfxRead_Screen_TileArrayBuffer[cnt] = rand() % 20;
	}

	// --

	stat = UPDATESTAT_Okay;

bailout:

// IExec->DebugPrintF( "myNewFull 2\n" );

	return( stat );
}

// --

static inline int myNewTile( struct Config *cfg, int tiles )
{
struct SocketIFace *ISocket;
int number;
int tile;
int stat;
int pos;
int len;
int rc;

// IExec->DebugPrintF( "myNewTile\n" );

	ISocket = cfg->NetSend_ISocket;

	stat = UPDATESTAT_Error;

	// --

	pos = cfg->NetSend_ScreenCurrentTile;

	number = 0;

	while( number < tiles )
	{
		if ( --pos < 0 )
		{
			pos = cfg->GfxRead_Screen_Tiles - 1;
		}

		tile = cfg->GfxRead_Screen_TileRandomBuffer[pos];

		if ( cfg->GfxRead_Screen_TileArrayBuffer[tile] < 0 )
		{
			len = myNewFill( cfg, tile );

			if ( len < 0 )
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = myASPrintF( "Internal: myNewFill failed" );
				}

				Log_PrintF( cfg, LOGTYPE_Error, "Buffer error" );
				goto bailout;
			}

			if ( len <= cfg->NetSend_SendBufferSize )
			{
				rc = ISocket->send( cfg->NetSend_ClientSocket, cfg->NetSend_SendBuffer, len, 0 );

				if ( rc == -1 )
				{
					if ( ! cfg->cfg_NetReason )
					{
						cfg->cfg_NetReason = myASPrintF( "Failed to send data (%d)", ISocket->Errno() );
					}

					Log_PrintF( cfg, LOGTYPE_Error, "Failed to send data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
					goto bailout;
				}

				if ( rc == 0 )
				{
					if ( ! cfg->cfg_NetReason )
					{
						cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
					}

					if ( cfg->cfg_LogUserDisconnect )
					{
						Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
					}
					goto bailout;
				}
			}
			else
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = myASPrintF( "Internal: myNewFill failed" );
				}

				rc = -1;
				Log_PrintF( cfg, LOGTYPE_Error, "Buffer overflow" );
				goto bailout;
			}

//			IExec->ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );

//			rc = ISocket->send( cfg->NetSend_ClientSocket, cfg->NetSend_SendBuffer, len, 0 );

//			IExec->ObtainSemaphore( & cfg->GfxRead_Screen_Sema );


			cfg->SessionStatus.si_Send_Bytes += rc;
			cfg->GfxRead_Screen_TileArrayBuffer[tile] = rand() % 20;

			number++;
		}
		else
		{
			// Count up untell it flips over to Negative
			cfg->GfxRead_Screen_TileArrayBuffer[tile]++;
		}
	}

	cfg->NetSend_ScreenCurrentTile = pos;

	// --

	stat = UPDATESTAT_Okay;

bailout:

// IExec->DebugPrintF( "myNewTile 2\n" );

	return( stat );
}

static inline int Count_myNewTile( struct Config *cfg )
{
int tiles;
int cnt;

	tiles = 0;

	for( cnt=0 ; cnt<cfg->GfxRead_Screen_Tiles ; cnt++ )
	{
		if ( cfg->GfxRead_Screen_TileArrayBuffer[cnt] < 0 )
		{
			tiles++;
		}
	}

	return( tiles );
}

// --

int NewBufferUpdate( struct Config *cfg, int Full )
{
struct BufferMessage *header;
struct SocketIFace *ISocket;
int doCursor;
int tiles;
int stat;
int cnt;
int rc;

// IExec->DebugPrintF( "NewBufferUpdate %ld\n", Full );

//	IExec->ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

	if ( cfg->GfxRead_Screen_Adr == NULL )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Screen NULL Pointer" );
		}

printf( "GfxRead_Screen_Adr NULL Pointer\n" );
		// Return okay?
		stat = UPDATESTAT_Error;
		goto bailout;
	}

	doCursor = FALSE;

	// -- Calculate Rects

	cnt = 0;

	if (( cfg->cfg_Active_Settings.RichCursor )
	&&	( cfg->cfg_ServerSupportsCursor )
	&&	( cfg->NetSend_Encodings1[ ENCODE_RichCursor ] )
	&&	( cfg->cfg_UpdateCursor ))
	{
		doCursor = TRUE;
		cnt++;
	}

	if ( Full )
	{
		tiles = cfg->GfxRead_Screen_Tiles;
	}
	else
	{
		tiles = Count_myNewTile( cfg );
	}

	cnt += tiles;

	if ( cnt == 0 )
	{
		stat = UPDATESTAT_NoChange;
		goto bailout;
	}

//	IExec->DebugPrintF( "Send Tiles %4d, Full %d, Cursor: %d, Fmt: %08x\n", tiles, Full, doCursor, cfg->GfxRead_Enocde_ActivePixel.pm_BitsPerPixel  );

	// -- Send Header

	header = cfg->NetSend_SendBuffer;

	header->bm_Type	= 0; // Framebuffer Update
	header->bm_Pad	= 0;
	header->bm_Rects = cnt;

	ISocket = cfg->NetSend_ISocket;

// printf( "Send Header Len: %d\n", sizeof( header ) );

	rc = ISocket->send( cfg->NetSend_ClientSocket, header, sizeof( header ), 0 );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to send data (%d)", ISocket->Errno() );
		}

		stat = UPDATESTAT_Error;
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to send data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		stat = UPDATESTAT_Error;
		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Send_Bytes += rc;

	// -- Send Rects

	if ( doCursor )
	{
		cfg->cfg_UpdateCursor = FALSE;

		NewBuffer_Cursor( cfg );
	}

	if ( Full )
	{
// IExec->DebugPrintF( "NewBufferUpdate Full\n" );
		stat = myNewFull( cfg );
	}
	else if ( tiles )
	{
		stat = myNewTile( cfg, tiles );
	}
	else
	{
		stat = UPDATESTAT_Okay;
	}

//	ssss = 0;

bailout:

//	IExec->ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );

// IExec->DebugPrintF( "NewBufferUpdate 2\n" );

	return( stat );
}

// --
