
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

// --

Object *Page_Stats( struct Config *cfg )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,				LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,				BVS_SBAR_VERT,
			LAYOUT_Label,					"Server Stats",

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Accecpted ] = NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					BUTTON_Integer,			cfg->Connect_Accecpted,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Accecpted",
			End,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Rejected ] = NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					BUTTON_Integer,			cfg->Connect_Rejected,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Rejected",
			End,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Allowed ] = NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					BUTTON_Integer,			cfg->Connect_Allowed_IPs,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Allowed IPs",
			End,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Blocked ] = NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					BUTTON_Integer,			cfg->Connect_Blocked_IPs,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Blocked IPs",
			End,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Connect ] = NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Started at",
			End,

			LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Duration ] = NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					NewObject( LabelClass, NULL,
				LABEL_Text,					"Duration",
			End,

		End,
		CHILD_WeightedHeight,				0,

		LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
		End,
	End;

	return( o );
}

// --

void Page_Refresh_Stats( struct Config *cfg )
{
	mySetTags( cfg, GUIObjects[ GID_Stats_Accecpted ],
		BUTTON_Integer,	cfg->Connect_Accecpted,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Stats_Rejected ],
		BUTTON_Integer,	cfg->Connect_Rejected,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Stats_Allowed ],
		BUTTON_Integer,	cfg->Connect_Allowed_IPs,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Stats_Blocked ],
		BUTTON_Integer,	cfg->Connect_Blocked_IPs,
		TAG_END
	);

	if ( cfg->cfg_ServerStatus != PROCESS_Stopped )
	{
		static char Buf_Duration[64];
		static char Buf_Connect[64];

		LogTime_GetConnectTime(  & cfg->Server_LogTime, Buf_Connect,  sizeof( Buf_Connect ));
		LogTime_GetDurationTime( & cfg->Server_LogTime, Buf_Duration, sizeof( Buf_Duration ));

		mySetTags( cfg, GUIObjects[ GID_Stats_Connect ],
			GA_Text, Buf_Connect,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_Stats_Duration ],
			GA_Text, Buf_Duration,
			TAG_END
		);
	}
}

// --
