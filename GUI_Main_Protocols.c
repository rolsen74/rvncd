
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static const char *ServerPages[] =
{
	"Protocol v3.3",
	"Protocol v3.7",
	NULL
};

static Object *Page_Protocols( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,				IIntuition->NewObject( ClickTabClass, NULL,
			GA_Text,					ServerPages,
			CLICKTAB_Current,			0,
			CLICKTAB_PageGroup,			IIntuition->NewObject( PageClass, NULL,
				PAGE_Add,				Page_Protocols_v33( cfg ),
				PAGE_Add,				Page_Protocols_v37( cfg ),
			End,
		End,

		LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --
