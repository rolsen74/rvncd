
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static const char *ActionPages[] =
{
	"Program",
	"Server",
	"User",
	NULL
};

static Object *Page_Actions( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( ClickTabClass, NULL,
			GA_Text,						ActionPages,
			CLICKTAB_Current,				0,
			CLICKTAB_PageGroup,				IIntuition->NewObject( PageClass, NULL,
				PAGE_Add,					Page_Action_Programs( cfg ),
				PAGE_Add,					Page_Action_Server( cfg ),
				PAGE_Add,					Page_Action_User( cfg ),
			End,
		End,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --
