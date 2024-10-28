
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static Object *Page_Program( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,		BVS_GROUP,

			LAYOUT_AddChild,		GUIObjects[ GID_Program_Verbose ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,				GID_Program_Verbose,
				GA_RelVerify,		TRUE,
				GA_Selected,		( DoVerbose ) ? 1 : 0,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Verbose",
			End,
		End,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,		BVS_GROUP,
			LAYOUT_Label,			"Program Startup",

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Program_WinStat ] = IIntuition->NewObject( ChooserClass, NULL,
					GA_ID,					GID_Program_WinStat,
					GA_RelVerify,			TRUE,
					CHOOSER_LabelArray,		WinStatStrings,
					CHOOSER_Selected,		cfg->MainWinState,
				End,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Main Win Stat",
			End,

			LAYOUT_AddChild,		GUIObjects[ GID_Program_AutoStart ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,				GID_Program_AutoStart,
				GA_RelVerify,		TRUE,
				GA_Selected,		( cfg->AutoStart ) ? 1 : 0,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Server Autostart",
			End,

			LAYOUT_AddChild,		GUIObjects[ GID_Program_Disable_ARexx ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,				GID_Program_Disable_ARexx,
				GA_RelVerify,		TRUE,
				GA_Selected,		( cfg->cfg_ProgramDisableARexx ) ? 1 : 0,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Disable ARexx",
			End,

			LAYOUT_AddChild,		GUIObjects[ GID_Program_Disable_CxBroker ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,				GID_Program_Disable_CxBroker,
				GA_RelVerify,		TRUE,
				GA_Selected,		( cfg->cfg_ProgramDisableCxBroker ) ? 1 : 0,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Disable CxBroker",
			End,

			LAYOUT_AddChild,		GUIObjects[ GID_Program_Disable_GUI ] = IIntuition->NewObject( CheckBoxClass, NULL,
				GA_ID,				GID_Program_Disable_GUI,
				GA_RelVerify,		TRUE,
				GA_Selected,		( cfg->cfg_ProgramDisableGUI ) ? 1 : 0,
			End,
			CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,			"Disable GUI",
			End,
		End,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

static void GUIFunc_AutoStart( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_AutoStart ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->AutoStart = ( val ) ? 1 : 0 ;

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Verbose( struct Config *cfg UNUSED )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_Verbose ],
		GA_Selected, & val,
		TAG_END
	);

	DoVerbose = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_ARexx( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_Disable_ARexx ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramDisableARexx = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_Broker( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_Disable_CxBroker ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramDisableCxBroker = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_GUI( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_Disable_GUI ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramDisableGUI = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_ProgramWinStat( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Program_WinStat ],
		CHOOSER_Selected, & val,
		TAG_END
	);

	cfg->MainWinState = val;
}

// --
