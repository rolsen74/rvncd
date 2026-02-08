
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

struct Library *			AslBase				= NULL;
struct Library *			DataTypesBase		= NULL;
struct Library *			DiskfontBase		= NULL;
struct GfxBase *			GfxBase				= NULL;
struct Library *			IconBase			= NULL;
struct IntuitionBase *		IntuitionBase		= NULL;
struct Library *			KeymapBase			= NULL;

struct AslIFace * 			IAsl				= NULL;
struct DataTypesIFace *		IDataTypes			= NULL;
struct DiskfontIFace *		IDiskfont			= NULL;
struct GraphicsIFace *  	IGraphics			= NULL;
struct IconIFace *			IIcon				= NULL;
struct IntuitionIFace * 	IIntuition			= NULL;
struct KeymapIFace *		IKeymap				= NULL;

#ifdef NEWLIB
struct UtilityBase *		UtilityBase			= NULL;
struct UtilityIFace *		IUtility			 = NULL;
#endif

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

	// --

	AslBase = OpenLibrary( "asl.library", 53 );
	IAsl = (PTR) GetInterface( AslBase, "main", 1, NULL );

	if ( ! IAsl )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening asl library (v53)" );
		goto bailout;
	}

	// --

	DataTypesBase = OpenLibrary( "datatypes.library", 53 );
	IDataTypes = (PTR) GetInterface( DataTypesBase, "main", 1, NULL );

	if ( ! IDataTypes )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening datatypes library (v53)" );
		goto bailout;
	}

	// --

	DiskfontBase = OpenLibrary( "diskfont.library", 53 );
	IDiskfont = (PTR) GetInterface( DiskfontBase, "main", 1, NULL );

	if ( ! IDiskfont )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening diskfont library (v53)" );
		goto bailout;
	}

	// --

	GfxBase = (PTR) OpenLibrary( "graphics.library", 53 );
	IGraphics = (PTR) GetInterface( (PTR) GfxBase, "main", 1, NULL );

	if ( ! IGraphics )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening graphics library (v53)" );
		goto bailout;
	}

	// --

	IconBase = (PTR) OpenLibrary( "icon.library", 53 );
	IIcon = (PTR) GetInterface( (PTR) IconBase, "main", 1, NULL );

	if ( ! IIcon )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening icon library (v53)" );
		goto bailout;
	}

	// --

	IntuitionBase = (PTR) OpenLibrary( "intuition.library", 53 );
	IIntuition = (PTR) GetInterface( (PTR) IntuitionBase, "main", 1, NULL );

	if ( ! IIntuition )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening intuition library (v53)" );
		goto bailout;
	}

	// --

	KeymapBase = OpenLibrary( "keymap.library", 53 );
	IKeymap = (PTR) GetInterface( KeymapBase, "main", 1, NULL );

	if ( ! IKeymap )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening keymap library (v53)" );
		goto bailout;
	}

	// --

	#ifdef NEWLIB

	UtilityBase = (PTR) OpenLibrary( "utility.library", 53 );
	IUtility = (PTR) GetInterface( (PTR) UtilityBase, "main", 1, NULL );

	if ( ! IUtility )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening utility library (v53)" );
		goto bailout;
	}

	#endif

	// --

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

	if ( IKeymap )
	{
		DropInterface( (PTR) IKeymap );
		IKeymap = NULL;
	}

	if ( KeymapBase )
	{
		CloseLibrary( KeymapBase );
		KeymapBase = NULL;
	}

	// --

	#ifdef NEWLIB

	if ( IUtility )
	{
		DropInterface( (PTR) IUtility );
		IUtility = NULL;
	}

	if ( UtilityBase )
	{
		CloseLibrary( (PTR) UtilityBase );
		UtilityBase = NULL;
	}

	#endif // NEWLIB

	// --

	if ( IIntuition )
	{
		DropInterface( (PTR) IIntuition );
		IIntuition = NULL;
	}

	if ( IntuitionBase )
	{
		CloseLibrary( (PTR) IntuitionBase );
		IntuitionBase = NULL;
	}

	// --

	if ( IIcon )
	{
		DropInterface( (PTR) IIcon );
		IIcon = NULL;
	}

	if ( IconBase )
	{
		CloseLibrary( IconBase );
		IconBase = NULL;
	}

	// --

	if ( IGraphics )
	{
		DropInterface( (PTR) IGraphics );
		IGraphics = NULL;
	}

	if ( GfxBase )
	{
		CloseLibrary( (PTR) GfxBase );
		GfxBase = NULL;
	}

	// --

	if ( IDiskfont )
	{
		DropInterface( (PTR) IDiskfont );
		IDiskfont = NULL;
	}

	if ( DiskfontBase )
	{
		CloseLibrary( DiskfontBase );
		DiskfontBase = NULL;
	}

	// --

	if ( IDataTypes )
	{
		DropInterface( (PTR) IDataTypes );
		IDataTypes = NULL;
	}

	if ( DataTypesBase )
	{
		CloseLibrary( DataTypesBase );
		DataTypesBase = NULL;
	}

	// --

	if ( IAsl )
	{
		DropInterface( (PTR) IAsl );
		IAsl = NULL;
	}

	if ( AslBase )
	{
		CloseLibrary( AslBase );
		AslBase = NULL;
	}
}

// --
