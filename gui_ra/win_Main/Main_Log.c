
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

Object *Page_Log( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,

			LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,		BVS_GROUP,
				LAYOUT_Label,			"Log Events",

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Program_Start ] = NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Program_Start,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogProgramStart ) ? 1 : 0,
				End,
				CHILD_Label,			NewObject( LabelClass, NULL,
					LABEL_Text,			"Program Start",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Program_Stop ] = NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Program_Stop,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogProgramStop ) ? 1 : 0,
				End,
				CHILD_Label,			NewObject( LabelClass, NULL,
					LABEL_Text,			"Program Stop",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Server_Start ] = NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Server_Start,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogServerStart ) ? 1 : 0,
				End,
				CHILD_Label,			NewObject( LabelClass, NULL,
					LABEL_Text,			"Server Start",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Server_Stop ] = NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Server_Stop,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogServerStart ) ? 1 : 0,
				End,
				CHILD_Label,			NewObject( LabelClass, NULL,
					LABEL_Text,			"Server Stop",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_User_Connect ] = NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_User_Connect,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogUserConnect ) ? 1 : 0,
				End,
				CHILD_Label,			NewObject( LabelClass, NULL,
					LABEL_Text,			"User Connect",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_User_Disconnect ] = NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_User_Disconnect,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogUserDisconnect ) ? 1 : 0,
				End,
				CHILD_Label,			NewObject( LabelClass, NULL,
					LABEL_Text,			"User Disconnect",
				End,
			End,

			LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,		BVS_GROUP,
				LAYOUT_Label,			"Log Types",

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Infos ] = NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Infos,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_ProgramLogInfos ) ? 1 : 0,
				End,
				CHILD_Label,			NewObject( LabelClass, NULL,
					LABEL_Text,			"Infos",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Warnings ] = NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Warnings,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_ProgramLogWarnings ) ? 1 : 0,
				End,
				CHILD_Label,			NewObject( LabelClass, NULL,
					LABEL_Text,			"Warnings",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Errors ] = NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Errors,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_ProgramLogErrors ) ? 1 : 0,
				End,
				CHILD_Label,			NewObject( LabelClass, NULL,
					LABEL_Text,			"Errors",
				End,

				LAYOUT_AddChild,		NewObject( LayoutClass, NULL,
				End,
			End,
		End,

		LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void GUIFunc_UserConnect( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Log_User_Connect ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogUserConnect = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_UserDisconnect( struct Config *cfg )
{
U32 val;

	val = 0;

	GetAttrs( GUIObjects[ GID_Log_User_Disconnect ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogUserDisconnect = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_Infos( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Log_Infos ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramLogInfos = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_Warnings( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Log_Warnings ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramLogWarnings = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_Errors( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Log_Errors ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramLogErrors = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_ServerStart( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Log_Server_Start ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogServerStart = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_ServerStop( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Log_Server_Stop ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogServerStop = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_ProgramStart( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Log_Program_Start ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogProgramStart = ( val ) ? 1 : 0 ;
}

// --

void GUIFunc_ProgramStop( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Log_Program_Stop ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogProgramStop = ( val ) ? 1 : 0 ;
}

// --

void Page_Refresh_Log( struct Config *cfg )
{
	mySetTags( cfg, GUIObjects[ GID_Log_Program_Start ],
		GA_Selected, ( cfg->cfg_LogProgramStart ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Program_Stop ],
		GA_Selected, ( cfg->cfg_LogProgramStop ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Server_Start ],
		GA_Selected, ( cfg->cfg_LogServerStart ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Server_Stop ],
		GA_Selected, ( cfg->cfg_LogServerStop ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_User_Connect ],
		GA_Selected, ( cfg->cfg_LogUserConnect ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Infos ],
		GA_Selected, ( cfg->cfg_ProgramLogInfos ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Warnings ],
		GA_Selected, ( cfg->cfg_ProgramLogWarnings ) ? 1 : 0,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Log_Errors ],
		GA_Selected, ( cfg->cfg_ProgramLogErrors ) ? 1 : 0,
		TAG_END
	);
}

// --

#endif // GUI_RA
