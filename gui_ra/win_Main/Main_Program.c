
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

// --

Object *Page_Program( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_GROUP,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Program_Verbose ] = NewObject( ChooserClass, NULL,
					GA_ID,					GID_Program_Verbose,
					GA_RelVerify,			TRUE,
					CHOOSER_LabelArray,		WinVerboseStrings,
					CHOOSER_Selected,		DoVerbose,
				End,
				CHILD_Label,				NewObject( LabelClass, NULL,
					LABEL_Text,				"Verbose",
				End,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
			End,
		End,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_GROUP,
			LAYOUT_Label,					"Program Startup",

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Program_WinStat ] = NewObject( ChooserClass, NULL,
					GA_ID,					GID_Program_WinStat,
					GA_RelVerify,			TRUE,
					CHOOSER_LabelArray,		WinStatStrings,
					CHOOSER_Selected,		cfg->MainWinState,
				End,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Main Win Stat",
			End,

			LAYOUT_AddChild,				GUIObjects[ GID_Program_AutoStart ] = NewObject( CheckBoxClass, NULL,
				GA_ID,						GID_Program_AutoStart,
				GA_RelVerify,				TRUE,
				GA_Selected,				( cfg->AutoStart ) ? 1 : 0,
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Server Autostart",
			End,

			LAYOUT_AddChild,				GUIObjects[ GID_Program_Disable_ARexx ] = NewObject( CheckBoxClass, NULL,
				GA_ID,						GID_Program_Disable_ARexx,
				GA_RelVerify,				TRUE,
				GA_Selected,				( cfg->cfg_Program_DisableARexx ) ? 1 : 0,
				#ifndef HAVE_AREXX
				GA_Disabled,				TRUE,
				#endif
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Disable ARexx",
			End,

			LAYOUT_AddChild,				GUIObjects[ GID_Program_Disable_CxBroker ] = NewObject( CheckBoxClass, NULL,
				GA_ID,						GID_Program_Disable_CxBroker,
				GA_RelVerify,				TRUE,
				GA_Selected,				( cfg->cfg_Program_DisableCxBroker ) ? 1 : 0,
				#ifndef HAVE_CXBROKER
				GA_Disabled,				TRUE,
				#endif
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Disable CxBroker",
			End,

			LAYOUT_AddChild,				GUIObjects[ GID_Program_Disable_GUI ] = NewObject( CheckBoxClass, NULL,
				GA_ID,						GID_Program_Disable_GUI,
				GA_RelVerify,				TRUE,
				GA_Selected,				( cfg->cfg_Program_DisableGUI ) ? 1 : 0,
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Disable GUI",
			End,
		End,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void GUIFunc_AutoStart( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Program_AutoStart ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->AutoStart = ( val ) ? 1 : 0 ;

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

void GUIFunc_Verbose( struct Config *cfg UNUSED )
{
U32 val;
	
	val = 0;

	#if 0

	GetAttrs( GUIObjects[ GID_Program_Verbose ],
		GA_Selected, & val,
		TAG_END
	);

	DoVerbose = ( val ) ? 1 : 0 ;

	#else

	GetAttrs( GUIObjects[ GID_Program_Verbose ],
		CHOOSER_Selected, & val,
		TAG_END
	);

	if (( DoVerbose >= 0 ) && ( DoVerbose <= 3 ))
	{
		DoVerbose = val;
	}

	#endif
}

// --

void GUIFunc_ARexx( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Program_Disable_ARexx ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Program_DisableARexx = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_Broker( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Program_Disable_CxBroker ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Program_DisableCxBroker = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_GUI( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Program_Disable_GUI ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Program_DisableGUI = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_ProgramWinStat( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Program_WinStat ],
		CHOOSER_Selected, & val,
		TAG_END
	);

	cfg->MainWinState = val;
}

// --
