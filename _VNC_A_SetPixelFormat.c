 
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
** - Client sets what Pixel format we should use
**
** Returns:
** - True and the socket will be closed
** - False and we continue
*/

#if 0

struct PixelMessage
{
	uint8	pm_Type;
	uint8	pm_Pad;
	uint8	pm_Pad2;
	uint8	pm_Pad3;
	uint8	pm_BitsPerPixel;
	uint8	pm_Depth;
	uint8	pm_BigEndian;
	uint8	pm_TrueColor;
	uint16	pm_RedMax;
	uint16	pm_GreenMax;
	uint16	pm_BlueMax;
	uint8	pm_RedShift;
	uint8	pm_GreenShift;
	uint8	pm_BlueShift;
	uint8	pm_Pad4;
	uint8	pm_Pad5;
	uint8	pm_Pad6;
};

#endif

int VNC_SetPixelFormat( struct Config *cfg )
{
struct PixelMessage *buf;
struct PixelMessage *pm;
struct SocketIFace *ISocket;
int error;
int size;
int rc;

	ISocket = cfg->NetRead_ISocket;

	error = TRUE;

	buf = cfg->NetRead_ReadBuffer;

	size = sizeof( struct PixelMessage );

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

	if (( buf->pm_Type != 0 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%d != %d)", rc, size );
		goto bailout;
	}

	//
	// Ultra VNC client, like to send SetPixelFormat and SetEncoding over
	// and over at random times .. so we only reactive if something change
	//

	if ( DoVerbose )
	{
		printf( "Client Request PixelFormat\n" );
		printf( "\n" );
		printf( " BitsPerPixel.: %d\n", buf->pm_BitsPerPixel );
		printf( " Pad..........: %d\n", buf->pm_Pad );
		printf( " Pad2.........: %d\n", buf->pm_Pad2 );
		printf( " Pad3.........: %d\n", buf->pm_Pad3 );
		printf( " Depth........: %d\n", buf->pm_Depth );
		printf( " BigEndian....: %d\n", buf->pm_BigEndian );
		printf( " TrueColor....: %d\n", buf->pm_TrueColor );
		printf( " RedMax.......: %d\n", buf->pm_RedMax );
		printf( " GreenMax.....: %d\n", buf->pm_GreenMax );
		printf( " BlueMax......: %d\n", buf->pm_BlueMax );
		printf( " RedShift.....: %d\n", buf->pm_RedShift );
		printf( " GreenShift...: %d\n", buf->pm_GreenShift );
		printf( " BlueShift....: %d\n", buf->pm_BlueShift );
		printf( " Pad4.........: %d\n", buf->pm_Pad4 );
		printf( " Pad5.........: %d\n", buf->pm_Pad5 );
		printf( " Pad6.........: %d\n", buf->pm_Pad6 );
		printf( "\n" );
	}

	buf->pm_Pad		= 0;
	buf->pm_Pad2	= 0;
	buf->pm_Pad3	= 0;
	buf->pm_Pad4	= 0;
	buf->pm_Pad5	= 0;
	buf->pm_Pad6	= 0;

	pm = & cfg->GfxRead_Enocde_ActivePixel;

	if ( memcmp( pm, buf, sizeof( struct PixelMessage )))
	{
		if ( DoVerbose )
		{
			printf( "Updating PixelFormat\n\n" );
		}

		mySetEncoding_Message( cfg, buf, TRUE );

		cfg->cfg_UpdateCursor = TRUE;
	}
	else
	{
		if ( DoVerbose )
		{
			printf( "Keeping PixelFormat (Nothing changed)\n\n" );
		}
	}

	cfg->cfg_ServerGotSetPixelFormat = TRUE;

	error = FALSE;

bailout:

	return( error );
}

// --
