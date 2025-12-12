
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

Object *Page_Server( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
		End,

		#if 1
		LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

			LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,				BVS_GROUP,
				LAYOUT_Label,					"Server",

				LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
					LAYOUT_AddChild,			GUIObjects[ GID_Port ] = NewObject( IntegerClass, NULL,
						GA_ID,					GID_Port,
						GA_RelVerify,			TRUE,
						GA_TabCycle,			TRUE,
						INTEGER_Minimum,		0,
						INTEGER_Maximum,		65535,
						INTEGER_Number,			cfg->cfg_Disk_Settings.Port,
					End,
					CHILD_WeightedWidth,		80,
					LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
					End,
					CHILD_WeightedWidth,		220,
				End,
				CHILD_Label,					NewObject( LabelClass, NULL,
					LABEL_Text,					"Port",
				End,

				LAYOUT_AddChild,				GUIObjects[ GID_ServerName ] = NewObject( StringClass, NULL,
					GA_ID,						GID_ServerName,
					GA_RelVerify,				TRUE,
					GA_TabCycle,				TRUE,
					STRINGA_WorkBuffer,			WorkBuffer,
					STRINGA_MaxChars,			MAX_SERVERNAME + 1,
					STRINGA_TextVal,			cfg->cfg_Disk_Settings.Name,
				End,
				CHILD_Label,					NewObject( LabelClass, NULL,
					LABEL_Text,					"Name",
				End,

				LAYOUT_AddChild,				GUIObjects[ GID_Password ] = NewObject( StringClass, NULL,
					GA_ID,						GID_Password,
					GA_RelVerify,				TRUE,
					GA_TabCycle,				TRUE,
					STRINGA_WorkBuffer,			WorkBuffer,
					STRINGA_MaxChars,			MAX_SERVERPASSWORD + 1,
					STRINGA_TextVal,			cfg->cfg_Disk_Settings.Password,
				End,
				CHILD_Label,					NewObject( LabelClass, NULL,
					LABEL_Text,					"Password",
				End,
			End,

			LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
				LAYOUT_BevelStyle,				BVS_GROUP,

				LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
					LAYOUT_AddChild,			GUIObjects[ GID_TileBufferKB ] = NewObject( IntegerClass, NULL,
						GA_ID,					GID_TileBufferKB,
						GA_RelVerify,			TRUE,
						GA_TabCycle,			TRUE,
						INTEGER_Minimum,		0,
						INTEGER_Maximum,		1024,
						INTEGER_Number,			cfg->cfg_Disk_Settings.TileBufferKB,
					End,
					CHILD_WeightedWidth,		80,
					LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
					End,
					CHILD_WeightedWidth,		220,
				End,
				CHILD_Label,					NewObject( LabelClass, NULL,
					LABEL_Text,					"Send Tile Buffer",
				End,
			End,


			LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,				LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,			BVS_GROUP,

					LAYOUT_AddChild,			GUIObjects[ GID_Disable_Blanker ] = NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Disable_Blanker,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.DisableBlanker ) ? 1 : 0,
					End,
					CHILD_Label,				NewObject( LabelClass, NULL,
						LABEL_Text,				"Disable Blanker",
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_Send_Clipboard ] = NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Send_Clipboard,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.SendClipboard ) ? 1 : 0,
					End,
					CHILD_Label,				NewObject( LabelClass, NULL,
						LABEL_Text,				"Send Clipboard",
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_Read_Clipboard ] = NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Read_Clipboard,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.ReadClipboard ) ? 1 : 0,
					End,
					CHILD_Label,				NewObject( LabelClass, NULL,
						LABEL_Text,				"Read Clipboard",
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_Send_Bell ] = NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Send_Bell,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.SendBell ) ? 1 : 0,
					End,
					CHILD_Label,				NewObject( LabelClass, NULL,
						LABEL_Text,				"Send Bell",
					End,

					LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
					End,
				End,

				LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,
					LAYOUT_BevelStyle,			BVS_GROUP,
					LAYOUT_Label,				"Server Support",

					LAYOUT_AddChild,			GUIObjects[ GID_Support_RRE ] = NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Support_RRE,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.RRE ) ? 1 : 0,
					End,
					CHILD_Label,				NewObject( LabelClass, NULL,
						LABEL_Text,				"RRE",
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_Support_ZLib ] = NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Support_ZLib,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.ZLib ) ? 1 : 0,
					End,
					CHILD_Label,				NewObject( LabelClass, NULL,
						LABEL_Text,				"ZLib",
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_Support_RichCursor ] = NewObject( CheckBoxClass, NULL,
						GA_ID,					GID_Support_RichCursor,
						GA_RelVerify,			TRUE,
						GA_Selected,			( cfg->cfg_Disk_Settings.RichCursor ) ? 1 : 0,
					End,
					CHILD_Label,				NewObject( LabelClass, NULL,
						LABEL_Text,				"Rich Cursor",
					End,

					LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
					End,
				End,
			End,
		End,
		#endif

		LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void GUIFunc_Support_RichCursor( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Support_RichCursor ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.RichCursor = ( val ) ? 1 : 0 ;

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

void GUIFunc_Support_RRE( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Support_RRE ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.RRE = ( val ) ? 1 : 0 ;

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

void GUIFunc_Support_ZLib( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Support_ZLib ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.ZLib = ( val ) ? 1 : 0 ;

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

void GUIFunc_Disable_Blanker( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Disable_Blanker ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.DisableBlanker = ( val ) ? 1 : 0 ;

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

void GUIFunc_Send_Clipboard( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Send_Clipboard ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.SendClipboard = ( val ) ? 1 : 0 ;

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

void GUIFunc_Read_Clipboard( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Read_Clipboard ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.ReadClipboard = ( val ) ? 1 : 0 ;

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

void GUIFunc_Send_Bell( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Send_Bell ],
		GA_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.SendBell = ( val ) ? 1 : 0 ;

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

void GUIFunc_Port( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Port ],
		INTEGER_Number, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.Port = val;

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

void GUIFunc_UpdatePassword( struct Config *cfg )
{
STR str;

	str = NULL;

	GetAttrs( GUIObjects[ GID_Password ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( cfg->cfg_Disk_Settings.Password, str, MAX_SERVERPASSWORD );
		cfg->cfg_Disk_Settings.Password[MAX_SERVERPASSWORD] = 0;
	}

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

void GUIFunc_UpdateServerName( struct Config *cfg )
{
STR str;

	str = NULL;

	GetAttrs( GUIObjects[ GID_ServerName ],
		STRINGA_TextVal, & str,
		TAG_END
	);

	if ( str )
	{
		strncpy( cfg->cfg_Disk_Settings.Name, str, MAX_SERVERNAME );		
	}

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

void GUIFunc_TileBufferKB( struct Config *cfg )
{
U32 val;

	val = 0;

	GetAttrs( GUIObjects[ GID_TileBufferKB ],
		INTEGER_Number, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.TileBufferKB = val;

//	SHELLBUF_PRINTF( "GUIFunc_TileBufferKB : Val %lu\n", val );

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

#endif // GUI_RA
