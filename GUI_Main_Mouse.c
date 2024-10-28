
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static const char *MouseType[] =
{
	"Built-in",
	"Custom",
	NULL
};

static Object *Page_Mouse( struct Config *cfg UNUSED )
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
				LAYOUT_BevelStyle,				BVS_SBAR_VERT,
				LAYOUT_Label,					"Mouse Pointer",

				LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
					LAYOUT_AddChild,			GUIObjects[ GID_Mouse_Type ] = IIntuition->NewObject( ChooserClass, NULL,
						GA_ID,					GID_Mouse_Type,
						GA_RelVerify,			TRUE,
						CHOOSER_LabelArray,		MouseType,
						CHOOSER_Selected,		cfg->cfg_Disk_Settings.PointerType,
					End,
					CHILD_MinWidth,				150,
					CHILD_WeightedWidth,		0,
					LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
					End,
				End,
				CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,					"Type",
				End,

				LAYOUT_AddChild,				GUIObjects[ GID_Mouse_Filename ] = IIntuition->NewObject( GetFileClass, NULL,
					GA_ID,						GID_Mouse_Filename,
					GA_Disabled,				cfg->cfg_Disk_Settings.PointerType == 0,
					GA_RelVerify,				TRUE,
					GETFILE_FullFile,			cfg->cfg_PointerFilename,
				End,
				CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
					LABEL_Text,					"Filename",
				End,
			End,
			CHILD_WeightedHeight,				0,
		End,
		#endif

		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

static void GUIFunc_Mouse_Filename( struct Config *cfg UNUSED )
{
char *str;

	if ( gfRequestFile( GUIObjects[ GID_Mouse_Filename ], cfg->cfg_WinData[WIN_Main].WindowAdr ))
	{
		str = NULL;

		IIntuition->GetAttrs( GUIObjects[ GID_Mouse_Filename ],
			GETFILE_FullFile, & str,
			TAG_END
		);

		if ( cfg->cfg_PointerFilename )
		{
			myFree( cfg->cfg_PointerFilename );
		}

		cfg->cfg_PointerFilename = myStrdup( str );
	}

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void GUIFunc_Mouse_Type( struct Config *cfg )
{
uint32 val;
	
	val = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_Mouse_Type ],
		CHOOSER_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.PointerType = val;

	mySetTags( cfg, GUIObjects[ GID_Mouse_Filename ],
		GA_Disabled, val == 0,
		TAG_END
	);

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void Page_Refresh_Mouse( struct Config *cfg )
{
	mySetTags( cfg, GUIObjects[ GID_Mouse_Type ],
		CHOOSER_Selected, cfg->cfg_Disk_Settings.PointerType,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Mouse_Filename ],
		GETFILE_FullFile,	cfg->cfg_PointerFilename,
		GA_Disabled,		cfg->cfg_Disk_Settings.PointerType == 0,
		TAG_END
	);
}

// --
