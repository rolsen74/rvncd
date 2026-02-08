
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

Object *GUI_Main_02_Program_Page( struct Config *cfg UNUSED )
{
Object *o;

	o = MUI_NewObject( MUIC_Group,

		MUIA_Group_Child,		MUI_NewObject( MUIC_Rectangle,
		End,

		MUIA_Group_Child,	SimpleButton( "Program" ),

		MUIA_Group_Child,		MUI_NewObject( MUIC_Rectangle,
		End,
	End;

	return( o );
}

// --
