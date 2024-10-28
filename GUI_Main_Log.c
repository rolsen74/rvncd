
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static Object *Page_Log( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
		End,

		#if 1
		LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,

			LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,		BVS_GROUP,
				LAYOUT_Label,			"Log Events",

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Program_Start ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Program_Start,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogProgramStart ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Program Start",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Program_Stop ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Program_Stop,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogProgramStop ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Program Stop",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Server_Start ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Server_Start,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogServerStart ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Server Start",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Server_Stop ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Server_Stop,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogServerStart ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Server Stop",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_User_Connect ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_User_Connect,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogUserConnect ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"User Connect",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_User_Disconnect ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_User_Disconnect,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_LogUserDisconnect ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"User Disconnect",
				End,
			End,

			LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,		LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,		BVS_GROUP,
				LAYOUT_Label,			"Log Types",

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Infos ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Infos,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_ProgramLogInfos ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Infos",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Warnings ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Warnings,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_ProgramLogWarnings ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Warnings",
				End,

				LAYOUT_AddChild,		GUIObjects[ GID_Log_Errors ] = IIntuition->NewObject( CheckBoxClass, NULL,
					GA_ID,				GID_Log_Errors,
					GA_RelVerify,		TRUE,
					GA_Selected,		( cfg->cfg_ProgramLogErrors ) ? 1 : 0,
				End,
				CHILD_Label,			IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,			"Errors",
				End,

				LAYOUT_AddChild,		IIntuition->NewObject( LayoutClass, NULL,
				End,
			End,
		End,
		#endif

		LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

static void GUIFunc_UserConnect( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_User_Connect ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogUserConnect = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_UserDisconnect( struct Config *cfg )
{
uint32 val;

	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_User_Disconnect ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogUserDisconnect = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_Infos( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Infos ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramLogInfos = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_Warnings( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Warnings ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramLogWarnings = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_Errors( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Errors ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_ProgramLogErrors = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_ServerStart( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Server_Start ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogServerStart = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_ServerStop( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Server_Stop ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogServerStop = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_ProgramStart( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Program_Start ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogProgramStart = ( val ) ? 1 : 0 ;
}

// --

static void GUIFunc_ProgramStop( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Log_Program_Stop ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_LogProgramStop = ( val ) ? 1 : 0 ;
}

// --

static void Page_Refresh_Log( struct Config *cfg )
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
