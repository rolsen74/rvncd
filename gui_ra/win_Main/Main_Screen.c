
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

Object *Page_Screen( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,

		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Screen Grabber",

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Screen_TileSize ] = NewObject( IntegerClass, NULL,
					GA_ID,					GID_Screen_TileSize,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					INTEGER_Minimum,		16,
					INTEGER_Maximum,		256,
					INTEGER_Number,			cfg->cfg_Disk_Settings.TileSize,
				End,
				CHILD_WeightedWidth,		80,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		220,
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Tile Size",
			End,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,
				LAYOUT_AddChild,			GUIObjects[ GID_Screen_DelaySlider ] = NewObject( SliderClass, NULL,
					GA_ID,					GID_Screen_DelaySlider,
					GA_RelVerify,			TRUE,
					GA_TabCycle, 			TRUE,
					SLIDER_Orientation,		SORIENT_HORIZ,
					SLIDER_Ticks,			11,
					SLIDER_ShortTicks,		TRUE,
					SLIDER_Max,				500,
					SLIDER_LevelPlace,		PLACETEXT_IN,
					SLIDER_LevelFormat,		"%ld",
					SLIDER_Level,			cfg->GfxRead_DelayFrequency,
				End,
			End,
			CHILD_WeightedHeight,			0,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Delay Frequency",
			End,
		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void GUIFunc_Screen_Delay( struct Config *cfg )
{
U32 val;

	GetAttrs( GUIObjects[ GID_Screen_DelaySlider ],
		SLIDER_Level, & val,
		TAG_END
	);

	cfg->GfxRead_DelayFrequency = val;
}

// --

void GUIFunc_Screen_TileSize( struct Config *cfg )
{
U32 val;

	GetAttrs( GUIObjects[ GID_Screen_TileSize ],
		INTEGER_Number, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.TileSize = val;

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

void Page_Refresh_Screen( struct Config *cfg )
{
	mySetTags( cfg, GUIObjects[ GID_Screen_DelaySlider ],
		SLIDER_Level, cfg->GfxRead_DelayFrequency,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Screen_TileSize ],
		INTEGER_Number,	cfg->cfg_Disk_Settings.TileSize,
		TAG_END
	);
}

// --

#endif // GUI_RA
