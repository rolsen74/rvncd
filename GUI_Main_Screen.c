
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static Object *Page_Screen( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,

		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,

		#if 1
		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Screen Grabber",

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Screen_TileSize ] = IIntuition->NewObject( IntegerClass, NULL,
					GA_ID,					GID_Screen_TileSize,
					GA_RelVerify,			TRUE,
					GA_TabCycle,			TRUE,
					INTEGER_Minimum,		16,
					INTEGER_Maximum,		256,
					INTEGER_Number,			cfg->cfg_Disk_Settings.TileSize,
				End,
				CHILD_WeightedWidth,		80,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		220,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Tile Size",
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_VERT,
				LAYOUT_AddChild,			GUIObjects[ GID_Screen_DelaySlider ] = IIntuition->NewObject( SliderClass, NULL,
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
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Delay Frequency",
			End,
		End,
		CHILD_WeightedHeight,				0,
		#endif

		LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

static void GUIFunc_Screen_Delay( struct Config *cfg )
{
uint32 val;

	IIntuition->GetAttrs( GUIObjects[ GID_Screen_DelaySlider ],
		SLIDER_Level, & val,
		TAG_END
	);

	cfg->GfxRead_DelayFrequency = val;
}

// --

static void GUIFunc_Screen_TileSize( struct Config *cfg )
{
uint32 val;

	IIntuition->GetAttrs( GUIObjects[ GID_Screen_TileSize ],
		INTEGER_Number, & val,
		TAG_END
	);

	cfg->cfg_Disk_Settings.TileSize = val;

	// --
	myGUI_Main_CheckSettings( cfg );
}

// --

static void Page_Refresh_Screen( struct Config *cfg )
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
