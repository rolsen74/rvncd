
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

static const STR ActionPages[] =
{
	"Program",
	"Server",
	"User",
	NULL
};

Object *Page_Actions( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					NewObject( ClickTabClass, NULL,
			GA_Text,						ActionPages,
			CLICKTAB_Current,				0,
			CLICKTAB_PageGroup,				NewObject( PageClass, NULL,
				PAGE_Add,					Page_Action_Programs( cfg ),
				PAGE_Add,					Page_Action_Server( cfg ),
				PAGE_Add,					Page_Action_User( cfg ),
			End,
		End,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

#endif // GUI_RA
