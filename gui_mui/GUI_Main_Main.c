
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#ifdef GUI_MUI

// --

static STR SystemStr =
	"( "

	"System: "
	#ifdef _AOS3_
	"AOS3"
	#endif // _ASO3_
	#ifdef _AOS4_
	"AOS4"
	#endif // _ASO4_

	"  Graphics: "
	#ifdef HAVE_CGFX
	"CGfx"
	#endif // HAVE_CGFX
	#ifdef HAVE_P96
	"P96"
	#endif // HAVE_P96
	#ifdef HAVE_GFX54
	"Gfx54"
	#endif // HAVE_GFX

	" )";

static STR OptionStr = "( "
	#ifdef HAVE_AREXX
	"ARexx "
	#endif // HAVE_AREXX
	#ifdef HAVE_CXBROKER
	"CxBroker "
	#endif // HAVE_CXBROKER
	#ifdef HAVE_ZLIB
	"ZLib "
	#endif // HAVE_ZLIB
	")";

// --

Object *GUI_Main_Main_Page( struct Config *cfg UNUSED )
{
Object *o;

	o = MUI_NewObject( MUIC_Group,

		MUIA_Group_Child,		MUI_NewObject( MUIC_Group,
		End,

		MUIA_Group_Child,		MUI_NewObject( MUIC_Text,
			MUIA_Text_PreParse, "\33c",
			MUIA_Text_Contents, & VERSTAG[7],
		End,

		MUIA_Group_Child,		MUI_NewObject( MUIC_Text,
			MUIA_Text_PreParse, "\33c",
			MUIA_Text_Contents, SystemStr,
		End,

		MUIA_Group_Child,		MUI_NewObject( MUIC_Text,
			MUIA_Text_PreParse, "\33c",
			MUIA_Text_Contents, OptionStr,
		End,

		MUIA_Group_Child,		MUI_NewObject( MUIC_Group,
		End,

	End;

	return( o );
}

// --

#endif // GUI_MUI
