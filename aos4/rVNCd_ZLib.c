
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifdef HAVE_ZLIB

// --

#include "rVNCd.h"

// --

S32 ZLib_Init( struct Config *cfg )
{
S32 retval;

	retval = FALSE;

	ZBase = (PTR) OpenLibrary( "z.library", 53 );
	IZ = (PTR) GetInterface( (PTR) ZBase, "main", 1, NULL );

	if ( ! IZ )
	{
		Log_PrintF( cfg, LOGTYPE_Warning, "Error opening z.library (v53), Disabling support" );
	}

	retval = TRUE;

//bailout:

	return( retval );
}

// --

void ZLib_Free( struct Config *cfg UNUSED )
{
	if ( IZ )
	{
		DropInterface( (PTR) IZ );
		IZ = NULL;
	}

	if ( ZBase )
	{
		CloseLibrary( (PTR) ZBase );
		ZBase = NULL;
	}
}

// --

void ZLib_Cleanup( struct Config *cfg )
{
	if ( cfg->cfg_ServerZLibAvailable )
	{
		DeflateEnd( & cfg->cfg_zStream );
		cfg->cfg_ServerZLibAvailable = FALSE;
	}
}

// --

void ZLib_Setup( struct Config *cfg )
{
S32 stat;

	if ( ! IZ )
	{
		// ZLib library not available
		goto bailout;
	}

	if ( ! cfg->cfg_Disk_Settings.ZLib )
	{
		// ZLib not enabled
		goto bailout;
	}

	if ( cfg->cfg_ServerZLibAvailable )
	{
		// Allready setup
		goto bailout;
	}

//	zs->cfg_zStream.data_type = Z_BINARY;

	stat = DeflateInit( & cfg->cfg_zStream, Z_DEFAULT_COMPRESSION );

	if ( stat != Z_OK )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to initalize zlib stream, disabling support" );
		goto bailout;
	}

	cfg->cfg_ServerZLibAvailable = TRUE;

bailout:

	if ( ! cfg->cfg_ServerZLibAvailable )
	{
		ZLib_Cleanup( cfg );
	}
}

// --

#endif // HAVE_ZLIB
