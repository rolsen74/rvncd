
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#ifdef GUI_MUI

// --

Object *GUI_Main_Log_Page( struct Config *cfg UNUSED )
{
Object *o;

	o = MUI_NewObject( MUIC_Group,

		MUIA_Group_Child,		MUI_NewObject( MUIC_Group,
		End,

		MUIA_Group_Child,	SimpleButton( "Log" ),

		MUIA_Group_Child,		MUI_NewObject( MUIC_Group,
		End,
	End;

	return( o );
}

// --

#endif // GUI_MUI
