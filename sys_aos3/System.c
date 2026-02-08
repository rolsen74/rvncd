
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

//struct Library *			AslBase				= NULL;
struct Library *			DataTypesBase		= NULL;
//struct Library *			DiskfontBase		= NULL;
struct GfxBase *			GfxBase				= NULL;
//struct Library *			IconBase			= NULL;
struct IntuitionBase *		IntuitionBase		= NULL;
struct Library *			KeymapBase			= NULL;

// --

S32 System_Init( struct Config *cfg )
{
S32 retval;

	// --

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "System_Init\n" );
	}

	// --

	retval = FALSE;

//	AslBase = OpenLibrary( "asl.library", 39 );
//
//	if ( ! AslBase )
//	{
//		Log_PrintF( cfg, LOGTYPE_Error, "Error opening asl library (v39)" );
//		goto bailout;
//	}

	DataTypesBase = OpenLibrary( "datatypes.library", 30 );

	if ( ! DataTypesBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening datatypes library (v30)" );
		goto bailout;
	}

//	DiskfontBase = OpenLibrary( "diskfont.library", 39 );
//
//	if ( ! DiskfontBase )
//	{
//		Log_PrintF( cfg, LOGTYPE_Error, "Error opening diskfont library (v39)" );
//		goto bailout;
//	}

	GfxBase = (PTR) OpenLibrary( "graphics.library", 30 );

	if ( ! GfxBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening graphics library (v30)" );
		goto bailout;
	}

//	IconBase = (PTR) OpenLibrary( "icon.library", 39 );
//
//	if ( ! IconBase )
//	{
//		Log_PrintF( cfg, LOGTYPE_Error, "Error opening icon library (v39)" );
//		goto bailout;
//	}

	IntuitionBase = (PTR) OpenLibrary( "intuition.library", 30 );

	if ( ! IntuitionBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening intuition library (v30)" );
		goto bailout;
	}

	KeymapBase = OpenLibrary( "keymap.library", 30 );

	if ( ! KeymapBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening keymap library (v30)" );
		goto bailout;
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --

void System_Free( void )
{
	// Note Config have been free'd here
	// --

	// --

	if ( DoVerbose > 2 )
	{
		printf( "System_Free\n" );
	}

	// --

	if ( KeymapBase )
	{
		CloseLibrary( KeymapBase );
		KeymapBase = NULL;
	}

	if ( IntuitionBase )
	{
		CloseLibrary( (PTR) IntuitionBase );
		IntuitionBase = NULL;
	}

//	if ( IconBase )
//	{
//		CloseLibrary( IconBase );
//		IconBase = NULL;
//	}

	if ( GfxBase )
	{
		CloseLibrary( (PTR) GfxBase );
		GfxBase = NULL;
	}

//	if ( DiskfontBase )
//	{
//		CloseLibrary( DiskfontBase );
//		DiskfontBase = NULL;
//	}

	if ( DataTypesBase )
	{
		CloseLibrary( DataTypesBase );
		DataTypesBase = NULL;
	}

//	if ( AslBase )
//	{
//		CloseLibrary( AslBase );
//		AslBase = NULL;
//	}
}

// --
