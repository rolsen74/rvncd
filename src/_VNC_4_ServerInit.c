
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if 0

struct ServerInitMessage
{
	U16	sim_Width;
	U16	sim_Height;
	U8	sim_BitsPerPixel;
	U8	sim_Depth;
	U8	sim_BigEndian;
	U8	sim_TrueColor;
	U16	sim_RedMax;
	U16	sim_GreenMax;
	U16	sim_BlueMax;
	U8	sim_RedShift;
	U8	sim_GreenShift;
	U8	sim_BlueShift;
	U8	sim_Pad;
	U8	sim_Pad2;
	U8	sim_Pad3;
	U32	sim_NameLength;
	// Name String
};

#endif

S32 VNC_ServerInit( struct Config *cfg )
{
struct ServerInitMessage *sim;
STR str;
S32 rejected;
S32 size;
S32 len;
S32 rc;

	rejected = TRUE;

	if ( ! cfg->GfxRead_Screen_Adr )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed getting Screen Info" );
		goto bailout;
	}

	sim	= (PTR) cfg->NetSend_SendBuffer;
	str = (STR) cfg->NetSend_SendBuffer;

	// --

	size = sizeof( struct ServerInitMessage );

	memset( sim, 0, size );

	if ( cfg->GfxRead_Screen_ViewMode == VIEWMODE_View )
	{
		sim->sim_Width		= cfg->GfxRead_Screen_ViewWidth;
		sim->sim_Height		= cfg->GfxRead_Screen_ViewHeight;
	}
	else
	{
		sim->sim_Width		= cfg->GfxRead_Screen_PageWidth;
		sim->sim_Height		= cfg->GfxRead_Screen_PageHeight;
	}

	sim->sim_BitsPerPixel	= cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel;
	sim->sim_Depth			= cfg->GfxRead_Encode_ActivePixel.pm_Depth;
	sim->sim_BigEndian		= cfg->GfxRead_Encode_ActivePixel.pm_BigEndian;
	sim->sim_TrueColor		= cfg->GfxRead_Encode_ActivePixel.pm_TrueColor;
	sim->sim_RedMax			= cfg->GfxRead_Encode_ActivePixel.pm_RedMax;
	sim->sim_GreenMax		= cfg->GfxRead_Encode_ActivePixel.pm_GreenMax;
	sim->sim_BlueMax		= cfg->GfxRead_Encode_ActivePixel.pm_BlueMax;
	sim->sim_RedShift		= cfg->GfxRead_Encode_ActivePixel.pm_RedShift;
	sim->sim_GreenShift		= cfg->GfxRead_Encode_ActivePixel.pm_GreenShift;
	sim->sim_BlueShift		= cfg->GfxRead_Encode_ActivePixel.pm_BlueShift;

	if ( DoVerbose > 1 )
	{
//		Disable();
		SHELLBUF_LOCK();
		SHELLBUF_PRINTF( "Server Default PixelFormat\n" );
		SHELLBUF_PRINTF( "\n" );
		SHELLBUF_PRINTF( " Width........: %" PRId32 "\n", (S32) sim->sim_Width );
		SHELLBUF_PRINTF( " Height.......: %" PRId32 "\n", (S32) sim->sim_Height );
		SHELLBUF_PRINTF( " BitsPerPixel.: %" PRId32 "\n", (S32) sim->sim_BitsPerPixel );
		SHELLBUF_PRINTF( " Depth........: %" PRId32 "\n", (S32) sim->sim_Depth );
		SHELLBUF_PRINTF( " BigEndian....: %" PRId32 "\n", (S32) sim->sim_BigEndian );
		SHELLBUF_PRINTF( " TrueColor....: %" PRId32 "\n", (S32) sim->sim_TrueColor );
		SHELLBUF_PRINTF( " RedMax.......: %" PRId32 "\n", (S32) sim->sim_RedMax );
		SHELLBUF_PRINTF( " GreenMax.....: %" PRId32 "\n", (S32) sim->sim_GreenMax );
		SHELLBUF_PRINTF( " BlueMax......: %" PRId32 "\n", (S32) sim->sim_BlueMax );
		SHELLBUF_PRINTF( " RedShift.....: %" PRId32 "\n", (S32) sim->sim_RedShift );
		SHELLBUF_PRINTF( " GreenShift...: %" PRId32 "\n", (S32) sim->sim_GreenShift );
		SHELLBUF_PRINTF( " BlueShift....: %" PRId32 "\n", (S32) sim->sim_BlueShift );
		SHELLBUF_PRINTF( "\n" );
		SHELLBUF_UNLOCK();
//		Enable();
	}

	// --

	len = strlen( cfg->cfg_Active_Settings.Name );

	if ( len )
	{
		memcpy( & str[ size ], cfg->cfg_Active_Settings.Name, len );
	}

	sim->sim_NameLength = len;

	// --

	rc = Func_NetSend( cfg, sim, size + len );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	// --

	rejected = FALSE;

bailout:

	return( rejected );
}

// --
