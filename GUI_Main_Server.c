
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static Object *Page_Server( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
		End,

		#if 1
		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,				BVS_GROUP,
				LAYOUT_Label,					"Server",

				LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
					LAYOUT_AddChild,			GUIObjects[ GID_Port ] = IIntuition->NewObject( IntegerClass, NULL,
						GA_ID,					GID_Port,
						GA_RelVerify,			TRUE,
						GA_TabCycle,			TRUE,
						INTEGER_Minimum,		0,
						INTEGER_Maximum,		65535,
						INTEGER_Number,			cfg->cfg_Disk_Settings.Port,
					End,
					CHILD_WeightedWidth,		80,
					LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
					End,
					CHILD_WeightedWidth,		220,
				End,
				CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,					"Port",
				End,

				LAYOUT_AddChild,				GUIObjects[ GID_ServerName ] = IIntuition->NewObject( StringClass, NULL,
					GA_ID,						GID_ServerName,
					GA_RelVerify,				TRUE,
					GA_TabCycle,				TRUE,
					STRINGA_WorkBuffer,			WorkBuffer,
					STRINGA_MaxChars,			MAX_SERVERNAME + 1,
					STRINGA_TextVal,			cfg->cfg_Disk_Settings.Name,
				End,
				CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,					"Name",
				End,

				LAYOUT_AddChild,				GUIObjects[ GID_Password ] = IIntuition->NewObject( StringClass, NULL,
					GA_ID,						GID_Password,
					GA_RelVerify,				TRUE,
					GA_TabCycle,				TRUE,
					STRINGA_WorkBuffer,			WorkBuffer,
					STRINGA_MaxChars,			MAX_SERVERPASSWORD + 1,
					STRINGA_TextVal,			cfg->cfg_Disk_Settings.Password,
				End,
				CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,					"Password",
				End,
			End,

			LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,				LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,			BVS_GROUP,

					LAYOUT_AddChild,			GUIObjects[ GID_Disable_Blanker ] = IIntuition->NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Disable_Blanker,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.DisableBlanker ) ? 1 : 0,
					End,
					CHILD_Label,				IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,				"Disable Blanker",
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_Send_Clipboard ] = IIntuition->NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Send_Clipboard,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.SendClipboard ) ? 1 : 0,
					End,
					CHILD_Label,				IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,				"Send Clipboard",
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_Read_Clipboard ] = IIntuition->NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Read_Clipboard,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.ReadClipboard ) ? 1 : 0,
					End,
					CHILD_Label,				IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,				"Read Clipboard",
					End,

					LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
					End,
				End,

				LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,			BVS_GROUP,
					LAYOUT_Label,				"Server Support",

					LAYOUT_AddChild,			GUIObjects[ GID_Support_ZLib ] = IIntuition->NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Support_ZLib,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.ZLib ) ? 1 : 0,
					End,
					CHILD_Label,				IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,				"ZLib",
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_Support_RichCursor ] = IIntuition->NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Support_RichCursor,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.RichCursor ) ? 1 : 0,
					End,
					CHILD_Label,				IIntuition->NewObject( LabelClass, NULL,
						LABEL_Text,				"Rich Cursor",
					End,

					LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
					End,
				End,
			End,
		End,
		#endif

		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

static void GUIFunc_Support_RichCursor( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Support_RichCursor ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.RichCursor = ( val ) ? 1 : 0 ;

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Support_ZLib( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Support_ZLib ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.ZLib = ( val ) ? 1 : 0 ;

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Disable_Blanker( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Disable_Blanker ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.DisableBlanker = ( val ) ? 1 : 0 ;

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Send_Clipboard( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Send_Clipboard ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.SendClipboard = ( val ) ? 1 : 0 ;

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Read_Clipboard( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Read_Clipboard ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.ReadClipboard = ( val ) ? 1 : 0 ;

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Port( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Port ],
		INTEGER_Number, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.Port = val;

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void GUIFunc_UpdatePassword( struct Config *cfg )
{
char *str;

	str = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_Password ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( cfg->cfg_Disk_Settings.Password, str, MAX_SERVERPASSWORD );		
	}

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void GUIFunc_UpdateServerName( struct Config *cfg )
{
char *str;

	str = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_ServerName ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( cfg->cfg_Disk_Settings.Name, str, MAX_SERVERNAME );		
	}

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --
