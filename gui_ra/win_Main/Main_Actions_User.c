
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

Object *Page_Action_User( struct Config *cfg UNUSED )
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
			LAYOUT_Label,					"User Connect",
			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Connect_Enable ] = NewObject( CheckBoxClass, NULL,
					GA_ID,					GID_Actions_User_Connect_Enable,
					GA_Selected,			( cfg->cfg_ActionsUserConnectEnable ) ? 1 : 0,
					GA_RelVerify,			TRUE,
				End,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Connect_Type ] = NewObject( ChooserClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsUserConnectEnable == 0,
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
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Connect_String ] = NewObject( StringClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsUserConnectEnable == 0,
					GA_ID,					GID_Actions_User_Connect_String,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					STRINGA_WorkBuffer,		WorkBuffer,
					STRINGA_MaxChars,		MAX_ACTIONBUFFER,
					STRINGA_TextVal,		ActionBuffer_UserConnect,
				End,
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"User Disconnect",
			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Disconnect_Enable ] = NewObject( CheckBoxClass, NULL,
					GA_ID,					GID_Actions_User_Disconnect_Enable,
					GA_Selected,			( cfg->cfg_ActionsUserDisconnectEnable ) ? 1 : 0,
					GA_RelVerify,			TRUE,
				End,
				CHILD_WeightedWidth,		0,
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Disconnect_Type ] = NewObject( ChooserClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsUserDisconnectEnable == 0,
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
				LAYOUT_AddChild,			GUIObjects[ GID_Actions_User_Disconnect_String ] = NewObject( StringClass, NULL,
					GA_Disabled,			cfg->cfg_ActionsUserDisconnectEnable == 0,
					GA_ID,					GID_Actions_User_Disconnect_String, 
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					STRINGA_WorkBuffer,		WorkBuffer,
					STRINGA_MaxChars,		MAX_ACTIONBUFFER,
					STRINGA_TextVal,		ActionBuffer_UserDisconnect,
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

void GUIFunc_Actions_User_Connect_Enable( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Actions_User_Connect_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ActionsUserConnectEnable = ( val ) ? 1 : 0 ;

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Connect_Type ],
		GA_Disabled, val == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Connect_String ],
		GA_Disabled, val == 0,
		TAG_END
	);
}

// --

void GUIFunc_Actions_User_Disconnect_Enable( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Actions_User_Disconnect_Enable ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ActionsUserDisconnectEnable = ( val ) ? 1 : 0 ;

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Disconnect_Type ],
		GA_Disabled, val == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Disconnect_String ],
		GA_Disabled, val == 0,
		TAG_END
	);
}

// --

void GUIFunc_UpdateActionUserConnect( struct Config *cfg UNUSED )
{
STR str;

	str = NULL;

	GetAttrs( GUIObjects[ GID_Actions_User_Connect_String ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( ActionBuffer_UserConnect, str, MAX_ACTIONBUFFER-1 );
	}
}

// --

void GUIFunc_UpdateActionUserDisconnect( struct Config *cfg UNUSED )
{
STR str;

	str = NULL;

	GetAttrs( GUIObjects[ GID_Actions_User_Disconnect_String ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( ActionBuffer_UserDisconnect, str, MAX_ACTIONBUFFER-1 );
	}
}

// --

void Page_Refresh_Action_User( struct Config *cfg )
{
	mySetTags( cfg, GUIObjects[ GID_Actions_User_Connect_Enable ],
		GA_Selected, ( cfg->cfg_ActionsUserConnectEnable ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Connect_Type ],
		GA_Disabled, cfg->cfg_ActionsUserConnectEnable == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Connect_String ],
		GA_Disabled, cfg->cfg_ActionsUserConnectEnable == 0,
		STRINGA_TextVal, ActionBuffer_UserConnect,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Disconnect_Enable ],
		GA_Selected, ( cfg->cfg_ActionsUserDisconnectEnable ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Disconnect_Type ],
		GA_Disabled, cfg->cfg_ActionsUserDisconnectEnable == 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Actions_User_Disconnect_String ],
		GA_Disabled, cfg->cfg_ActionsUserDisconnectEnable == 0,
		STRINGA_TextVal, ActionBuffer_UserDisconnect,
		TAG_END
	);
}

// --

#endif // GUI_RA
