
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

struct Library *			P96Base				= NULL;

#ifdef _AOS4_
struct P96IFace *			IP96				= NULL;
#endif

// --

S32 GFX_Init( struct Config *cfg )
{
S32 retval;

	retval = FALSE;

	P96Base = (PTR) OpenLibrary( "Picasso96API.library", 0 );

	if ( ! P96Base )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening Picasso96API library (v0)" );
		goto bailout;
	}

	#ifdef _AOS4_

	IP96 = (PTR) GetInterface( (PTR) P96Base, "main", 1, NULL );

	if ( ! IP96 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining Picasso96 interface" );
		goto bailout;
	}

	#endif

	retval = TRUE;

bailout:

	return( retval );
}

// --

void GFX_Free( struct Config *cfg UNUSED )
{
	#ifdef _AOS4_

	if ( IP96 )
	{
		DropInterface( (PTR) IP96 );
		IP96 = NULL;
	}

	#endif

	if ( P96Base )
	{
		CloseLibrary( (PTR) P96Base );
		P96Base = NULL;
	}
}

// --
