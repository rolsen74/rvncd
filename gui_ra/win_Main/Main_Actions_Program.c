
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

// --

Object *Page_Action_Programs( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,

		#if 1
		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Program Start",
			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Start_Enable ] = NewObject( CheckBoxClass, NULL,
					GA_ID,					GID_Actions_Program_Start_Enable,
					GA_Selected,			( cfg->cfg_ActionsProgramStartEnable ) ? 1 : 0,
					GA_RelVerify,			TRUE,
				End,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Start_Type ] = NewObject( ChooserClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsProgramStartEnable == 0,
					CHOOSER_LabelArray,		ActionTypes,
					CHOOSER_Selected,		0,
//					GA_RelVerify,			TRUE,
//					GA_Selected,			( DoVerbose ) ? 1 : 0,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
			End,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Start_String ] = NewObject( StringClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsProgramStartEnable == 0,
					GA_ID,					GID_Actions_Program_Start_String,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					STRINGA_WorkBuffer,		WorkBuffer,
					STRINGA_MaxChars,		MAX_ACTIONBUFFER,
					STRINGA_TextVal,		ActionBuffer_ProgramStart,
				End,
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Program Stop",
			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Stop_Enable ] = NewObject( CheckBoxClass, NULL,
					GA_ID,					GID_Actions_Program_Stop_Enable,
					GA_Selected,			( cfg->cfg_ActionsProgramStopEnable ) ? 1 : 0,
					GA_RelVerify,			TRUE,
				End,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Stop_Type ] = NewObject( ChooserClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsProgramStopEnable == 0,
					CHOOSER_LabelArray,		ActionTypes,
					CHOOSER_Selected,		0,
//					GA_RelVerify,			TRUE,
//					GA_Selected,			( DoVerbose ) ? 1 : 0,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
			End,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_Program_Stop_String ] = NewObject( StringClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsProgramStopEnable == 0,
					GA_ID,					GID_Actions_Program_Stop_String,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					STRINGA_WorkBuffer,		WorkBuffer,
					STRINGA_MaxChars,		MAX_ACTIONBUFFER,
					STRINGA_TextVal,		ActionBuffer_ProgramStop,
				End,
			End,
		End,
		CHILD_WeightedHeight,				0,
		#endif

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void GUIFunc_Actions_Program_Start_Enable( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Actions_Program_Start_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ActionsProgramStartEnable = ( val ) ? 1 : 0 ;

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Start_Type ],
		GA_Disabled, val == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Start_String ],
		GA_Disabled, val == 0,
		TAG_END
	);
}

// --

void GUIFunc_Actions_Program_Stop_Enable( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Actions_Program_Stop_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ActionsProgramStopEnable = ( val ) ? 1 : 0 ;

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Stop_Type ],
		GA_Disabled, val == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Stop_String ],
		GA_Disabled, val == 0,
		TAG_END
	);
}

// --

void GUIFunc_UpdateActionProgramStart( struct Config *cfg UNUSED )
{
STR str;

	str = NULL;

	GetAttrs( GUIObjects[ GID_Actions_Program_Start_String ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( ActionBuffer_ProgramStart, str, MAX_ACTIONBUFFER-1 );
	}
}

// --

void GUIFunc_UpdateActionProgramStop( struct Config *cfg UNUSED )
{
STR str;

	str = NULL;

	GetAttrs( GUIObjects[ GID_Actions_Program_Stop_String ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( ActionBuffer_ProgramStop, str, MAX_ACTIONBUFFER-1 );
	}
}

// --

void Page_Refresh_Action_Program( struct Config *cfg )
{
	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Start_Enable ],
		GA_Selected, ( cfg->cfg_ActionsProgramStartEnable ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Start_Type ],
		GA_Disabled, cfg->cfg_ActionsProgramStartEnable == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Start_String ],
		GA_Disabled, cfg->cfg_ActionsProgramStartEnable == 0,
		STRINGA_TextVal, ActionBuffer_ProgramStart,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Stop_Enable ],
		GA_Selected, ( cfg->cfg_ActionsProgramStopEnable ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Stop_Type ],
		GA_Disabled, cfg->cfg_ActionsProgramStopEnable == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_Program_Stop_String ],
		GA_Disabled, cfg->cfg_ActionsProgramStopEnable == 0,
		STRINGA_TextVal, ActionBuffer_ProgramStop,
		TAG_END
	);
}

// --
