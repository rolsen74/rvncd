
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#ifdef SHOW_WORK_TILE
extern U32 TileOOO;
#endif

S32 NewBuffer_Update_Tile_Fill( struct Config *cfg, struct UpdateNode *un, U32 tilenr, S32 hardcursor )
{
enum VNCEncoding t;
S32 datalen;
S32 cnt;

	#ifdef SHOW_WORK_TILE
	TileOOO++;
	if ( TileOOO >= 20 )
	{
		DebugPrintF( "%4ld\n", tilenr );
		TileOOO = 0;
	}
	else
	{
		DebugPrintF( "%4ld ", tilenr );
	}
	#endif

	/*
	** Handle 1 Tile
	**
	** Return : -1 for Error, or the number of bytes to send
	**
	** Tile size   : max 256x256x4 bytes
	** Fill Buffer : NetSend_SendBuffer with data
	** Buffer size : 1 tile + 4096 + (4095 rounding) bytes
	*/

	// --
	// Get Tile Buffer (in correct Pixel format)

	ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

	datalen = cfg->GfxRead_Encode_RenderTile( cfg, cfg->NetSend_EncBuffer, tilenr );

	ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );

	if ( datalen <= 0 )
	{
		SHELLBUF_PRINTF( "myNewFill: GfxRead_Encode_RenderTile error\n" );
		goto bailout;
	}

	cfg->NetSend_EncBufferUsed = datalen;

	// --

	if ( ! hardcursor )
	{
		// Draw Soft Cursor on top of Tile, if we overlap
		NewBuffer_Cursor_Soft( cfg, cfg->NetSend_EncBuffer, tilenr );
	}

	// --

	datalen = -1;

	for( cnt=0 ; cnt<ENCODE_LAST ; cnt++ )
	{
		// We will loop untell datalen is positive.. 
		// as Raw is always enabled, this
		// should never fail.. (if done right)
		//
		// if an encoding fails, it should return
		// negative and we just try the next one
		//

		if ( ! cfg->NetSend_Encodings2[ cnt ].Enabled )
		{
			continue;
		}

		t = cfg->NetSend_Encodings2[ cnt ].Type;

		switch( t )
		{
			case ENCODE_Raw:
			{
				datalen = myEnc_Raw( cfg, un, tilenr );

				if ( datalen <= 0 )
				{
					// if this fail.. we are done
					goto bailout;
				}
				break;
			}

			case ENCODE_RRE:
			{
				if ( cfg->cfg_Active_Settings.RRE )
				{
					datalen = myEnc_RRE( cfg, un, tilenr );
				}
				break;
			}

			#ifdef HAVE_ZLIB
			case ENCODE_ZLib:
			{
				if ( cfg->cfg_Active_Settings.ZLib )
				{
					datalen = myEnc_ZLib( cfg, un, tilenr );
				}
				break;
			}
			#endif // HAVE_ZLIB

			default:
			{
//				DebugPrintF( "Unknown Encoding (%ld)\n", t );
				SHELLBUF_PRINTF( "Unknown Encoding : (%ld) :\n", t );
				break;
			}
		}

		if ( datalen > 0 )
		{
			// Done
			break;
		}
	}

	if ( datalen < 0 )
	{
		// One last try
		datalen = myEnc_Raw( cfg, un, tilenr );
	}

bailout:

	#ifdef DEBUG
	if ( datalen <= 0 )
	{
		SHELLBUF_PRINTF( "myNewFill: error datalen %ld\n", datalen );
	}
	#endif

	return( datalen );
}

// --
