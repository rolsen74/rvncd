
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --
// --- Library Bases

struct Library *			AslBase				= NULL;
struct Library *			CxBase				= NULL;
struct Library *			DataTypesBase		= NULL;
struct Library *			DiskfontBase		= NULL;
struct GfxBase *			GfxBase				= NULL;
struct Library *			IconBase			= NULL;
struct IntuitionBase *		IntuitionBase		= NULL;
struct Library *			KeymapBase			= NULL;

#ifdef HAVE_CGFX
struct Library *			CyberGfxBase		= NULL;
#endif // HAVE_CGFX

#ifdef HAVE_P96
struct Library *			P96Base				= NULL;
#endif // HAVE_P96

#ifdef HAVE_ZLIB
struct ZLibBase *			ZBase				= NULL;
#endif // HAVE_ZLIB

#ifndef HAVE_UTILITYLIB
struct UtilityBase *		UtilityBase			= NULL;
#endif // HAVE_UTILITYLIB

// --
// --- AmigaOS4 Interfaces

#ifdef _AOS4_
struct AslIFace * 			IAsl				= NULL;
struct CommoditiesIFace *	ICommodities		= NULL;
struct DataTypesIFace *		IDataTypes			= NULL;
struct DiskfontIFace *		IDiskfont			= NULL;
struct GraphicsIFace *  	IGraphics			= NULL;
struct IconIFace *			IIcon				= NULL;
struct IntuitionIFace * 	IIntuition			= NULL;
struct KeymapIFace *		IKeymap				= NULL;

#ifdef HAVE_CGFX
struct CyberGfxIFace *		ICyberGfx			= NULL;
#endif // HAVE_CGFX

#ifdef HAVE_P96
struct P96IFace *			IP96				= NULL;
#endif // HAVE_P96

#ifdef HAVE_ZLIB
struct ZIFace *				IZ					= NULL;
#endif // HAVE_ZLIB

#ifndef HAVE_UTILITYLIB
struct UtilityIFace *		IUtility			= NULL;
#endif // HAVE_UTILITYLIB

#endif // _AOS4_

// --

S32 System_Init( struct Config *cfg )
{
S32 retval;

	retval = FALSE;

	#ifdef _AOS4_

	AslBase = OpenLibrary( "asl.library", 53 );
	IAsl = (PTR) GetInterface( AslBase, "main", 1, NULL );

	if ( ! IAsl )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening asl library (v53)" );
		goto bailout;
	}

	DataTypesBase = OpenLibrary( "datatypes.library", 53 );
	IDataTypes = (PTR) GetInterface( DataTypesBase, "main", 1, NULL );

	if ( ! IDataTypes )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening datatypes library (v53)" );
		goto bailout;
	}

	DiskfontBase = OpenLibrary( "diskfont.library", 53 );
	IDiskfont = (PTR) GetInterface( DiskfontBase, "main", 1, NULL );

	if ( ! IDiskfont )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening diskfont library (v53)" );
		goto bailout;
	}

	GfxBase = (PTR) OpenLibrary( "graphics.library", 53 );
	IGraphics = (PTR) GetInterface( (PTR) GfxBase, "main", 1, NULL );

	if ( ! IGraphics )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening graphics library (v53)" );
		goto bailout;
	}

	IconBase = (PTR) OpenLibrary( "icon.library", 53 );
	IIcon = (PTR) GetInterface( (PTR) IconBase, "main", 1, NULL );

	if ( ! IIcon )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening icon library (v53)" );
		goto bailout;
	}

	IntuitionBase = (PTR) OpenLibrary( "intuition.library", 53 );
	IIntuition = (PTR) GetInterface( (PTR) IntuitionBase, "main", 1, NULL );

	if ( ! IIntuition )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening intuition library (v53)" );
		goto bailout;
	}

	KeymapBase = OpenLibrary( "keymap.library", 53 );
	IKeymap = (PTR) GetInterface( KeymapBase, "main", 1, NULL );

	if ( ! IKeymap )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening keymap library (v53)" );
		goto bailout;
	}

	#ifdef HAVE_CGFX

	CyberGfxBase = (PTR) OpenLibrary( "cybergraphics.library", 53 );
	ICyberGfx = (PTR) GetInterface( CyberGfxBase, "main", 1, NULL );

	if ( ! ICyberGfx )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening cybergraphics library (v53)" );
		goto bailout;
	}

	#endif // HAVE_CGFX

	#ifdef HAVE_P96

	P96Base = (PTR) OpenLibrary( "Picasso96API.library", 53 );
	IP96 = (PTR) GetInterface( (PTR) P96Base, "main", 1, NULL );

	if ( ! IP96 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening Picasso96 library (v53)" );
		goto bailout;
	}

	#endif // HAVE_P96

	UtilityBase = (PTR) OpenLibrary( "utility.library", 53 );
	IUtility = (PTR) GetInterface( (PTR) UtilityBase, "main", 1, NULL );

	if ( ! IUtility )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening utility library (v53)" );
		goto bailout;
	}

	#else // _AOS4

	AslBase = OpenLibrary( "asl.library", 39 );

	if ( ! AslBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening asl library (v39)" );
		goto bailout;
	}

	DataTypesBase = OpenLibrary( "datatypes.library", 39 );

	if ( ! DataTypesBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening datatypes library (v39)" );
		goto bailout;
	}

	DiskfontBase = OpenLibrary( "diskfont.library", 39 );

	if ( ! DiskfontBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening diskfont library (v39)" );
		goto bailout;
	}

	GfxBase = (PTR) OpenLibrary( "graphics.library", 39 );

	if ( ! GfxBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening graphics library (v39)" );
		goto bailout;
	}

	IconBase = (PTR) OpenLibrary( "icon.library", 39 );

	if ( ! IconBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening icon library (v39)" );
		goto bailout;
	}

	IntuitionBase = (PTR) OpenLibrary( "intuition.library", 39 );

	if ( ! IntuitionBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening intuition library (v39)" );
		goto bailout;
	}

	KeymapBase = OpenLibrary( "keymap.library", 39 );

	if ( ! KeymapBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening keymap library (v39)" );
		goto bailout;
	}

	#ifdef HAVE_CGFX

	CyberGfxBase = (PTR) OpenLibrary( "cybergraphics.library", 39 );

	if ( ! CyberGfxBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening cybergraphics library (v39)" );
		goto bailout;
	}

	#endif // HAVE_CGFX

	#ifdef HAVE_P96

	P96Base = (PTR) OpenLibrary( "Picasso96API.library", 39 );

	if ( ! P96Base )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening Picasso96 library (v39)" );
		goto bailout;
	}

	#endif // HAVE_P96

	UtilityBase = (PTR) OpenLibrary( "utility.library", 39 );

	if ( ! UtilityBase )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening utility library (v39)" );
		goto bailout;
	}

	#endif // AOS4_

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

void System_Free( struct Config *cfg UNUSED )
{
PTR node;

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
	// --- AmigaOS4 Interfaces
	// --

	#ifdef _AOS4_

	#ifdef NEWLIB

	if ( IUtility )
	{
		DropInterface( (PTR) IUtility );
		IUtility = NULL;
	}

	#endif // NEWLIB

	#ifdef HAVE_CGFX

	if ( ICyberGfx )
	{
		DropInterface( (PTR) ICyberGfx );
		ICyberGfx = NULL;
	}

	#endif // HAVE_CGFX

	#ifdef HAVE_P96

	if ( IP96 )
	{
		DropInterface( (PTR) IP96 );
		IP96 = NULL;
	}

	#endif // HAVE_CGFX

	if ( IKeymap )
	{
		DropInterface( (PTR) IKeymap );
		IKeymap = NULL;
	}

	if ( IIntuition )
	{
		DropInterface( (PTR) IIntuition );
		IIntuition = NULL;
	}

	if ( IIcon )
	{
		DropInterface( (PTR) IIcon );
		IIcon = NULL;
	}

	if ( IGraphics )
	{
		DropInterface( (PTR) IGraphics );
		IGraphics = NULL;
	}

	if ( IDiskfont )
	{
		DropInterface( (PTR) IDiskfont );
		IDiskfont = NULL;
	}

	if ( IDataTypes )
	{
		DropInterface( (PTR) IDataTypes );
		IDataTypes = NULL;
	}

	if ( IAsl )
	{
		DropInterface( (PTR) IAsl );
		IAsl = NULL;
	}

	#endif // _AOS4_

	// --
	// --- Library Bases
	// --

	#ifdef NEWLIB

	if ( UtilityBase )
	{
		CloseLibrary( (PTR) UtilityBase );
		UtilityBase = NULL;
	}

	#endif // NEWLIB

	#ifdef HAVE_CGFX

	if ( CyberGfxBase )
	{
		CloseLibrary( (PTR) CyberGfxBase );
		CyberGfxBase = NULL;
	}

	#endif // HAVE_CGFX

	#ifdef HAVE_P96

	if ( P96Base )
	{
		CloseLibrary( (PTR) P96Base );
		P96Base = NULL;
	}

	#endif // HAVE_P96

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

	if ( IconBase )
	{
		CloseLibrary( IconBase );
		IconBase = NULL;
	}

	if ( GfxBase )
	{
		CloseLibrary( (PTR) GfxBase );
		GfxBase = NULL;
	}

	if ( DiskfontBase )
	{
		CloseLibrary( DiskfontBase );
		DiskfontBase = NULL;
	}

	if ( DataTypesBase )
	{
		CloseLibrary( DataTypesBase );
		DataTypesBase = NULL;
	}

	if ( AslBase )
	{
		CloseLibrary( AslBase );
		AslBase = NULL;
	}
}

// --
