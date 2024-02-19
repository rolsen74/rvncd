 
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
** - Server send Screen Info to Client
**
** Returns:
** - True and the socket will be closed
** - False and we continue
*/

#pragma pack(1)

struct ServerInitMessage
{
	uint16	sim_Width;
	uint16	sim_Height;
	uint8	sim_BitsPerPixel;
	uint8	sim_Depth;
	uint8	sim_BigEndian;
	uint8	sim_TrueColor;
	uint16	sim_RedMax;
	uint16	sim_GreenMax;
	uint16	sim_BlueMax;
	uint8	sim_RedShift;
	uint8	sim_GreenShift;
	uint8	sim_BlueShift;
	uint8	sim_Pad;
	uint8	sim_Pad2;
	uint8	sim_Pad3;
	uint32	sim_NameLength;
	// Name String
};

#pragma pack(0)

int VNC_ServerInit( struct Config *cfg )
{
struct ServerInitMessage *sim;
struct SocketIFace *ISocket;
char *str;
int rejected;
int size;
int len;
int rc;

// IExec->DebugPrintF( "VNC_ServerInit\n" );

	rejected = TRUE;

	if ( ! cfg->GfxRead_Screen_Adr )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed getting Screen Info" );
		goto bailout;
	}

	ISocket = cfg->NetSend_ISocket;
	sim	= cfg->NetSend_SendBuffer;
	str = cfg->NetSend_SendBuffer;

	// --

	size = sizeof( struct ServerInitMessage );

	memset( sim, 0, size );

	sim->sim_Width			= cfg->GfxRead_Screen_Width;
	sim->sim_Height			= cfg->GfxRead_Screen_Height;
	sim->sim_BitsPerPixel	= cfg->GfxRead_Enocde_ActivePixel.pm_BitsPerPixel;
	sim->sim_Depth			= cfg->GfxRead_Enocde_ActivePixel.pm_Depth;
	sim->sim_BigEndian		= cfg->GfxRead_Enocde_ActivePixel.pm_BigEndian;
	sim->sim_TrueColor		= cfg->GfxRead_Enocde_ActivePixel.pm_TrueColor;
	sim->sim_RedMax			= cfg->GfxRead_Enocde_ActivePixel.pm_RedMax;
	sim->sim_GreenMax		= cfg->GfxRead_Enocde_ActivePixel.pm_GreenMax;
	sim->sim_BlueMax		= cfg->GfxRead_Enocde_ActivePixel.pm_BlueMax;
	sim->sim_RedShift		= cfg->GfxRead_Enocde_ActivePixel.pm_RedShift;
	sim->sim_GreenShift		= cfg->GfxRead_Enocde_ActivePixel.pm_GreenShift;
	sim->sim_BlueShift		= cfg->GfxRead_Enocde_ActivePixel.pm_BlueShift;

	if ( DoVerbose )
	{
		printf( "Server Default PixelFormat\n" );
		printf( "\n" );
		printf( " Width........: %d\n", sim->sim_Width );
		printf( " Height.......: %d\n", sim->sim_Height );
		printf( " BitsPerPixel.: %d\n", sim->sim_BitsPerPixel );
		printf( " Depth........: %d\n", sim->sim_Depth );
		printf( " BigEndian....: %d\n", sim->sim_BigEndian );
		printf( " TrueColor....: %d\n", sim->sim_TrueColor );
		printf( " RedMax.......: %d\n", sim->sim_RedMax );
		printf( " GreenMax.....: %d\n", sim->sim_GreenMax );
		printf( " BlueMax......: %d\n", sim->sim_BlueMax );
		printf( " RedShift.....: %d\n", sim->sim_RedShift );
		printf( " GreenShift...: %d\n", sim->sim_GreenShift );
		printf( " BlueShift....: %d\n", sim->sim_BlueShift );
		printf( "\n" );
	}

	// --

	len = strlen( cfg->cfg_Active_Settings.Name );

	if ( len )
	{
		memcpy( & str[ size ], cfg->cfg_Active_Settings.Name, len );
	}

	sim->sim_NameLength = len;

	// --

	rc = ISocket->send( cfg->NetSend_ClientSocket, sim, size + len, 0 );

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

	// --

	rejected = FALSE;

bailout:

// IExec->DebugPrintF( "VNC_ServerInit (%d)\n", rejected );

	return( rejected );
}

// --
