 
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

static int myNewFill( struct Config *cfg, struct UpdateNode *un, int tile )
{
int datalen;
int cnt;
int t;

	datalen = -1;

	for( cnt=0 ; cnt<ENCODE_LAST ; cnt++ )
	{
		if ( ! cfg->NetSend_Encodings2[ cnt ].Enabled )
		{
			continue;
		}

		t = cfg->NetSend_Encodings2[ cnt ].Type;

		switch( t )
		{
			case ENCODE_Raw:
			{
				datalen = myEnc_Raw( cfg, un, tile );
				break;
			}

			case ENCODE_ZLib:
			{
				if ( cfg->cfg_Active_Settings.ZLib )
				{
					datalen = myEnc_ZLib( cfg, un, tile );
				}
				break;
			}

			default:
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = myASPrintF( "Internal: Unsupported Tile Encoding (%d)", t );
				}

				printf( "Unknown Encoding (%d)\n", t );
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

static int myNewTile( struct Config *cfg, struct UpdateNode *un, int tiles )
{
struct SocketIFace *ISocket;
struct TileInfo *ti;
int stat;
int pos;
int len;
int rc;
int x1;
int y1;
int x2;
int y2;

	ISocket = cfg->NetSend_ISocket;

	stat = UPDATESTAT_Error;

	pos = ( un->un_Incremental ) ? cfg->NetSend_ScreenCurrentTile : 0 ;

	x1 = un->un_XPos;
	y1 = un->un_YPos;
	x2 = un->un_XPos + un->un_Width;
	y2 = un->un_YPos + un->un_Height;

	while( tiles )
	{
		if ( --pos < 0 )
		{
			pos = cfg->GfxRead_Screen_Tiles - 1;
		}

		ti = & cfg->GfxRead_Screen_TileInfoBuffer[pos];

		if ( ti->X > x2 )
		{
			continue;
		}

		if ( ti->X + ti->W < x1 )
		{
			continue;
		}

		if ( ti->Y > y2 )
		{
			continue;
		}

		if ( ti->Y + ti->H < y1 )
		{
			continue;
		}

		// --

		if (( un->un_Incremental ) && ( cfg->GfxRead_Screen_TileArrayBuffer[pos] >= 0 ))
		{
			// Count up untell it flips over to Negative
			cfg->GfxRead_Screen_TileArrayBuffer[pos]++;
			continue;
		}

		cfg->GfxRead_Screen_TileArrayBuffer[pos] = rand() % 20;

		// --

		len = myNewFill( cfg, un, pos );

		if ( len < 0 )
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Internal: myNewFill failed" );
			}

			Log_PrintF( cfg, LOGTYPE_Error, "Buffer error" );
			goto bailout;
		}

		if ( len > cfg->NetSend_SendBufferSize )
		{
			if ( ! cfg->cfg_NetReason )
			{
				cfg->cfg_NetReason = myASPrintF( "Internal: myNewFill failed" );
			}

			rc = -1;
			Log_PrintF( cfg, LOGTYPE_Error, "Buffer overflow" );
			goto bailout;
		}

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

		cfg->SessionStatus.si_Send_Bytes += rc;

		tiles--;
	}

	cfg->NetSend_ScreenCurrentTile = pos;

	// --

	stat = UPDATESTAT_Okay;

bailout:

	return( stat );
}

// --

static int myCountTiles( struct Config *cfg, struct UpdateNode *un )
{
struct TileInfo *ti;
int tiles;
int pos;
int x1;
int y1;
int x2;
int y2;

	tiles = 0;

	x1 = un->un_XPos;
	y1 = un->un_YPos;
	x2 = un->un_XPos + un->un_Width;
	y2 = un->un_YPos + un->un_Height;

#if 0
if (( cfg->GfxRead_Screen_PageWidth  != un->un_Width  )
||  ( cfg->GfxRead_Screen_PageHeight != un->un_Height ))
{
	printf( "Request Update : %dx%d %dx%d\n", un->un_XPos, un->un_YPos, un->un_Width, un->un_Height );
}
#endif

	for( pos=0 ; pos < cfg->GfxRead_Screen_Tiles ; pos++ )
	{
		ti = & cfg->GfxRead_Screen_TileInfoBuffer[pos];

		if ( ti->X > x2 )
		{
			continue;
		}

		if ( ti->X + ti->W < x1 )
		{
			continue;
		}

		if ( ti->Y > y2 )
		{
			continue;
		}

		if ( ti->Y + ti->H < y1 )
		{
			continue;
		}

		if (( un->un_Incremental ) && ( cfg->GfxRead_Screen_TileArrayBuffer[pos] >= 0 ))
		{
			continue;
		}

		tiles++;
	}

	return( tiles );
}

// --

int NewBufferUpdate( struct Config *cfg )
{
struct BufferMessage *header;
struct SocketIFace *ISocket;
struct UpdateNode *un;
int doCursor;
int total;
int tiles;
int stat;
int rc;

	// --

	IExec->ObtainSemaphore( & cfg->Server_UpdateSema );

	un = (APTR) IExec->GetHead( & cfg->Server_UpdateList );

	IExec->ReleaseSemaphore( & cfg->Server_UpdateSema );

	// Sending RichCursor Updates, without a BufferUpdateReq
	// could fail if Client changed Screen format
	if ( un == NULL )
	{
		stat = UPDATESTAT_NoChange;		// Do a Delay(2)
		goto bailout;
	}

	doCursor = 0;
	total = 0;
	tiles = 0;

	// --

	tiles = myCountTiles( cfg, un );
	total += tiles;

	if (( cfg->cfg_Active_Settings.RichCursor )
	&&	( cfg->cfg_ServerSupportsCursor )
	&&	( cfg->NetSend_Encodings1[ ENCODE_RichCursor ] )
	&&	( cfg->cfg_UpdateCursor ))
	{
		doCursor = TRUE;
		total++;
	}

	if ( total == 0 )
	{
		stat = UPDATESTAT_NoChange;		// Do a Delay(2)
		goto bailout;
	}

	// --

	// -- Send Header

	header = cfg->NetSend_SendBuffer;

	header->bm_Type	= 0; // Framebuffer Update
	header->bm_Pad	= 0;
	header->bm_Rects = total;

	ISocket = cfg->NetSend_ISocket;

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

	// -- Update Graphics Tiles

	if ( tiles )
	{
		stat = myNewTile( cfg, un, tiles );

		if ( stat != UPDATESTAT_Okay )
		{
			goto bailout;
		}
	}

	// -- Update Cursor Tile

	if ( doCursor )
	{
		cfg->cfg_UpdateCursor = FALSE;

		stat = NewBuffer_Cursor( cfg );

		if ( stat != UPDATESTAT_Okay )
		{
			goto bailout;
		}
	}

	// -- Move UpdateNode to Free List

	if ( un )
	{
		IExec->ObtainSemaphore( & cfg->Server_UpdateSema );

		IExec->Remove( & un->un_Node );

		IExec->AddTail( & cfg->Server_UpdateFree, & un->un_Node );

		IExec->ReleaseSemaphore( & cfg->Server_UpdateSema );
	}

	// -- All Done

	stat = UPDATESTAT_Okay;			// No wait
//	stat = UPDATESTAT_NoChange;		// Do a Delay(2)

bailout:

	return( stat );
}
