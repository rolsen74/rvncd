
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

Object *GUI_Main_10_IPs_Page( struct Config *cfg UNUSED )
{
Object *o;

	o = MUI_NewObject( MUIC_Group,

		MUIA_Group_Child,		MUI_NewObject( MUIC_Group,
		End,

		MUIA_Group_Child,	SimpleButton( "IPs List" ),

		MUIA_Group_Child,		MUI_NewObject( MUIC_Group,
		End,

	End;

	return( o );
}

// --
