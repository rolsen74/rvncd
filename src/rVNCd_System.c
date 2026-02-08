
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --
// --- Library Bases


#ifdef HAVE_ZLIB
struct ZLibBase *			ZBase				= NULL;
#endif // HAVE_ZLIB

// --
// --- AmigaOS4 Interfaces

#ifdef _AOS4_

#ifdef HAVE_ZLIB
struct ZIFace *				IZ					= NULL;
#endif // HAVE_ZLIB

#endif // _AOS4_

// --

S32 System_Startup( struct Config *cfg )
{
S32 retval;

	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "System_Startup\n" );
	}

	// --

	retval = FALSE;

	// --

	if ( ! System_Init( cfg ))
	{
		goto bailout;
	}

	// --

	if ( ! GFX_Init( cfg ))
	{
		goto bailout;
	}

 	// --- Open Timer

	if ( ! Timer_Init() )
	{
		goto bailout;		
	}

	// -- Open ZLib

	#ifdef HAVE_ZLIB

	if ( ! ZLib_Init( cfg ))
	{
		goto bailout;
	}

	#endif // HAVE_ZLIB

	retval = TRUE;

bailout:

	return( retval );
}

// --

void System_Shutdown( struct Config *cfg )
{
PTR node;

	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "System_Shutdown\n" );
	}

	// --

	while(( node = List_RemHead( & KeyLoggerList )))
	{
		mem_Free( node );
	}

	while(( node = List_RemHead( & LogStringList )))
	{
		mem_Free( node );
	}

	// -- Close Libraries and Devices

	Timer_Free();

	// -- Close ZLib

	#ifdef HAVE_ZLIB

	ZLib_Free( cfg );

	#endif // HAVE_ZLIB

	// --

	GFX_Free( cfg );

	// --

	Config_Free( cfg );
	System_Free();

	// --
}

// --
