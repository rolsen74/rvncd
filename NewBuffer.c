
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

#if 0

struct GfxRectHeader
{
	uint8	bm_Type;
	uint8	bm_Pad;
	uint16	bm_Rects;
	// Rects
};

#endif

// --

static int myNewFill( struct Config *cfg, struct UpdateNode *un, int tile, int hardcursor )
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
				datalen = myEnc_Raw( cfg, un, tile, hardcursor );
				break;
			}

			case ENCODE_ZLib:
			{
				if ( cfg->cfg_Active_Settings.ZLib )
				{
					datalen = myEnc_ZLib( cfg, un, tile, hardcursor );
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

static int myNewTile( struct Config *cfg, struct UpdateNode *un, int tiles, int hardcursor )
{
struct TileInfo *ti;
int stat;
int pos;
int len;
int rc;
int x1;
int y1;
int x2;
int y2;

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

		len = myNewFill( cfg, un, pos, hardcursor );

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

		rc = myNetSend( cfg, cfg->NetSend_SendBuffer, len );

		if ( rc <= 0 )
		{
			goto bailout;
		}

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
struct GfxRectHeader *header;
struct UpdateNode *un;
int doCursor;
int hardcursor;
int total;
int tiles;
int stat;
int rc;

	// --

	IExec->ObtainSemaphore( & cfg->Server_UpdateSema );

	un = (APTR) IExec->GetHead( & cfg->Server_UpdateList );

	// Sending RichCursor Updates, without a BufferUpdateReq
	// could fail if Client changed Screen format
	// happens with RealVNC client

	if ( un == NULL )
	{
		stat = UPDATESTAT_NoChange;		// Do a Delay(2)
		goto bailout;
	}

	// Make a copy, so it don't change during Render
	cfg->cfg_RenderMouseX = cfg->cfg_CurMouseX;
	cfg->cfg_RenderMouseY = cfg->cfg_CurMouseY;


	doCursor = 0;
	total = 0;
	tiles = 0;

	// --

	tiles = myCountTiles( cfg, un );
	total += tiles;

	// are we Using Hardware Cursor ..
	hardcursor = (( cfg->cfg_Active_Settings.RichCursor ) && ( cfg->cfg_ServerSupportsCursor ) && ( cfg->NetSend_Encodings1[ ENCODE_RichCursor ] ));

	if ( hardcursor )
	{
		if ( cfg->cfg_UpdateCursor )
		{
			doCursor = TRUE;
			total++;
		}
	}

	if ( total == 0 )
	{
		stat = UPDATESTAT_NoChange;		// Do a Delay(2)
		goto bailout;
	}

	// --

	if ( cfg->cfg_ServerCursorStat != hardcursor )
	{
		cfg->cfg_ServerCursorStat = hardcursor;

		if ( hardcursor )
		{
			Log_PrintF( cfg, LOGTYPE_Info, "Using Rich cursor" );
		}
		else
		{
			Log_PrintF( cfg, LOGTYPE_Info, "Using Soft cursor" );
		}
	}

	// -- Send Header

	header = cfg->NetSend_SendBuffer;

	header->bm_Type	= 0; // Framebuffer Update
	header->bm_Pad	= 0;
	header->bm_Rects = total;

	rc = myNetSend( cfg, header, sizeof( header ));

	if ( rc <= 0 )
	{
		stat = UPDATESTAT_Error;
		goto bailout;
	}

	// -- Update Graphics Tiles

	if ( tiles )
	{
		stat = myNewTile( cfg, un, tiles, hardcursor );

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
//		IExec->ObtainSemaphore( & cfg->Server_UpdateSema );

		IExec->Remove( & un->un_Node );

		IExec->AddTail( & cfg->Server_UpdateFree, & un->un_Node );

//		IExec->ReleaseSemaphore( & cfg->Server_UpdateSema );
	}

	// -- All Done

	stat = UPDATESTAT_Okay;			// No wait
//	stat = UPDATESTAT_NoChange;		// Do a Delay(2)

bailout:

	IExec->ReleaseSemaphore( & cfg->Server_UpdateSema );

	return( stat );
}
