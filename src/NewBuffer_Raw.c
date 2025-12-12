
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 myEnc_Raw( struct Config *cfg, struct UpdateNode *un, U32 tilenr )
{
S32 datalen;

	/**/ if ( cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel == 0 )
	{
		datalen = -1;
		Log_PrintF( cfg, LOGTYPE_Error, "Unsupported Bits width (%ld)", cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel );
	}
	else if ( cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel <= 8 )
	{
		datalen = myEnc_Raw_8( cfg, un, tilenr );
	}
	else if ( cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel <= 16 )
	{
		datalen = myEnc_Raw_16( cfg, un, tilenr );
	}
	else if ( cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel <= 24 )
	{
		datalen = -1;
		Log_PrintF( cfg, LOGTYPE_Error, "Unsupported Bits width (%ld)", cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel );
	}
	else if ( cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel <= 32 )
	{
		datalen = myEnc_Raw_32( cfg, un, tilenr );
	}
	else
	{
		datalen = -1;
		Log_PrintF( cfg, LOGTYPE_Error, "Unsupported Bits width (%ld)", cfg->GfxRead_Encode_ActivePixel.pm_BitsPerPixel );
	}

	return( datalen );
}

// --
