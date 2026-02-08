
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

// --

Object *Page_Protocols_v37( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
		End,

		#if 1
		LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,
//			LAYOUT_BevelStyle,			BVS_GROUP,
//			LAYOUT_Label,				"Server Support",

			LAYOUT_AddChild,			GUIObjects[ GID_Protocol_37_Enable ] = NewObject( CheckBoxClass, NULL,
				GA_ID,					GID_Protocol_37_Enable,
				GA_RelVerify,			TRUE,
				GA_Selected,			cfg->cfg_Disk_Settings.Protocol37,
			End,
			CHILD_Label,				NewObject( LabelClass, NULL,
				LABEL_Text,				"Enable",
			End,
			LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
			End,
		End,
		#endif
	
		LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void GUIFunc_Protocol_37_Enable( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Protocol_37_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.Protocol37 = ( val ) ? 1 : 0 ;

	// --
	if ( ActiveGUI.Check_Settings )
	{
		#ifdef DEBUG
		ActiveGUI.Check_Settings( cfg, __FILE__, __LINE__ );
		#else
		ActiveGUI.Check_Settings( cfg );
		#endif
	}
}

// --

void Page_Refresh_Protocol_37( struct Config *cfg )
{
	mySetTags( cfg, GUIObjects[ GID_Protocol_37_Enable ],
		GA_Selected, ( cfg->cfg_Disk_Settings.Protocol37 ) ? 1 : 0,
		TAG_END
	);
}

// --

