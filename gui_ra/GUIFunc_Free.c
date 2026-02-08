
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

void GUIFunc_Free( struct Config *cfg UNUSED )
{
	ActiveGUI.gui_SignalBits = 0;

	if ( ProgramIcon )
	{
		FreeDiskObject( ProgramIcon );
		ProgramIcon = NULL;
	}

	// -- Close GUI

	if ( WindowBase )
	{
		CloseClass( (PTR) WindowBase );
		WindowClass = NULL;
		WindowBase = NULL;
	}

	if ( StringBase )
	{
		CloseClass( (PTR) StringBase );
		StringClass = NULL;
		StringBase = NULL;
	}

	if ( SliderBase )
	{
		CloseClass( (PTR) SliderBase );
		SliderClass = NULL;
		SliderBase = NULL;
	}

	if ( ScrollerBase )
	{
		CloseClass( (PTR) ScrollerBase );
		ScrollerClass = NULL;
		ScrollerBase = NULL;
	}

	if ( IntegerBase )
	{
		CloseClass( (PTR) IntegerBase );
		IntegerClass = NULL;
		IntegerBase = NULL;
	}

	if ( IListBrowser )
	{
		DropInterface( (PTR) IListBrowser );
		IListBrowser = NULL;
	}

	if ( ListBrowserBase )
	{
		CloseClass( (PTR) ListBrowserBase );
		ListBrowserClass = NULL;
		ListBrowserBase = NULL;
	}

	if ( ILayout )
	{
		DropInterface( (PTR) ILayout );
		ILayout = NULL;
	}

	if ( LayoutBase )
	{
		CloseClass( (PTR) LayoutBase );
		LayoutClass = NULL;
		LayoutBase = NULL;
	}

	if ( LabelBase )
	{
		CloseClass( (PTR) LabelBase );
		LabelClass = NULL;
		LabelBase = NULL;
	}

	if ( GetFileBase )
	{
		CloseClass( (PTR) GetFileBase );
		GetFileClass = NULL;
		GetFileBase = NULL;
	}

	if ( ClickTabBase )
	{
		CloseClass( (PTR) ClickTabBase );
		ClickTabClass = NULL;
		ClickTabBase = NULL;
	}

	if ( ChooserBase )
	{
		CloseClass( (PTR) ChooserBase );
		ChooserClass = NULL;
		ChooserBase = NULL;
	}

	if ( CheckBoxBase )
	{
		CloseClass( (PTR) CheckBoxBase );
		CheckBoxClass = NULL;
		CheckBoxBase = NULL;
	}

	if ( ButtonBase )
	{
		CloseClass( (PTR) ButtonBase );
		ButtonClass = NULL;
		ButtonBase = NULL;
	}

	if ( BitMapBase )
	{
		CloseClass( (PTR) BitMapBase );
		BitMapClass = NULL;
		BitMapBase = NULL;
	}

	// --

	MsgPort_Free( & WinAppPort );
	MsgPort_Free( & WinMsgPort );
}

// --
