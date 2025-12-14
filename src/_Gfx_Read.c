
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

static void mySetScreen( struct Config *cfg, struct Screen *scr );

// --
#define DO_OWN_CMP_CPY
#ifdef  DO_OWN_CMP_CPY

static inline U32 myMemCompare( U64 *buf2, U64 *buf1, U32 size )
{
U32 dirty;
U32 loop;
U32 rem;
U64 a;
U64 b;
U32 i;
U8 *s8;
U8 *d8;
U8 sa;
U8 sb;

	// 64bit memcmp + memcpy, with no if brance inside loop
	// Looks like we are hitting the bandwidth wall
	// atleast on a1222

	dirty	= 0;
	loop	= size / 8;
	rem		= size & 7;
	s8		= (U8 *)(buf1 + loop);
	d8		= (U8 *)(buf2 + loop);

	for( i=0 ; i<loop ; i++ )
	{
		a		 = buf1[i];
		b		 = buf2[i];
		dirty	|= a ^ b;
		buf2[i]	 = a;
	}

	// Handle remaning bytes 0-7
	for( i=0 ; i<rem ; i++ )
	{
		sa		 = s8[i];
		sb		 = d8[i];
		dirty	|= sa ^ sb;
		d8[i]	 = sa;
	}

	return( dirty );
}

#endif
// --

static void myCheckTile( struct Config *cfg, U32 pos )
{
struct TileInfo *ti;
struct Screen *scr;
struct Screen *cur;
U8 *buf1;
U8 *buf2;
S32 stat;
S32 size;
U32 tilenr;
S32 lock;
S32 ts;

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
	tilenr	= cfg->GfxRead_Screen_TileRandomBuffer[pos];
	buf1	= cfg->GfxRead_Screen_TileCheckBuffer;
	size	= cfg->GfxRead_Encode_FormatSize * ts * ts;
	buf2	= & cfg->GfxRead_Screen_ChunkyBuffer[ size * tilenr ];

	if ( ti[tilenr].C )
	{
		// We need to clear the tile first
		memset( buf1, 0, size );
	}

	// -- Atleast try too see if the Screen still is there

	scr = cfg->GfxRead_Screen_Adr;

	lock = LockIBase( 0 );

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

	UnlockIBase( lock );

	if ( cur == NULL )
	{
		mySetScreen( cfg, NULL );
		goto bailout;
	}

	S32 sx = ti[tilenr].X;
	S32 sy = ti[tilenr].Y;

	if ( cfg->GfxRead_Screen_ViewMode == VIEWMODE_View )
	{
		if ( cfg->GfxRead_Screen_Adr->ViewPort.DxOffset < 0 )
		{
			sx -= cfg->GfxRead_Screen_Adr->ViewPort.DxOffset;
		}

		if ( cfg->GfxRead_Screen_Adr->ViewPort.DyOffset < 0 )
		{
			sy -= cfg->GfxRead_Screen_Adr->ViewPort.DyOffset;
		}
	}

	// ############################################

	#if defined(HAVE_CGFX)
 
	{
		// cgfx
		ReadPixelArray( 
			buf1,				// DestRect .. Chunky
			0,					// DestX
			0,					// DestY
			cfg->GfxRead_Encode_FormatSize * ts,	// DestMod (Bytes per row)
			& scr->RastPort,	// RastPort
			sx,					// SrcX
			sy,					// SrcY
			ti[tilenr].W,		// SizeX (Width)
			ti[tilenr].H,		// SizeY (Height)
			cfg->GfxRead_Encode_Format2				// DestFormat
		);
	}

	// ############################################

	#elif defined(HAVE_P96)
	{
		struct RenderInfo ri;

		ri.Memory		= buf1;
		ri.BytesPerRow	= cfg->GfxRead_Encode_FormatSize * ts;
		ri.RGBFormat	= cfg->GfxRead_Encode_Format2;

		p96ReadPixelArray(
			& ri, 				// ri
			0,					// DestX, 
			0,					// DestY, 
			& scr->RastPort,	// rp 
			sx,					// SrcX, 
			sy,					// SrcY, 
			ti[tilenr].W,		// SizeX (Width)
			ti[tilenr].H		// SizeY (Height)
		);
	}

	// ############################################

	#elif defined(HAVE_GFX54)
	{
		// gfx v54
		ReadPixelArray( 
			& scr->RastPort,
			sx,				// srcx
			sy,				// srcy
			buf1,			// Chunky
			0,				// dstx
			0,				// dsty
			cfg->GfxRead_Encode_FormatSize * ts,	// Bytes per row
			cfg->GfxRead_Encode_Format2,			// Format
			ti[tilenr].W,	// Width
			ti[tilenr].H	// Height
		);
	}

	// ############################################

	#else

	#error Unknown Graphics System

	#endif

	// ############################################

	// --
{
//	struct VNCTimeVal start;
//	struct VNCTimeVal stop;
//	GetSysTime( (PTR) & start );


	ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

	#ifdef DO_OWN_CMP_CPY
	stat = myMemCompare( (PTR) buf2, (PTR) buf1, (U32) size ) ? 1 : 0;
	#else
	stat = memcmp( buf2, buf1, size ) ? 1 : 0;
	#endif

	if ( stat )
	{
		#ifndef DO_OWN_CMP_CPY
		memcpy( buf2, buf1, size ) ;
		#endif

		cfg->GfxRead_Screen_TileArrayBuffer[tilenr] = 0x80;
	}
	else
	{
		if ( cfg->GfxRead_Screen_TileArrayBuffer[tilenr] >= 0 )
		{
			 cfg->GfxRead_Screen_TileArrayBuffer[tilenr]++;
		}
	}

	ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );


//	GetSysTime( (PTR) & stop );

//	SubTime( (PTR) & stop, (PTR) & start );

//	static U64 __aa = 0;
//	static U64 __bb = 0;
//	__aa += stop.Microseconds;
//	__bb += 1;

//	DebugPrintF( "GfxRipper : %06lld : %06ld : %ld\n", __aa/__bb, stop.Microseconds, stat );
}



	// --

bailout:

	return;
}

// --

static S32 myTileCheck( struct Config *cfg )
{
PTR buf;
S32 size;
S32 err;

	err = TRUE;

	if ( cfg->GfxRead_Screen_TileCheckBuffer )
	{
		#ifdef _AOS4_
		UnlockMem( cfg->GfxRead_Screen_TileCheckBuffer, cfg->GfxRead_Screen_TileCheckSize );
		#endif
		FreeVec( cfg->GfxRead_Screen_TileCheckBuffer );
		cfg->GfxRead_Screen_TileCheckBuffer = NULL;
	}

	// 1x Tile chunky .. max 256x256x4
	size = cfg->GfxRead_Screen_TileSize * cfg->GfxRead_Screen_TileSize * 4;

	#ifdef _AOS4_

	buf = AllocVecTags( size,
		AVT_PhysicalAlignment, 4096,
		AVT_Contiguous, TRUE,
		AVT_Alignment, 4096,
		AVT_Lock, TRUE,
		TAG_END
	);

	#else

	buf = AllocVec( size, MEMF_ANY );

	#endif

	if ( ! buf )
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
U32 cnt;

	for( cnt=0 ; cnt<cfg->GfxRead_Screen_Tiles ; cnt++ )
	{
		myCheckTile( cfg, cnt );
	}
}

// --

static S32 myScreenChunky( struct Config *cfg )
{
PTR buf;
S32 size;
S32 err;

	err = TRUE;

	if ( cfg->GfxRead_Screen_ChunkyBuffer )
	{
		#ifdef _AOS4_
		UnlockMem( cfg->GfxRead_Screen_ChunkyBuffer, cfg->GfxRead_Screen_ChunkySize );
		#endif
		FreeVec( cfg->GfxRead_Screen_ChunkyBuffer );
		cfg->GfxRead_Screen_ChunkyBuffer = NULL;
	}

	// We may allocate more memory than the screen use
	// to fit the tiles.. and those border tiles need zero memory

	size = (( cfg->GfxRead_Screen_TileWidth  * cfg->GfxRead_Screen_TileSize ) * 
			( cfg->GfxRead_Screen_TileHeight * cfg->GfxRead_Screen_TileSize ) * ( 4 ));

	#ifdef _AOS4_

	buf = AllocVecTags( size,
		AVT_PhysicalAlignment, 4096,
		AVT_Contiguous, TRUE,
		AVT_Alignment, 4096,
		AVT_Clear, 0,
		AVT_Lock, TRUE,
		TAG_END
	);

	#else

	buf = AllocVec( size, MEMF_ANY | MEMF_CLEAR );

	#endif

	if ( ! buf )
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

static S32 myTileArray( struct Config *cfg )
{
PTR buf;
S32 size;
S32 err;

	err = TRUE;

	if ( cfg->GfxRead_Screen_TileArrayBuffer )
	{
		mem_Free( cfg->GfxRead_Screen_TileArrayBuffer );
		cfg->GfxRead_Screen_TileArrayBuffer = NULL;
	}

	size = cfg->GfxRead_Screen_Tiles;

	buf = mem_AllocClr( size * sizeof( S8 ));

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

static S32 myTilesInfo( struct Config *cfg )
{
struct TileInfo *ti;
S32 error;
S32 size;
S32 pos;
S32 ts;
S32 xx;
S32 yy;
S32 x;
S32 y;
S32 w;
S32 h;
S32 c;

	error = TRUE;

	if ( cfg->GfxRead_Screen_TileInfoBuffer )
	{
		mem_Free( cfg->GfxRead_Screen_TileInfoBuffer );
		cfg->GfxRead_Screen_TileInfoBuffer = NULL;
	}

	size = cfg->GfxRead_Screen_Tiles;

	ti = mem_Alloc( size * sizeof( struct TileInfo ));

	if ( ! ti )
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

			if ( x + ts > cfg->GfxRead_Screen_PageWidth )
			{
				w = ts - ( x + ts - cfg->GfxRead_Screen_PageWidth );
				c = TRUE;
			}

			if ( y + ts > cfg->GfxRead_Screen_PageHeight )
			{
				h = ts - ( y + ts - cfg->GfxRead_Screen_PageHeight );
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

static S32 myRandomizeTiles( struct Config *cfg )
{
S32 *buf;
S32 *rnd;
S32 offset;
S32 error;
S32 size;
S32 pos;
S32 val;
S32 i;

	error = TRUE;

	size = cfg->GfxRead_Screen_TileWidth * cfg->GfxRead_Screen_TileHeight;

	if ( cfg->GfxRead_Screen_TileRandomBuffer )
	{
		mem_Free( cfg->GfxRead_Screen_TileRandomBuffer );
		cfg->GfxRead_Screen_TileRandomBuffer = NULL;
	}

	buf = mem_Alloc( size * sizeof( S32 ));

	if ( buf == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "5: Error allocating memory. (%ld Bytes)", size * sizeof( S32 ));
		goto bailout;
	}

	cfg->GfxRead_Screen_TileRandomBuffer = buf;

	// --

	/**/ if ( cfg->cfg_Active_Settings.TileScanMethod == SCANMethod_Linear )
	{
		for( i=0 ; i<size ; i++ )
		{
			buf[i] = i;
		}
	}
	else // ( cfg->cfg_Active_Settings.TileScanMethod == SCANMethod_Random )
	{
		rnd = mem_Alloc( size * sizeof( S32 ));

		if ( rnd == NULL )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "6: Error allocating memory. (%ld Bytes)", size * sizeof( S32 ));
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

			memmove( & rnd[offset], &rnd[offset+1], ( i - offset - 1 ) * sizeof( S32 ) );
		}

		mem_Free( rnd );
	}

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

static void mySetScreen( struct Config *cfg, struct Screen *scr )
{
//U32 modeid;
//U32 pixfmt;
S32 err;

	ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

	err = TRUE;

	if ( scr == NULL )
	{
		cfg->GfxRead_Screen_Adr			= NULL;
		cfg->GfxRead_Screen_PageWidth	= 0;
		cfg->GfxRead_Screen_PageHeight	= 0;
		cfg->GfxRead_Screen_ViewWidth	= 0;
		cfg->GfxRead_Screen_ViewHeight	= 0;
		cfg->GfxRead_Screen_Format		= VNCPix_Unknown;
		cfg->GfxRead_Screen_TileWidth	= 0;
		cfg->GfxRead_Screen_TileHeight	= 0;
		cfg->GfxRead_Screen_Tiles		= 0;

		if ( cfg->GfxRead_Screen_TileCheckBuffer )
		{
			#ifdef _AOS4_
			UnlockMem( cfg->GfxRead_Screen_TileCheckBuffer, cfg->GfxRead_Screen_TileCheckSize );
			#endif
			FreeVec( cfg->GfxRead_Screen_TileCheckBuffer );
			cfg->GfxRead_Screen_TileCheckBuffer = NULL;
		}

		if ( cfg->GfxRead_Screen_ChunkyBuffer )
		{
			#ifdef _AOS4_
			UnlockMem( cfg->GfxRead_Screen_ChunkyBuffer, cfg->GfxRead_Screen_ChunkySize );
			#endif
			FreeVec( cfg->GfxRead_Screen_ChunkyBuffer );
			cfg->GfxRead_Screen_ChunkyBuffer = NULL;
		}

		if ( cfg->GfxRead_Screen_TileArrayBuffer )
		{
			mem_Free( cfg->GfxRead_Screen_TileArrayBuffer );
			cfg->GfxRead_Screen_TileArrayBuffer = NULL;
		}

		if ( cfg->GfxRead_Screen_TileInfoBuffer )
		{
			mem_Free( cfg->GfxRead_Screen_TileInfoBuffer );
			cfg->GfxRead_Screen_TileInfoBuffer = NULL;
		}

		if ( cfg->GfxRead_Screen_TileRandomBuffer )
		{
			mem_Free( cfg->GfxRead_Screen_TileRandomBuffer );
			cfg->GfxRead_Screen_TileRandomBuffer = NULL;
		}

		if ( cfg->GfxRead_PubScreenAdr )
		{
			UnlockPubScreen( NULL, cfg->GfxRead_PubScreenAdr );
			cfg->GfxRead_PubScreenAdr = NULL;
		}

		mySetEncoding_Format( cfg, VNCPix_Unknown );

		err = FALSE;
	}
	else
	{
		cfg->GfxRead_Screen_Adr			= scr;
		cfg->GfxRead_Screen_PageWidth	= scr->Width;
		cfg->GfxRead_Screen_PageHeight	= scr->Height;


		// ############################################

		#if defined(HAVE_CGFX)
		{
			// GfxBase : GetVPModeID v36
			U32 modeid = GetVPModeID( & scr->ViewPort );

			// CyberGfx
			U32 pixfmt = GetCyberIDAttr( CYBRIDATTR_PIXFMT,	modeid );

			cfg->GfxRead_Screen_ModeID		= modeid;
			cfg->GfxRead_Screen_Format		= myConvert_CGFX_2_VNC_Mode( pixfmt );
			cfg->GfxRead_Screen_ViewWidth	= GetCyberIDAttr( CYBRIDATTR_WIDTH, modeid );
			cfg->GfxRead_Screen_ViewHeight	= GetCyberIDAttr( CYBRIDATTR_HEIGHT, modeid );
		}

		// ############################################

		#elif defined(HAVE_P96)
		{
			// GFX v39
			U32 modeid = BestModeID( BIDTAG_ViewPort, & scr->ViewPort, TAG_END );

			// P96 
			RGBFTYPE pixfmt = p96GetBitMapAttr( scr->RastPort.BitMap, P96BMA_RGBFORMAT );

			// P96
			cfg->GfxRead_Screen_ModeID		= modeid;
			cfg->GfxRead_Screen_Format		= myConvert_P96_2_VNC_Mode( pixfmt );
			cfg->GfxRead_Screen_ViewWidth	= p96GetModeIDAttr( modeid, P96IDA_WIDTH );
			cfg->GfxRead_Screen_ViewHeight	= p96GetModeIDAttr( modeid, P96IDA_HEIGHT );
		}

		// ############################################

		#elif defined(HAVE_GFX54)
		{
			// GFX v39
			U32 modeid = BestModeID( BIDTAG_ViewPort, & scr->ViewPort, TAG_END );

			// GFX : GetBitMapAttr (v39/v54)
			enum enPixelFormat pixfmt = GetBitMapAttr( scr->RastPort.BitMap, BMA_PIXELFORMAT );

			// GFX v36
			struct DimensionInfo di;
			GetDisplayInfoData( NULL, (PTR) & di, sizeof( di ), DTAG_DIMS, modeid );

			// GFX
			cfg->GfxRead_Screen_ModeID		= modeid;
			cfg->GfxRead_Screen_Format		= myConvert_GFX54_2_VNC_Mode( pixfmt );
			cfg->GfxRead_Screen_ViewWidth	= di.Nominal.MaxX - di.Nominal.MinX + 1;
			cfg->GfxRead_Screen_ViewHeight	= di.Nominal.MaxY - di.Nominal.MinY + 1;
		}

		// ############################################

		#else

		#error Unknown Graphics System
 
		#endif

		// ############################################


//SHELLBUF_PRINTF( "isCGFX: %d\n", IsCyberModeID( cfg->GfxRead_Screen_ModeID ) ? TRUE : FALSE );
//SHELLBUF_PRINTF( "ModeID: %08" PRIx32 "\n", cfg->GfxRead_Screen_ModeID );
//SHELLBUF_PRINTF( "PixFmt: %08" PRIx32 "\n", cfg->GfxRead_Screen_Format );
//SHELLBUF_PRINTF( "Width : %" PRId32 "\n", cfg->GfxRead_Screen_ViewWidth );
//SHELLBUF_PRINTF( "Height: %" PRId32 "\n", cfg->GfxRead_Screen_ViewHeight );

		cfg->GfxRead_Screen_ViewMode	= cfg->cfg_Active_Settings.ScreenViewMode;

		if (( cfg->GfxRead_Screen_ViewMode == VIEWMODE_View ) && ( cfg->GfxRead_Screen_ModeID ))
		{
			cfg->GfxRead_Screen_TileWidth	= ( cfg->GfxRead_Screen_ViewWidth  + cfg->GfxRead_Screen_TileSize - 1 ) / cfg->GfxRead_Screen_TileSize;
			cfg->GfxRead_Screen_TileHeight	= ( cfg->GfxRead_Screen_ViewHeight + cfg->GfxRead_Screen_TileSize - 1 ) / cfg->GfxRead_Screen_TileSize;
			cfg->GfxRead_Screen_Tiles		= ( cfg->GfxRead_Screen_TileWidth  * cfg->GfxRead_Screen_TileHeight );
		}
		else
		{
			// if we don't have the ModeID, force Page mode
			cfg->GfxRead_Screen_ViewMode	= VIEWMODE_Page;
			cfg->GfxRead_Screen_TileWidth	= ( cfg->GfxRead_Screen_PageWidth  + cfg->GfxRead_Screen_TileSize - 1 ) / cfg->GfxRead_Screen_TileSize;
			cfg->GfxRead_Screen_TileHeight	= ( cfg->GfxRead_Screen_PageHeight + cfg->GfxRead_Screen_TileSize - 1 ) / cfg->GfxRead_Screen_TileSize;
			cfg->GfxRead_Screen_Tiles		= ( cfg->GfxRead_Screen_TileWidth  * cfg->GfxRead_Screen_TileHeight );
		}

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

	ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );
}

// --

static void myDetectScrChange( struct Config *cfg )
{
struct Screen *scr;
S32 lock;
S32 w;
S32 h;

	// --

// cfg->GfxRead_Screen_SupportChange = 1;

	if ( ! cfg->GfxRead_Screen_SupportChange )
	{
		if ( cfg->GfxRead_PubScreenAdr == NULL )
		{
			cfg->GfxRead_PubScreenAdr = LockPubScreen( NULL );
		}

		scr = cfg->GfxRead_PubScreenAdr;
	}
	else
	{
		if ( cfg->GfxRead_PubScreenAdr )
		{
			UnlockPubScreen( NULL, cfg->GfxRead_PubScreenAdr );
			cfg->GfxRead_PubScreenAdr = NULL;
		}

		// --

		lock = LockIBase( 0 );

		scr = IntuitionBase->FirstScreen;

		UnlockIBase( lock );
	}

	// --

	w = ( scr ) ? scr->Width  : 0;
	h = ( scr ) ? scr->Height : 0;

	if (( cfg->GfxRead_Screen_Adr == scr )
	&&	( cfg->GfxRead_Screen_PageWidth	== w )
	&&	( cfg->GfxRead_Screen_PageHeight == h ))
	{
		goto bailout;
	}

	// --

	mySetScreen( cfg, scr );

	#ifdef __RVNCD_GUI_H__
	myGUI_PxlFmtRefresh( cfg );
	#endif // __RVNCD_GUI_H__

	// --

bailout:

	return;
}

// --

static void myProcess_Main( struct Config *cfg )
{
S32 delay;
U32 wait;
U32 mask;
S32 pos;

	delay = 0;
	pos = 0;

	cfg->GfxRead_Screen_SupportChange = TRUE;

	wait = NET_EXIT_SIGNAL;

	#ifdef DEBUG
	DebugPrintF( "Gfx started\n" );
	#endif

	while( TRUE )  
	{
		mask = CheckSignal( wait );

		if ((( mask & ( NET_EXIT_SIGNAL )) == ( NET_EXIT_SIGNAL )) && ( cfg->GfxRead_Exit ))
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
						Delay( 2 );
						delay = 0;
					}
				}
			}
			else
			{
				Delay( 25 );
			}
		}
		else
		{
			if ( cfg->GfxRead_Screen_Adr )
			{
				mySetScreen( cfg, NULL );
			}

			cfg->cfg_UserDisconnectSignal = FALSE;
			Delay( 2 );
			pos = 0;
		}
	}

	#ifdef DEBUG
	DebugPrintF( "Gfx stopping\n" );
	#endif
}

// --

static S32 myProcess_Init( struct Config *cfg UNUSED )
{
S32 retval;

	retval = FALSE;


	retval = TRUE;

//bailout:

	return( retval );
}

// --

static void myProcess_Free( struct Config *cfg )
{
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
S32 stat;

	#ifdef DEBUG
	DebugPrintF( "Gfx starting 1/2\n" );
	#endif

	//--------

	Self = FindTask( NULL );

	while( TRUE )
	{
		sm = Self->tc_UserData;

		if ( sm )
		{
			break;
		}

		#ifdef DEBUG
		DebugPrintF( "Gfx starting delay\n" );
		#endif

		Delay( 2 );
	}

	#ifdef DEBUG
	DebugPrintF( "Gfx starting 2/2\n" );
	#endif

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
		Signal( Parent, SIGBREAKF_CTRL_E );

		// --

		#ifdef DEBUG
		DebugPrintF( "Gfx entering main\n" );
		#endif

		SetTaskPri( Self, PRI_GFX );
		myProcess_Main( Config );
		SetTaskPri( Self, PRI_SHUTDOWN );

		#ifdef DEBUG
		DebugPrintF( "Gfx exited main\n" );
		#endif

		// --

		Config->cfg_GfxReadStatus = PROCESS_Stopping;

		Parent = Config->GfxRead_Exit;

		Config->GfxRead_Task = NULL;
	}

	myProcess_Free( Config );

	Config->cfg_GfxReadStatus = PROCESS_Stopped;

	//--------

	#ifdef DEBUG
	DebugPrintF( "Gfx stopped\n" );
	#endif

	if ( Parent )
	{
		Forbid();
		Signal( Parent, SIGBREAKF_CTRL_F );
	}
}

// --

S32 myStart_Gfx_Read( struct Config *cfg )
{
struct StartMessage msg;
struct Process *process;
S32 error;
U32 mask;
U32 wait;

	error = TRUE;

	#ifdef DEBUG
	DebugPrintF( "myStart_Gfx_Read\n" );
	#endif

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "myStart_Gfx_Read\n" );
	}

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down

	while(( cfg->cfg_GfxReadStatus == PROCESS_Starting ) 
	||	  ( cfg->cfg_GfxReadStatus == PROCESS_Stopping ))
	{
		Delay( 2 );
	}

	if ( cfg->cfg_GfxReadStatus == PROCESS_Stopped )
	{
		msg.Parent = FindTask( NULL );
		msg.Config = cfg;

		#ifdef _AOS4_

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : Gfx Read Process",
			NP_Priority,	PRI_STARTUP,
			NP_Entry,		myServerProcess,
			NP_Child,		TRUE,
			NP_UserData,	& msg,
			TAG_END
		);

		#else

		process = CreateNewProcTags(
			NP_Name,		"rVNCd : Gfx Read Process",
			NP_Priority,	PRI_STARTUP,
			NP_Entry,		myServerProcess,
			TAG_END
		);

		#endif

		if ( process )
		{
			#ifndef _AOS4_
			process->pr_Task.tc_UserData = & msg;
			#endif

			wait = SIGBREAKF_CTRL_E | SIGBREAKF_CTRL_F ;

			while( TRUE )
			{
				mask = Wait( wait );

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
U32 mask;
S32 cnt;

	#ifdef DEBUG
	DebugPrintF( "myStop_Gfx_Read\n" );
	#endif

	if ( DoVerbose > 2 )
	{
		printf( "myStop_Gfx_Read\n" );
	}

	#ifdef DEBUG
	if ( cfg->UserCount > 0 )
	{
		SHELLBUF_PRINTF( "\n##\n## WARNING User(s) %d still connected\n##\n\n", cfg->UserCount );		
		Delay( 2 );
	}
	#endif

	// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	if ( cfg->cfg_GfxReadStatus != PROCESS_Stopped )
	{
		while(( cfg->cfg_GfxReadStatus == PROCESS_Starting ) 
		||	  ( cfg->cfg_GfxReadStatus == PROCESS_Stopping ))
		{
			Delay( 2 );
		}

		if ( cfg->cfg_GfxReadStatus == PROCESS_Running )
		{
			cfg->GfxRead_Exit = FindTask( NULL );

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF( "GfxRead: Sending CTRL+D+E\n" );
			}

			// Clear CTRL+F
			SetSignal( 0L, SIGBREAKF_CTRL_F );

			// Send Break Signal(s)
			Signal( cfg->GfxRead_Task, NET_EXIT_SIGNAL );

			cnt = 0;

			while( TRUE )
			{
				mask = CheckSignal( SIGBREAKF_CTRL_F );

				if ( mask & SIGBREAKF_CTRL_F )
				{
					break;
				}
				else
				{
					Delay( 5 );

					if ( ! ( ++cnt % 50 ))
					{
						Signal( cfg->GfxRead_Task, NET_EXIT_SIGNAL );
						printf( "myStop_Gfx_Read still waiting : %d\n", cnt );
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
