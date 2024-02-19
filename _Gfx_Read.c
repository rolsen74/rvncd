 
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

static void mySetScreen( struct Config *cfg, struct Screen *scr );

// --

static void myCheckTile( struct Config *cfg, int pos )
{
struct TileInfo *ti;
struct Screen *scr;
struct Screen *cur;
uint8 *buf1;
uint8 *buf2;
int stat;
int size;
int tile;
int lock;
int ts;

	// --

	if (( cfg->GfxRead_Screen_Adr == NULL )
	||	( cfg->GfxRead_Screen_Tiles <= pos )
	||	( cfg->GfxRead_Encode_Format == 0 )
	||	( cfg->GfxRead_Encode_FormatSize == 0 ))
	{
		goto bailout;
	}

	// --

	ts		= cfg->GfxRead_Screen_TileSize;
	ti		= cfg->GfxRead_Screen_TileInfoBuffer;
	tile	= cfg->GfxRead_Screen_TileRandomBuffer[pos];
	buf1	= cfg->GfxRead_Screen_TileCheckBuffer;
	size	= cfg->GfxRead_Encode_FormatSize * ts * ts;
	buf2	= & cfg->GfxRead_Screen_ChunkyBuffer[ size * tile ];

	if ( ti[tile].C )
	{
		// We need to clear the tile first
		memset( buf1, 0, size );
	}

	// -- Atleast try too see if the Screen still is there

	scr = cfg->GfxRead_Screen_Adr;

	lock = IIntuition->LockIBase( 0 );

	cur = IntuitionBase->FirstScreen;

	while( cur )
	{
		if ( scr == cur )
		{
			break;
		}
		else
		{
			cur = cur->NextScreen;
		}
	}

	IIntuition->UnlockIBase( lock );

	if ( cur == NULL )
	{
		mySetScreen( cfg, NULL );
		goto bailout;
	}

	#if 0
IExec->DebugPrintF( "X: %4d, Y: %4d, W: %3d, H: %3d, %d, Pos: %d\n", 
	ti[tile].X,
	ti[tile].Y,
	ti[tile].W,
	ti[tile].H,
	cfg->Enc_FormatSize,
	pos );
	#endif

	IGraphics->ReadPixelArray( 
		& scr->RastPort,
		ti[tile].X,		// srcx
		ti[tile].Y,		// srcy
		buf1,			// Chunky
		0,				// dstx
		0,				// dsty
		cfg->GfxRead_Encode_FormatSize * ts, // Bytes per row
		cfg->GfxRead_Encode_Format,	// Format
		ti[tile].W,		// Width
		ti[tile].H		// Height
	);

	// --

	IExec->ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

	stat = memcmp( buf2, buf1, size );

	if ( stat )
	{
		memcpy( buf2, buf1, size ) ;

// IExec->DebugPrintF( "Update pos %d\n", pos );

		cfg->GfxRead_Screen_TileArrayBuffer[tile] = 0x80;
	}
	else
	{
		if ( 0 <= cfg->GfxRead_Screen_TileArrayBuffer[tile] )
		{
			cfg->GfxRead_Screen_TileArrayBuffer[tile]++;
		}
	}

	IExec->ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );

	// --

bailout:

	return;
}

// --

static int myTileCheck( struct Config *cfg )
{
void *buf;
int size;
int err;

	err = TRUE;

	if ( cfg->GfxRead_Screen_TileCheckBuffer )
	{
		IExec->UnlockMem( cfg->GfxRead_Screen_TileCheckBuffer, cfg->GfxRead_Screen_TileCheckSize );
		IExec->FreeVec( cfg->GfxRead_Screen_TileCheckBuffer );
		cfg->GfxRead_Screen_TileCheckBuffer = NULL;
	}

	// 1x Tile chunky .. max 256x256x4
	size = cfg->GfxRead_Screen_TileSize * cfg->GfxRead_Screen_TileSize * 4;

	buf = IExec->AllocVecTags( size,
		AVT_PhysicalAlignment, 4096,
		AVT_Contiguous, TRUE,
		AVT_Alignment, 4096,
		AVT_Lock, TRUE,
		TAG_END
	);

	if ( buf == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "1: Error allocating memory. (%ld Bytes)", size );
		goto bailout;
	}

	cfg->GfxRead_Screen_TileCheckSize = size;
	cfg->GfxRead_Screen_TileCheckBuffer = buf;

	err = FALSE;

bailout:

	return( err );
}

// --

// Read Hold Screen
static void myReadHoleScreen( struct Config *cfg )
{
int cnt;

	for( cnt=0 ; cnt<cfg->GfxRead_Screen_Tiles ; cnt++ )
	{
		myCheckTile( cfg, cnt );
	}
}

// --

static int myScreenChunky( struct Config *cfg )
{
void *buf;
int size;
int err;

	err = TRUE;

	if ( cfg->GfxRead_Screen_ChunkyBuffer )
	{
		IExec->UnlockMem( cfg->GfxRead_Screen_ChunkyBuffer, cfg->GfxRead_Screen_ChunkySize );
		IExec->FreeVec( cfg->GfxRead_Screen_ChunkyBuffer );
		cfg->GfxRead_Screen_ChunkyBuffer = NULL;
	}

	// We may allocate more memory than the screen use
	// to fit the tiles.. and those border tiles need zero memory

	size = (( cfg->GfxRead_Screen_TileWidth  * cfg->GfxRead_Screen_TileSize ) * 
			( cfg->GfxRead_Screen_TileHeight * cfg->GfxRead_Screen_TileSize ) * ( 4 ));

	buf = IExec->AllocVecTags( size,
		AVT_PhysicalAlignment, 4096,
		AVT_Contiguous, TRUE,
		AVT_Alignment, 4096,
		AVT_Clear, 0,
		AVT_Lock, TRUE,
		TAG_END
	);

	if ( buf == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "2: Error allocating memory. (%ld Bytes)", size );
		goto bailout;
	}

	cfg->GfxRead_Screen_ChunkySize = size;
	cfg->GfxRead_Screen_ChunkyBuffer = buf;

	err = FALSE;

bailout:

	return( err );
}

// --

static int myTileArray( struct Config *cfg )
{
void *buf;
int size;
int err;

	err = TRUE;

	if ( cfg->GfxRead_Screen_TileArrayBuffer )
	{
		myFree( cfg->GfxRead_Screen_TileArrayBuffer );
		cfg->GfxRead_Screen_TileArrayBuffer = NULL;
	}

	size = cfg->GfxRead_Screen_Tiles;

	buf = myMalloc( size * sizeof( int8 ));

	if ( buf == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "3: Error allocating memory. (%ld Bytes)", size );
		goto bailout;
	}

	cfg->GfxRead_Screen_TileArrayBuffer = buf;

	err = FALSE;

bailout:

	return( err );
}

// --

static int myTilesInfo( struct Config *cfg )
{
struct TileInfo *ti;
int error;
int size;
int pos;
int ts;
int xx;
int yy;
int x;
int y;
int w;
int h;
int c;

	error = TRUE;

	if ( cfg->GfxRead_Screen_TileInfoBuffer )
	{
		myFree( cfg->GfxRead_Screen_TileInfoBuffer );
		cfg->GfxRead_Screen_TileInfoBuffer = NULL;
	}

	size = cfg->GfxRead_Screen_Tiles;

	ti = myMalloc( size * sizeof( struct TileInfo ));

	if ( ti == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "4: Error allocating memory. (%ld Bytes)", size * sizeof( struct TileInfo ));
		goto bailout;
	}

	cfg->GfxRead_Screen_TileInfoBuffer = ti;

	// --

	pos = 0;

	ts = cfg->GfxRead_Screen_TileSize;

	for( yy=0 ; yy < cfg->GfxRead_Screen_TileHeight ; yy++ )
	{
		for( xx=0 ; xx < cfg->GfxRead_Screen_TileWidth ; xx++, pos++ )
		{
			x = ts * xx;
			y = ts * yy;
			w = ts;
			h = ts;
			c = FALSE;

			if ( x + ts > cfg->GfxRead_Screen_Width )
			{
				w = ts - ( x + ts - cfg->GfxRead_Screen_Width );
				c = TRUE;
			}

			if ( y + ts > cfg->GfxRead_Screen_Height )
			{
				h = ts - ( y + ts - cfg->GfxRead_Screen_Height );
				c = TRUE;
			}

			ti[pos].X = x;
			ti[pos].Y = y;
			ti[pos].W = w;
			ti[pos].H = h;
			ti[pos].C = c;
		}
	}

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

static int myRandomizeTiles( struct Config *cfg )
{
int *buf;
int *rnd;
int offset;
int error;
int size;
int pos;
int val;
int i;

	error = TRUE;

	size = cfg->GfxRead_Screen_TileWidth * cfg->GfxRead_Screen_TileHeight;

	if ( cfg->GfxRead_Screen_TileRandomBuffer )
	{
		myFree( cfg->GfxRead_Screen_TileRandomBuffer );
		cfg->GfxRead_Screen_TileRandomBuffer = NULL;
	}

	buf = myMalloc( size * sizeof( int ));

	if ( buf == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "5: Error allocating memory. (%ld Bytes)", size * sizeof( int ));
		goto bailout;
	}

	cfg->GfxRead_Screen_TileRandomBuffer = buf;

	// --

	/**/ if ( cfg->GfxRead_BufferScanMethod == SCANMethod_Linear )
	{
		for( i=0 ; i<size ; i++ )
		{
			buf[i] = i;
		}
	}
	else // ( cfg->GfxRead_BufferScanMethod == SCANMethod_Random )
	{
		rnd = myMalloc( size * sizeof( int ));

		if ( rnd == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "6: Error allocating memory. (%ld Bytes)", size * sizeof( int ));
			goto bailout;
		}

		for( i=0 ; i<size ; i++ )
		{
			rnd[i] = i;
		}

		pos = 0;

		for( i=size ; i > 0 ; i-- )
		{
			offset = rand() % i;

			val = rnd[offset];

			buf[pos++] = val;

			memmove( & rnd[offset], &rnd[offset+1], ( i - offset - 1 ) * sizeof( int ) );
		}

		myFree( rnd );
	}

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

static void mySetScreen( struct Config *cfg, struct Screen *scr )
{
int err;

	IExec->ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

	err = TRUE;

	if ( scr == NULL )
	{

//IExec->DebugPrintF( "Set Screen NULL\n" );
//printf( "Set Screen NULL\n" );

		cfg->GfxRead_Screen_Adr			= NULL;
		cfg->GfxRead_Screen_Width		= 0;
		cfg->GfxRead_Screen_Height		= 0;
		cfg->GfxRead_Screen_Format		= 0;
		cfg->GfxRead_Screen_TileWidth	= 0;
		cfg->GfxRead_Screen_TileHeight	= 0;
		cfg->GfxRead_Screen_Tiles		= 0;

		if ( cfg->GfxRead_Screen_TileCheckBuffer )
		{
			IExec->UnlockMem( cfg->GfxRead_Screen_TileCheckBuffer, cfg->GfxRead_Screen_TileCheckSize );
			IExec->FreeVec( cfg->GfxRead_Screen_TileCheckBuffer );
			cfg->GfxRead_Screen_TileCheckBuffer = NULL;
		}

		if ( cfg->GfxRead_Screen_ChunkyBuffer )
		{
			IExec->UnlockMem( cfg->GfxRead_Screen_ChunkyBuffer, cfg->GfxRead_Screen_ChunkySize );
			IExec->FreeVec( cfg->GfxRead_Screen_ChunkyBuffer );
			cfg->GfxRead_Screen_ChunkyBuffer = NULL;
		}

		if ( cfg->GfxRead_Screen_TileArrayBuffer )
		{
			myFree( cfg->GfxRead_Screen_TileArrayBuffer );
			cfg->GfxRead_Screen_TileArrayBuffer = NULL;
		}

		if ( cfg->GfxRead_Screen_TileInfoBuffer )
		{
			myFree( cfg->GfxRead_Screen_TileInfoBuffer );
			cfg->GfxRead_Screen_TileInfoBuffer = NULL;
		}

		if ( cfg->GfxRead_Screen_TileRandomBuffer )
		{
			myFree( cfg->GfxRead_Screen_TileRandomBuffer );
			cfg->GfxRead_Screen_TileRandomBuffer = NULL;
		}

		if ( cfg->GfxRead_PubScreenAdr )
		{
			IIntuition->UnlockPubScreen( NULL, cfg->GfxRead_PubScreenAdr );
			cfg->GfxRead_PubScreenAdr = NULL;
		}

		mySetEncoding_Format( cfg, 0 );

		err = FALSE;
	}
	else
	{
//IExec->DebugPrintF( "Set Screen %p\n", scr );
//printf( "Set Screen %p\n", scr );

		cfg->GfxRead_Screen_Adr			= scr;
		cfg->GfxRead_Screen_Width		= scr->Width;
		cfg->GfxRead_Screen_Height		= scr->Height;
		cfg->GfxRead_Screen_Format		= IGraphics->GetBitMapAttr( scr->RastPort.BitMap, BMA_PIXELFORMAT );
		cfg->GfxRead_Screen_TileWidth	= ( scr->Width  + cfg->GfxRead_Screen_TileSize - 1 ) / cfg->GfxRead_Screen_TileSize;
		cfg->GfxRead_Screen_TileHeight	= ( scr->Height + cfg->GfxRead_Screen_TileSize - 1 ) / cfg->GfxRead_Screen_TileSize;
		cfg->GfxRead_Screen_Tiles		= cfg->GfxRead_Screen_TileWidth * cfg->GfxRead_Screen_TileHeight;

		if ( myRandomizeTiles( cfg ))
		{
			goto bailout;
		}

		if ( myTilesInfo( cfg ))
		{
			goto bailout;
		}

		if ( myTileArray( cfg ))
		{
			goto bailout;
		}

		if ( myScreenChunky( cfg ))
		{
			goto bailout;
		}

		if ( myTileCheck( cfg ))
		{
			goto bailout;
		}

		mySetEncoding_Format( cfg, cfg->GfxRead_Screen_Format );

		// Must be after SetEncoding
		myReadHoleScreen( cfg );

		err = FALSE;
	}

bailout:

	if (( err ) && ( scr ))
	{
		mySetScreen( cfg, NULL );
	}

	IExec->ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );

//IExec->DebugPrintF( "22 ScrAdr: %p\n", cfg->GfxRead_Screen_Adr );
//printf( "22 ScrAdr: %p\n", cfg->GfxRead_Screen_Adr );

}

// --

static void myDetectScrChange( struct Config *cfg )
{
struct Screen *scr;
int lock;
int w;
int h;

	// --

// cfg->GfxRead_Screen_SupportChange = 1;

	if ( ! cfg->GfxRead_Screen_SupportChange )
	{
		if ( cfg->GfxRead_PubScreenAdr == NULL )
		{
			cfg->GfxRead_PubScreenAdr = IIntuition->LockPubScreen( NULL );
		}

		scr = cfg->GfxRead_PubScreenAdr;
	}
	else
	{
		if ( cfg->GfxRead_PubScreenAdr )
		{
			IIntuition->UnlockPubScreen( NULL, cfg->GfxRead_PubScreenAdr );
			cfg->GfxRead_PubScreenAdr = NULL;
		}

		// --

		lock = IIntuition->LockIBase( 0 );

		scr = IntuitionBase->FirstScreen;

		IIntuition->UnlockIBase( lock );
	}

	// --

	w = ( scr ) ? scr->Width  : 0;
	h = ( scr ) ? scr->Height : 0;

	if (( cfg->GfxRead_Screen_Adr == scr )
	&&	( cfg->GfxRead_Screen_Width	== w )
	&&	( cfg->GfxRead_Screen_Height == h ))
	{
		goto bailout;
	}

	// --

//	IExec->DebugPrintF( "Dectected Screen Change\n" );
//	printf( "Dectected Screen Change\n" );

	// --

	mySetScreen( cfg, scr );

	myGUI_PxlFmtRefresh( cfg );

	// --

bailout:

	return;
}

// --

static void myProcess_Main( struct Config *cfg )
{
uint32 mask;
int delay;
int pos;

	delay = 0;
	pos = 0;

	cfg->GfxRead_Screen_SupportChange = TRUE;

	while( TRUE )
	{
		mask = IDOS->CheckSignal( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E );

		if (( mask & ( SIGBREAKF_CTRL_D | SIGBREAKF_CTRL_E )) && ( cfg->GfxRead_Exit ))
		{
			break;
		}

		if ( cfg->UserCount ) 
		{
			myDetectScrChange( cfg );

			// Clear after ScrChange
			cfg->cfg_UserConnectSignal = FALSE;

			if ( cfg->GfxRead_Screen_Adr )
			{
				if ( --pos < 0 )
				{
					pos = cfg->GfxRead_Screen_Tiles - 1;
				}

				myCheckTile( cfg, pos );

				if ( cfg->GfxRead_DelayFrequency )
				{
					delay++;

					if ( cfg->GfxRead_DelayFrequency <= delay )
					{
						IDOS->Delay( 2 );
						delay = 0;
					}
				}
			}
			else
			{
				IDOS->Delay( 25 );
			}
		}
		else
		{
			if ( cfg->GfxRead_Screen_Adr )
			{
				mySetScreen( cfg, NULL );
			}

			cfg->cfg_UserDisconnectSignal = FALSE;
			IDOS->Delay( 2 );
			pos = 0;
		}
	}

//	Log_PrintF( cfg, LOGTYPE_Info, "Stopping" );
}

// --

static int myProcess_Init( struct Config *cfg UNUSED )
{
int retval;

	retval = FALSE;


	retval = TRUE;

//bailout:

	return( retval );
}

// --

static void myProcess_Free( struct Config *cfg )
{
	// --

//	Log_PrintF( cfg, LOGTYPE_Info, "Shutting down .. " );

	// --

	// Free Screen Resources
	mySetScreen( cfg, NULL );
}

// --

static void myServerProcess( void )
{
struct StartMessage *sm;
struct Config *Config;
struct Task *Parent;
struct Task *Self;
int stat;

	//--------

	Self = IExec->FindTask( NULL );

	sm = Self->tc_UserData;

	Parent = sm->Parent;
	Config = sm->Config;

	//--------

	Config->GfxRead_Exit = NULL;
	Config->cfg_GfxReadStatus = PROCESS_Starting;

	stat = myProcess_Init( Config );

	if ( stat )
	{
		Config->GfxRead_Task = Self;

		Config->cfg_GfxReadStatus = PROCESS_Running;

		// Set signal after Status
		IExec->Signal( Parent, SIGBREAKF_CTRL_E );

		// --

		myProcess_Main( Config );

		// --

		Config->cfg_GfxReadStatus = PROCESS_Stopping;

		Parent = Config->GfxRead_Exit;

		Config->GfxRead_Task = NULL;
	}

	myProcess_Free( Config );

	Config->cfg_GfxReadStatus = PROCESS_Stopped;

	//--------

	if ( Parent )
	{
		IExec->Forbid();
		IExec->Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

int myStart_Gfx_Read( struct Config *cfg )
{
struct StartMessage msg;
uint32 mask;
int error;

	error = TRUE;

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_GfxReadStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_GfxReadStatus == PROCESS_Stopping ))
	{
		IDOS->Delay( 2 );
	}

	if ( cfg->cfg_GfxReadStatus == PROCESS_Stopped )
	{
		msg.Parent = IExec->FindTask( NULL );
		msg.Config = cfg;

		if ( IDOS->CreateNewProcTags(
			NP_Name,		"Gfx Read Process",
			NP_Priority,	-5,
			NP_Entry,		myServerProcess,
			NP_Child,		TRUE,
			NP_UserData,	& msg,
			TAG_END ))
		{
			while( TRUE )
			{
				mask = IExec->Wait( SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_F );

				// CTRL+E is meaning every thing is okey
				if ( mask & SIGBREAKF_CTRL_E )
				{
					break;
				}

				// CTRL+F is means failure
				if ( mask & SIGBREAKF_CTRL_F )
				{
					break;
				}
			}
		}
	}

	if ( cfg->cfg_GfxReadStatus == PROCESS_Running )
	{
		error = FALSE;
	}

	return( error );
}

// --

void myStop_Gfx_Read( struct Config *cfg )
{
uint32 mask;
int cnt;

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	if ( cfg->cfg_GfxReadStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_GfxReadStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_GfxReadStatus == PROCESS_Stopping ))
		{
			IDOS->Delay( 2 );
		}

		if ( cfg->cfg_GfxReadStatus == PROCESS_Running )
		{
			cfg->GfxRead_Exit = IExec->FindTask( NULL );

			IExec->Signal( cfg->GfxRead_Task, SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E );

			cnt = 0;

			while( TRUE )
			{
				mask = IDOS->CheckSignal( SIGBREAKF_CTRL_F );

				if ( mask & SIGBREAKF_CTRL_F )
				{
					break;
				}
				else
				{
					IDOS->Delay( 5 );

					if ( ! ( ++cnt % 50 ))
					{
						printf( "\rmyStop_Gfx_Read still waiting : %d", cnt );
					}
				}
			}

			if ( cnt >= 50 )
			{
				printf( "\n" );
			}
		}
	}
}

// --
