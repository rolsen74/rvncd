
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static Object *Page_Stats( struct Config *cfg )
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
			LAYOUT_Label,					"Server Stats",

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Accecpted ] = IIntuition->NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					BUTTON_Integer,			cfg->Connect_Accecpted,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Accecpted",
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Rejected ] = IIntuition->NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					BUTTON_Integer,			cfg->Connect_Rejected,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Rejected",
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Whitelisted ] = IIntuition->NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					BUTTON_Integer,			cfg->Connect_WhiteListed,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Whitelisted",
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Blacklisted ] = IIntuition->NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					BUTTON_Integer,			cfg->Connect_BlackListed,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Blacklisted",
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Connect ] = IIntuition->NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Started at",
			End,

			LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,
				LAYOUT_AddChild,			GUIObjects[ GID_Stats_Duration ] = IIntuition->NewObject( ButtonClass, NULL,
					BUTTON_BevelStyle,		BVS_THIN,
					GA_Text,				"------------",
					GA_ReadOnly,			TRUE,
				End,
				CHILD_MinWidth,				150,
				CHILD_WeightedWidth,		200,
				LAYOUT_AddChild,			IIntuition->NewObject( LayoutClass, NULL,
				End,
				CHILD_WeightedWidth,		100,
			End,
			CHILD_Label,					IIntuition->NewObject( LabelClass, NULL,
				LABEL_Text,					"Duration",
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

#if 0
static char *myDuration( struct Config *cfg, char *buf )
{
struct TimeVal *time;
struct TimeVal cur;
char *yearsbuf;
char *daysbuf;
char *hoursbuf;
char *minsbuf;
char *secsbuf;
int years;
int days;
int hours;
int mins;
int val;

	time = & cfg->Server_Time_Logon;

	if ( cfg->cfg_ServerStatus != PROCESS_Stopped )
	{
		// Server Still Running
		ITimer->GetUpTime( & cur );
		ITimer->SubTime( & cur, time );
	}
	else
	{
		// Server no longer Running 
		cur = cfg->Server_Time_Logoff;
		ITimer->SubTime( & cur, time );
	}

	// --

	val = 60 * 60 * 24 * 365;

	years = cur.Seconds / val;

	cur.Seconds -= val * years;

	// --

	val = 60 * 60 * 24;

	days = cur.Seconds / val;

	cur.Seconds -= val * days;

	// --

	val = 60 * 60;

	hours = cur.Seconds / val;

	cur.Seconds -= val * hours;

	// --

	val = 60;

	mins = cur.Seconds / val;

	cur.Seconds -= val * mins;

	// --

	if ( years )
	{
		yearsbuf = myASPrintF( "%ldy ", years );
	}
	else
	{
		yearsbuf = myASPrintF( "" );
	}

	// --

	if ( days )
	{
		daysbuf = myASPrintF( "%ldd ", days );
	}
	else
	{
		daysbuf = myASPrintF( "" );
	}

	// --

	if ( hours )
	{
		hoursbuf = myASPrintF( "%ldh ", hours );
	}
	else
	{
		hoursbuf = myASPrintF( "" );
	}

	// --

	if ( mins )
	{
		minsbuf = myASPrintF( "%ldm ", mins );
	}
	else
	{
		minsbuf = myASPrintF( "" );
	}

	// --

	if (( cur.Seconds ) || (( years == 0 ) && ( days == 0 ) && ( hours == 0 ) && ( mins == 0 )))
	{
		secsbuf = myASPrintF( "%lds ", cur.Seconds );
	}
	else
	{
		secsbuf = myASPrintF( "" );
	}

	// --

	sprintf( buf, "%s%s%s%s%s", yearsbuf, daysbuf, hoursbuf, minsbuf, secsbuf );

	// --

	myFree( yearsbuf );
	myFree( daysbuf );
	myFree( hoursbuf );
	myFree( minsbuf );
	myFree( secsbuf );

	// --

	return( buf );
}
#endif

// --

static void Page_Refresh_Stats( struct Config *cfg )
{
	mySetTags( cfg, GUIObjects[ GID_Stats_Accecpted ],
		BUTTON_Integer,	cfg->Connect_Accecpted,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Stats_Rejected ],
		BUTTON_Integer,	cfg->Connect_Rejected,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Stats_Whitelisted ],
		BUTTON_Integer,	cfg->Connect_WhiteListed,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_Stats_Blacklisted ],
		BUTTON_Integer,	cfg->Connect_BlackListed,
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
