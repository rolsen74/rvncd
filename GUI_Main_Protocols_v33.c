
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static Object *Page_Protocols_v33( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
		End,

		#if 1
		LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,
//			LAYOUT_BevelStyle,			BVS_GROUP,
//			LAYOUT_Label,				"Server Support",

			LAYOUT_AddChild,			GUIObjects[ GID_Protocol_33_Enable ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,					GID_Protocol_33_Enable,
				GA_RelVerify,			TRUE,
				GA_Selected,			cfg->cfg_Disk_Settings.Protocol33,
			End,
			CHILD_Label,				IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,				"Enable",
			End,
			LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
			End,
		End,
		#endif
	
		LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

static void GUIFunc_Protocol_33_Enable( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Protocol_33_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.Protocol33 = ( val ) ? 1 : 0 ;

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void Page_Refresh_Protocol_33( struct Config *cfg )
{
	mySetTags( cfg, GUIObjects[ GID_Protocol_33_Enable ],
		GA_Selected, ( cfg->cfg_Disk_Settings.Protocol33 ) ? 1 : 0,
		TAG_END
	);
}

// --
