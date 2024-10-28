
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

static Object *Page_White_List( struct Config *cfg UNUSED )
{
Object *o;

	o = IIntuition->NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
		End,
		CHILD_WeightedHeight,				    20,

		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,					BVS_GROUP,
			LAYOUT_Label,						"White List",

			LAYOUT_AddChild,					GUIObjects[ GID_WhiteList ] = IIntuition->NewObject( ListBrowserClass, NULL,
				GA_ID,							GID_WhiteList,
				GA_RelVerify,					TRUE,
				LISTBROWSER_ShowSelected,		TRUE,
				LISTBROWSER_AutoFit,			TRUE,
//				LISTBROWSER_Labels,				& MainListHeader,
				LISTBROWSER_MinVisible,			5,
				LISTBROWSER_Striping,			LBS_ROWS,
			End,

			LAYOUT_AddChild,					IIntuition->NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,				LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
				End,

				LAYOUT_AddChild,				IIntuition->NewObject( LayoutClass, NULL,
					LAYOUT_EvenSize,			TRUE,
					LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,

					LAYOUT_AddChild,			GUIObjects[ GID_WhiteAdd ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ID,					GID_WhiteAdd,
						GA_Text,				"Add",
						GA_RelVerify,			TRUE,
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_WhiteEdit ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ID,					GID_WhiteEdit,
						GA_Text,				"Edit",
						GA_Disabled,			TRUE,
						GA_RelVerify,			TRUE,
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_WhiteDelete ] = IIntuition->NewObject( ButtonClass, NULL,
						GA_ID,					GID_WhiteDelete,
						GA_Text,				"Delete",
						GA_Disabled,			TRUE,
						GA_RelVerify,			TRUE,
					End,
				End,

				LAYOUT_AddChild,				GUIObjects[ GID_WhiteUp ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,						GID_WhiteUp,
					GA_Disabled,				TRUE,
					GA_RelVerify,				TRUE,
					BUTTON_AutoButton,			BAG_UPARROW,
				End,
				CHILD_WeightedWidth,			0,

				LAYOUT_AddChild,				GUIObjects[ GID_WhiteDown ] = IIntuition->NewObject( ButtonClass, NULL,
					GA_ID,						GID_WhiteDown,
					GA_Disabled,				TRUE,
					GA_RelVerify,				TRUE,
					BUTTON_AutoButton,			BAG_DNARROW,
				End,
				CHILD_WeightedWidth,			0,
			End,
			CHILD_WeightedHeight,				0,
		End,
		CHILD_WeightedHeight,				    80,

		LAYOUT_AddChild,						IIntuition->NewObject( LayoutClass, NULL,
		End,
		CHILD_WeightedHeight,				    20,
	End;

	if ( o )
	{
		myGUI_Main_UpdateWhiteList( cfg );
	}

	return( o );
}

// --

void myGUI_Main_UpdateWhiteList( struct Config *cfg )
{
struct IPNode *node;
struct Node *n;
char str[64];
int len;

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	IListBrowser->FreeListBrowserList( & MainWhiteList );

	node = (APTR) IExec->GetHead( & cfg->WhiteList );

	while( node )
	{
		str[0] = 0;
		len = 0;

		if ( node->ipn_A == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", node->ipn_A );
		}

		len = strlen( str );

		if ( node->ipn_B == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", node->ipn_B );
		}

		len = strlen( str );

		if ( node->ipn_C == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", node->ipn_C );
		}

		len = strlen( str );

		if ( node->ipn_D == -1 )
		{
			sprintf( & str[len], "*" );
		}
		else
		{
			sprintf( & str[len], "%d", node->ipn_D );
		}

		n = IListBrowser->AllocListBrowserNode( 1,
			LBNA_Column, 0,
				LBNCA_CopyText, TRUE,
				LBNCA_Text, str,
			TAG_END
		);

		if ( n )
		{
			n->ln_Name = (APTR) node;
			IExec->AddTail( & MainWhiteList, n );
		}

		node = (APTR) IExec->GetSucc( (APTR) node );
	}

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, & MainWhiteList,
		TAG_END
	);
}

// --

static void GUIFunc_WhiteUp( struct Config *cfg )
{
struct Node *prev;
struct Node *node;
struct Node *prev2;
struct Node *node2;
uint32 pos;

	node = NULL;
	pos = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_WhiteList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	if ( node == NULL )
	{
		goto bailout;
	}

	prev = IExec->GetPred( node );

	if ( prev == NULL )
	{
		goto bailout;
	}

	node2 = (APTR) node->ln_Name;
	prev2 = (APTR) prev->ln_Name;

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	// Change LB list
	IExec->Remove( prev );
	IExec->Insert( & MainWhiteList, prev, node );

	// Change IP list
	IExec->Remove( prev2 );
	IExec->Insert( & cfg->WhiteList, prev2, node2 );

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, & MainWhiteList,
		LISTBROWSER_MakeVisible, pos - 1,
		LISTBROWSER_Selected, pos - 1,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteUp ],
		GA_Disabled, ( IExec->GetPred( node ) == 0 ),
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteDown ],
		GA_Disabled, ( IExec->GetSucc( node ) == 0 ),
		TAG_END
	);

bailout:

	return;
}

// --

static void GUIFunc_WhiteDown( struct Config *cfg )
{
struct Node *node;
struct Node *next;
struct Node *node2;
struct Node *next2;
uint32 pos;

	node = NULL;
	pos = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_WhiteList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	if ( node == NULL )
	{
		goto bailout;
	}

	next = IExec->GetSucc( node );

	if ( next == NULL )
	{
		goto bailout;
	}

	node2 = (APTR) node->ln_Name;
	next2 = (APTR) next->ln_Name;

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	// Change LB list
	IExec->Remove( node );
	IExec->Insert( & MainWhiteList, node, next );

	// Change IP list
	IExec->Remove( node2 );
	IExec->Insert( & cfg->WhiteList, node2, next2 );

	mySetTags( cfg, GUIObjects[ GID_WhiteList ],
		LISTBROWSER_Labels, & MainWhiteList,
		LISTBROWSER_MakeVisible, pos + 1,
		LISTBROWSER_Selected, pos + 1,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteUp ],
		GA_Disabled, ( IExec->GetPred( node ) == 0 ),
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteDown ],
		GA_Disabled, ( IExec->GetSucc( node ) == 0 ),
		TAG_END
	);

bailout:

	return;
}

// --

static void myWhiteEdit( struct Config *cfg, struct Node *node )
{
	if ( ! node )
	{
		return;
	}

	if ( cfg->cfg_IPEditStat != IPES_Unused )
	{
		return;
	}

	cfg->cfg_IPEditStat = IPES_White_Edit;
	cfg->cfg_IPEditNode = node;

	cfg->cfg_WinData[WIN_Main].BusyWin( cfg, TRUE );
	cfg->cfg_WinData[WIN_IPEdit].Status = WINSTAT_Open;
	cfg->cfg_WinData[WIN_IPEdit].OpenWin( cfg );
}

// --

static void GUIFunc_WhiteList( struct Config *cfg )
{
struct Node *node;
uint32 relevent;
uint32 total;
uint32 pos;

	relevent = 0;
	total = 0;
	node = NULL;
	pos = 0;

	IIntuition->GetAttrs( GUIObjects[ GID_WhiteList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_TotalNodes, & total,
		LISTBROWSER_RelEvent, & relevent,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteDelete ],
		GA_Disabled, ( node == NULL ) ? TRUE : FALSE,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_WhiteEdit ],
		GA_Disabled, ( node == NULL ) ? TRUE : FALSE,
		TAG_END
	);

	if ( node == NULL )
	{
		mySetTags( cfg, GUIObjects[ GID_WhiteUp ],
			GA_Disabled, TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_WhiteDown ],
			GA_Disabled, TRUE,
			TAG_END
		);
	}
	else
	{
		mySetTags( cfg, GUIObjects[ GID_WhiteUp ],
			GA_Disabled, ( pos != 0 ) ? FALSE : TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_WhiteDown ],
			GA_Disabled, ( pos < total - 1 ) ? FALSE : TRUE,
			TAG_END
		);
	}

	if (( node ) &&( relevent & LBRE_DOUBLECLICK ))
	{
		myWhiteEdit( cfg, node );
	}
}

// --

static void GUIFunc_WhiteEdit( struct Config *cfg )
{
struct Node *node;

	node = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_WhiteList ],
		LISTBROWSER_SelectedNode, & node,
		TAG_END
	);

	if ( node )
	{
		myWhiteEdit( cfg, node );
	}
}

// --

static void GUIFunc_WhiteAdd( struct Config *cfg )
{
	if ( cfg->cfg_IPEditStat != IPES_Unused )
	{
		return;
	}

	cfg->cfg_IPEditStat = IPES_White_Add;
	cfg->cfg_IPEditNode = NULL;

	cfg->cfg_WinData[WIN_Main].BusyWin( cfg, TRUE );
	cfg->cfg_WinData[WIN_IPEdit].Status = WINSTAT_Open;
	cfg->cfg_WinData[WIN_IPEdit].OpenWin( cfg );
}

// --

void Page_Refresh_White( struct Config *cfg )
{
struct Node *n;

	n = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_WhiteList ],
		LISTBROWSER_SelectedNode, & n,
		TAG_END
	);

	if ( n )
	{
		mySetTags( cfg, GUIObjects[ GID_WhiteUp ],
			GA_Disabled, ( IExec->GetPred( n ) == 0 ),
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_WhiteDown ],
			GA_Disabled, ( IExec->GetSucc( n ) == 0 ),
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_WhiteEdit ],
			GA_Disabled, FALSE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_WhiteDelete ],
			GA_Disabled, FALSE,
			TAG_END
		);
	}
	else
	{
		mySetTags( cfg, GUIObjects[ GID_WhiteUp ],
			GA_Disabled, TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_WhiteDown ],
			GA_Disabled, TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_WhiteEdit ],
			GA_Disabled, TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_WhiteDelete ],
			GA_Disabled, TRUE,
			TAG_END
		);
	}
}

// --

static void GUIFunc_WhiteDelete( struct Config *cfg )
{
struct IPNode *node;
struct Node *n;

	n = NULL;

	IIntuition->GetAttrs( GUIObjects[ GID_WhiteList ],
		LISTBROWSER_SelectedNode, & n,
		TAG_END
	);

	if ( n )
	{
		node = (APTR) n->ln_Name;

		IExec->Remove( n );
		myFree( n );

		if ( node )
		{
			IExec->Remove( (APTR) node );
			myFree( node );
		}

		myGUI_Main_UpdateWhiteList( cfg );
		Page_Refresh_White( cfg );
	}
}

// --
