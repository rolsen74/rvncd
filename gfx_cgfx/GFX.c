
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

struct Library *			CyberGfxBase		= NULL;

#ifdef _AOS4_
struct CyberGfxIFace *		ICyberGfx			= NULL;
#endif

// --

S32 GFX_Init( struct Config *cfg )
{
S32 retval;

	retval = FALSE;

	CyberGfxBase = (PTR) OpenLibrary( "cybergraphics.library", 53 );


	#ifdef _AOS4_

	ICyberGfx = (PTR) GetInterface( CyberGfxBase, "main", 1, NULL );

	if ( ! ICyberGfx )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening cybergraphics library (v53)" );
		goto bailout;
	}

	#endif

	retval = TRUE;

bailout:

	return( retval );
}

// --

void GFX_Free( struct Config *cfg )
{
	#ifdef _AOS4_

	if ( ICyberGfx )
	{
		DropInterface( (PTR) ICyberGfx );
		ICyberGfx = NULL;
	}

	#endif

	if ( CyberGfxBase )
	{
		CloseLibrary( (PTR) CyberGfxBase );
		CyberGfxBase = NULL;
	}
}

// --
