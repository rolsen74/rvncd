 
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

struct BufferRect
{
	uint16	br_XPos;
	uint16	br_YPos;
	uint16	br_Width;
	uint16	br_Height;
	uint32	br_Encoding;
};

#pragma pack(0)

// --

static void Send_Render_Pointer( struct Config *cfg, void *buffer, uint8 *mask, int pw, int ph )
{
uint8 maskdata[] = { 0x80, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01 };
uint32 *data32;
uint32 col32;
uint16 *data16;
uint16 col16;
uint8 *chunky;
uint8 *data8;
uint8 col8;
int bytewidth;
int bytesize;
int maskpos;
int pos;
int rs;
int gs;
int bs;
int x;
int y;
int r;
int g;
int b;

	data32	= buffer;
	data16	= buffer;
	data8	= buffer;

	bytewidth = ( pw + 7 ) / 8;
	bytesize  = ( cfg->GfxRead_Enocde_ActivePixel.pm_BitsPerPixel + 7 ) / 8;

	// --

	rs = 8;
	y = 1;

	while( y < cfg->GfxRead_Enocde_ActivePixel.pm_RedMax )
	{
		y = y << 1;
		rs--;
	}

	// --

	gs = 8;
	y = 1;

	while( y < cfg->GfxRead_Enocde_ActivePixel.pm_GreenMax )
	{
		y = y << 1;
		gs--;
	}

	// --

	bs = 8;
	y = 1;

	while( y < cfg->GfxRead_Enocde_ActivePixel.pm_BlueMax )
	{
		y = y << 1;
		bs--;
	}

	// --

	chunky = cfg->cfg_PointerChunky;

	// --

	for( y=0 ; y<ph ; y++ )
	{
		for( x=0 ; x<pw ; x++ )
		{
			pos = (( y * 64 * 4 ) + ( x * 4 ));

			//-- 

			if ( chunky[ pos + 0 ] > 25 ) // Check Alpha Channel
			{
				maskpos = ( bytewidth * y ) + ( x / 8 );

				mask[maskpos] |= maskdata[ x & 7 ];
			}

			// --

			r = chunky[ pos + 1 ] >> rs;
			g = chunky[ pos + 2 ] >> gs;
			b = chunky[ pos + 3 ] >> bs;

			/**/ if ( bytesize == 1 )
			{
				col8  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift );
				col8 |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
				col8 |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift );

				*data8++ = col8;
			}
			else if ( bytesize == 2 )
			{
				col16  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift );
				col16 |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
				col16 |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift );

				if ( cfg->GfxRead_Enocde_ActivePixel.pm_BigEndian )
				{
					*data16++ = col16;
				}
				else
				{
					*data16++ = SWAP16( col16 );
				}
			}
			else if ( bytesize == 4 )
			{
				col32  = ( r << cfg->GfxRead_Enocde_ActivePixel.pm_RedShift );
				col32 |= ( g << cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift );
				col32 |= ( b << cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift );

				if ( cfg->GfxRead_Enocde_ActivePixel.pm_BigEndian )
				{
					*data32++ = col32;
				}
				else
				{
					*data32++ = SWAP32( col32 );
				}
			}
		}
	}

	return;
}

// --

void NewBuffer_Cursor( struct Config *cfg )
{
struct SocketIFace *ISocket;
struct BufferRect *rect;
uint8 *buffer;
uint8 *mask;
uint8 *gfx;
int masksize;
int gfxsize;
int size;
int rc;
int pw;
int ph;
int bw;

	// --

	#if 0
	if ( ! cfg->NetSend_ClientPixelFormatSet )
	{
		printf( "%s%04d: Client Pixel Format not Set\n", __FILE__, __LINE__ );
		goto bailout;
	}
	#endif

	// --

	pw = cfg->cfg_PointerWidth;		// Cursor Pixel Width
	ph = cfg->cfg_PointerHeight;	// Cursor Pixel Height
	bw = ( pw + 7 ) / 8;			// Byte Width

	gfxsize  = pw * ph * ( cfg->GfxRead_Enocde_ActivePixel.pm_BitsPerPixel / 8 );
	masksize = bw * ph;

	// --

	buffer = cfg->NetSend_SendBuffer;

	size = sizeof( struct BufferRect ) + gfxsize + masksize;

	memset( buffer, 0, size );

	rect	= (APTR) buffer;
	gfx		= & buffer[ sizeof( struct BufferRect ) ];
	mask	= & buffer[ sizeof( struct BufferRect ) + gfxsize ];

	// -- Rect
//	rect->br_XPos		= 0;
//	rect->br_YPos		= 0;
	rect->br_Width		= pw;
	rect->br_Height		= ph;
	rect->br_Encoding	= -239; // Cursor

	// -- Draw Cursor

	Send_Render_Pointer( cfg, gfx, mask, pw, ph );

	// --

	ISocket = cfg->NetSend_ISocket;

	if ( size <= cfg->NetSend_SendBufferSize )
	{
		rc = ISocket->send( cfg->NetSend_ClientSocket, cfg->NetSend_SendBuffer, size, 0 );

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
			cfg->cfg_NetReason = myASPrintF( "Internal: Buffer overflow" );
		}

		rc = -1;
		Log_PrintF( cfg, LOGTYPE_Error, "Buffer overflow" );
		goto bailout;
	}

	cfg->SessionStatus.si_Send_Bytes += rc;

	// --

bailout:

	return;
}

// --

