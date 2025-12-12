
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

// Make sure only One GUI is included, so reuse name
#ifndef __HAVE__RVNCD__GUI__
#define __HAVE__RVNCD__GUI__
#ifdef GUI_RA

// --

S32 GUIFunc_Init( struct Config *cfg )
{
S32 retval;

//	DebugPrintF( "RA: GUIFunc_Init\n" );

	retval = FALSE;

	if ( ! MsgPort_Init( & WinMsgPort ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Window MsgPort" );
		goto bailout;
	}

	if ( ! MsgPort_Init( & WinAppPort ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Window AppPort" );
		goto bailout;
	}

	// --

	BitMapBase = (PTR) OpenClass( "images/bitmap.image", 53, & BitMapClass );

	if ( BitMapBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening label bitmap class" );
		goto bailout;
	}

	ButtonBase = (PTR) OpenClass( "gadgets/button.gadget", 53, & ButtonClass );

	if ( ButtonBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening button gadget class" );
		goto bailout;
	}

	CheckBoxBase = (PTR) OpenClass( "gadgets/checkbox.gadget", 53, & CheckBoxClass );

	if ( CheckBoxBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening checkbox gadget class" );
		goto bailout;
	}

	ChooserBase = (PTR) OpenClass( "gadgets/chooser.gadget", 53, & ChooserClass );

	if ( ChooserBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening chooser gadget class" );
		goto bailout;
	}

	ClickTabBase = (PTR) OpenClass( "gadgets/clicktab.gadget", 53, & ClickTabClass );

	if ( ClickTabBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening clicktab gadget class" );
		goto bailout;
	}

	GetFileBase = (PTR) OpenClass( "gadgets/getfile.gadget", 53, & GetFileClass );

	if ( GetFileBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening getfile gadget class" );
		goto bailout;
	}

	LabelBase = (PTR) OpenClass( "images/label.image", 53, & LabelClass );

	if ( LabelBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening label image class" );
		goto bailout;
	}

	// --

	LayoutBase = (PTR) OpenClass( "gadgets/layout.gadget", 53, & LayoutClass );

	if ( LayoutBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening layout gadget class" );
		goto bailout;
	}

	ILayout = (PTR) GetInterface( (PTR) LayoutBase, "main", 1, NULL );

	if ( ILayout == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error failed to obtain Layout Interface" );
		goto bailout;
	}

	PageClass = PAGE_GetClass();

	// --

	ListBrowserBase = (PTR) OpenClass( "gadgets/listbrowser.gadget", 53, & ListBrowserClass );

	if ( ListBrowserBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening listbrowser gadget class" );
		goto bailout;
	}

	IListBrowser = (PTR) GetInterface( (PTR) ListBrowserBase, "main", 1, NULL );

	if ( IListBrowser == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error failed to obtain ListBrowser Interface" );
		goto bailout;
	}

	// --

	IntegerBase = (PTR) OpenClass( "gadgets/integer.gadget", 53, & IntegerClass );

	if ( IntegerBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening integer gadget class" );
		goto bailout;
	}

	// --

	ScrollerBase = (PTR) OpenClass( "gadgets/scroller.gadget", 53, & ScrollerClass );

	if ( ScrollerBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening scroller gadget class" );
		goto bailout;
	}

	// --

	SliderBase = (PTR) OpenClass( "gadgets/slider.gadget", 53, & SliderClass );

	if ( SliderBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening lider gadget class" );
		goto bailout;
	}

	// --

	StringBase = (PTR) OpenClass( "gadgets/string.gadget", 53, & StringClass );

	if ( StringBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening string gadget class" );
		goto bailout;
	}

	// --

	WindowBase = (PTR) OpenClass( "window.class", 53, & WindowClass );

	if ( WindowBase == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening window class" );
		goto bailout;
	}

	// --

	ProgramIcon = GetDiskObjectNew( "rVNCd" );

	ActiveGUI.gui_SignalBits = WinMsgPort.vmp_SignalBit | WinAppPort.vmp_SignalBit ;

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --

#endif // GUI_RA
#endif // __HAVE__RVNCD__GUI__
