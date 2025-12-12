
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

static const STR MouseType[] =
{
	"Built-in",
	"Custom",
	NULL
};

Object *Page_Mouse( struct Config *cfg UNUSED )
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
				LAYOUT_BevelStyle,				BVS_SBAR_VERT,
				LAYOUT_Label,					"Mouse Pointer",

				LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
					LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
					LAYOUT_AddChild,			GUIObjects[ GID_Mouse_Type ] = NewObject( ChooserClass, NULL,
						GA_ID,					GID_Mouse_Type,
						GA_RelVerify,			TRUE,
						CHOOSER_LabelArray,		MouseType,
						CHOOSER_Selected,		cfg->cfg_Disk_Settings.PointerType,
					End,
					CHILD_MinWidth,				150,
					CHILD_WeightedWidth,		0,
					LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
					End,
				End,
				CHILD_Label,					NewObject( LabelClass, NULL,
					LABEL_Text,					"Type",
				End,

				LAYOUT_AddChild,				GUIObjects[ GID_Mouse_Filename ] = NewObject( GetFileClass, NULL,
					GA_ID,						GID_Mouse_Filename,
					GA_Disabled,				cfg->cfg_Disk_Settings.PointerType == 0,
					GA_RelVerify,				TRUE,
					GETFILE_FullFile,			cfg->cfg_PointerFilename,
				End,
				CHILD_Label,					NewObject( LabelClass, NULL,
					LABEL_Text,					"Filename",
				End,
			End,
			CHILD_WeightedHeight,				0,
		End,
		#endif

		LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void GUIFunc_Mouse_Filename( struct Config *cfg UNUSED )
{
STR str;

	if ( gfRequestFile( GUIObjects[ GID_Mouse_Filename ], cfg->cfg_WinData[WIN_Main].WindowAdr ))
	{
		str = NULL;

		GetAttrs( GUIObjects[ GID_Mouse_Filename ],
			GETFILE_FullFile, & str,
			TAG_END
		);

		if ( cfg->cfg_PointerFilename )
		{
			mem_Free( cfg->cfg_PointerFilename );
		}

		cfg->cfg_PointerFilename = mem_Strdup( str );
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

void GUIFunc_Mouse_Type( struct Config *cfg )
{
U32 val;
	
	val = 0;

	GetAttrs( GUIObjects[ GID_Mouse_Type ],
		CHOOSER_Selected, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.PointerType = val;

	mySetTags( cfg, GUIObjects[ GID_Mouse_Filename ],
		GA_Disabled, val == 0,
		TAG_END
	);

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

void Page_Refresh_Mouse( struct Config *cfg )
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

#endif // GUI_RA
