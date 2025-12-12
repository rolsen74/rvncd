
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "win.h"

#ifdef GUI_RA

// --

Object *Page_IP_List( struct Config *cfg UNUSED )
{
Object *o;

	o = NewObject( LayoutClass, NULL,
		LAYOUT_Orientation,						LAYOUT_ORIENT_VERT,

		LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
		End,
		CHILD_WeightedHeight,					20,

		LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
			LAYOUT_Orientation,					LAYOUT_ORIENT_VERT,
			LAYOUT_BevelStyle,					BVS_GROUP,
			LAYOUT_Label,						"IP List",

			LAYOUT_AddChild,					GUIObjects[ GID_IPList ] = NewObject( ListBrowserClass, NULL,
				GA_ID,							GID_IPList,
				GA_RelVerify,					TRUE,
				LISTBROWSER_ShowSelected,		TRUE,
				LISTBROWSER_AutoFit,			TRUE,
//				LISTBROWSER_Labels,				& MainListHeader.vl_List,
				LISTBROWSER_MinVisible,			5,
				LISTBROWSER_Striping,			LBS_ROWS,
			End,

			LAYOUT_AddChild,					NewObject( LayoutClass, NULL,
				LAYOUT_Orientation,				LAYOUT_ORIENT_HORIZ,

				LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
				End,

				LAYOUT_AddChild,				NewObject( LayoutClass, NULL,
					LAYOUT_EvenSize,			TRUE,
					LAYOUT_Orientation,			LAYOUT_ORIENT_HORIZ,

					LAYOUT_AddChild,			GUIObjects[ GID_IPAdd ] = NewObject( ButtonClass, NULL,
						GA_ID,					GID_IPAdd,
						GA_Text,				"Add",
						GA_RelVerify,			TRUE,
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_IPEdit ] = NewObject( ButtonClass, NULL,
						GA_ID,					GID_IPEdit,
						GA_Text,				"Edit",
						GA_Disabled,			TRUE,
						GA_RelVerify,			TRUE,
					End,

					LAYOUT_AddChild,			GUIObjects[ GID_IPDelete ] = NewObject( ButtonClass, NULL,
						GA_ID,					GID_IPDelete,
						GA_Text,				"Delete",
						GA_Disabled,			TRUE,
						GA_RelVerify,			TRUE,
					End,

				End,

				LAYOUT_AddChild,				GUIObjects[ GID_IPUp ] = NewObject( ButtonClass, NULL,
					GA_ID,						GID_IPUp,
					GA_Disabled,				TRUE,
					GA_RelVerify,				TRUE,
					BUTTON_AutoButton,			BAG_UPARROW,
				End,
				CHILD_WeightedWidth,			0,

				LAYOUT_AddChild,				GUIObjects[ GID_IPDown ] = NewObject( ButtonClass, NULL,
					GA_ID,						GID_IPDown,
					GA_Disabled,				TRUE,
					GA_RelVerify,				TRUE,
					BUTTON_AutoButton,			BAG_DNARROW,
				End,
				CHILD_WeightedWidth,			0,
			End,
			CHILD_WeightedHeight,				0,
		End,
		CHILD_WeightedHeight,					80,

		LAYOUT_AddChild,						NewObject( LayoutClass, NULL,
		End,
		CHILD_WeightedHeight,					20,
	End;

	if ( o )
	{
		myGUI_Main_UpdateIPList( cfg );
	}

	return( o );
}

// --

void myGUI_Main_UpdateIPList( struct Config *cfg )
{
struct IPNode *node;
struct Node *n;
char str[64];
S32 len;

	mySetTags( cfg, GUIObjects[ GID_IPList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	FreeListBrowserList( & MainIPList.vl_List );
	MainIPList.vl_Entries = 0;

	ObtainSemaphore( & cfg->IPSema );
	node = List_GetHead( & cfg->IPList );

	while( node )
	{
		/**/ if ( node->ipn_Type == IPT_Allow )	sprintf( str, "Allow : " );
		else if ( node->ipn_Type == IPT_Block )	sprintf( str, "Block : " );
		else									sprintf( str, "Unknown : " );

		len = strlen( str );

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

		n = AllocListBrowserNode( 1,
			LBNA_Column, 0,
				LBNCA_CopyText, TRUE,
				LBNCA_Text, str,
			TAG_END
		);

		if ( n )
		{
			n->ln_Name = (PTR) node;
			List_AddTail( & MainIPList, n );
		}

		node = List_GetNext( node );
	}
	ReleaseSemaphore( & cfg->IPSema );

	mySetTags( cfg, GUIObjects[ GID_IPList ],
		LISTBROWSER_Labels, & MainIPList.vl_List,
		TAG_END
	);
}

// --

void GUIFunc_IPUp( struct Config *cfg )
{
struct Node *prev;
struct Node *node;
struct Node *prev2;
struct Node *node2;
U32 pos;

	node = NULL;
	pos = 0;

	GetAttrs( GUIObjects[ GID_IPList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	if ( ! node )
	{
		goto bailout;
	}

	prev = List_GetPrev( node );

	if ( ! prev )
	{
		goto bailout;
	}

	node2 = (PTR) node->ln_Name;
	prev2 = (PTR) prev->ln_Name;

	mySetTags( cfg, GUIObjects[ GID_IPList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);


	ObtainSemaphore( & cfg->IPSema );
	// Change LB list
	Remove( prev );
	List_Insert( & MainIPList, prev, node );

	// Change IP list
	Remove( prev2 );
	List_Insert( & cfg->IPList, prev2, node2 );
	//
	ReleaseSemaphore( & cfg->IPSema );

	mySetTags( cfg, GUIObjects[ GID_IPList ],
		LISTBROWSER_Labels, & MainIPList.vl_List,
		LISTBROWSER_MakeVisible, pos - 1,
		LISTBROWSER_Selected, pos - 1,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_IPUp ],
		GA_Disabled, ( ! List_GetPrev( node )),
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_IPDown ],
		GA_Disabled, ( ! List_GetNext( node )),
		TAG_END
	);

bailout:

	return;
}

// --

void GUIFunc_IPDown( struct Config *cfg )
{
struct Node *node;
struct Node *next;
struct Node *node2;
struct Node *next2;
U32 pos;

	node = NULL;
	pos = 0;

	GetAttrs( GUIObjects[ GID_IPList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	if ( ! node )
	{
		goto bailout;
	}

	next = List_GetNext( node );

	if ( ! next )
	{
		goto bailout;
	}

	node2 = (PTR) node->ln_Name;
	next2 = (PTR) next->ln_Name;

	mySetTags( cfg, GUIObjects[ GID_IPList ],
		LISTBROWSER_Labels, NULL,
		TAG_END
	);

	ObtainSemaphore( & cfg->IPSema );
	// Change LB list
	Remove( node );
	List_Insert( & MainIPList, node, next );

	// Change IP list
	Remove( node2 );
	List_Insert( & cfg->IPList, node2, next2 );
	//
	ReleaseSemaphore( & cfg->IPSema );

	mySetTags( cfg, GUIObjects[ GID_IPList ],
		LISTBROWSER_Labels, & MainIPList.vl_List,
		LISTBROWSER_MakeVisible, pos + 1,
		LISTBROWSER_Selected, pos + 1,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_IPUp ],
		GA_Disabled, ( ! List_GetPrev( node )),
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_IPDown ],
		GA_Disabled, ( ! List_GetNext( node )),
		TAG_END
	);

bailout:

	return;
}

// --

static void myIPEdit( struct Config *cfg, struct Node *node )
{
	if ( ! node )
	{
		return;
	}

	if ( cfg->cfg_IPEditStat != IPES_Unused )
	{
		return;
	}

	cfg->cfg_IPEditStat = IPES_Edit;
	cfg->cfg_IPEditNode = node;

	if ( cfg->cfg_WinData[WIN_Main].BusyWin )
	{
		 cfg->cfg_WinData[WIN_Main].BusyWin( cfg, TRUE );
	}

	cfg->cfg_WinData[WIN_IPEdit].Status = WINSTAT_Open;

	if ( cfg->cfg_WinData[WIN_IPEdit].OpenWin )
	{
		 cfg->cfg_WinData[WIN_IPEdit].OpenWin( cfg );
	}
}

// --

void GUIFunc_IPList( struct Config *cfg )
{
struct Node *node;
U32 relevent;
U32 total;
U32 pos;

	relevent = 0;
	total = 0;
	node = NULL;
	pos = 0;

	GetAttrs( GUIObjects[ GID_IPList ],
		LISTBROWSER_SelectedNode, & node,
		LISTBROWSER_TotalNodes, & total,
		LISTBROWSER_RelEvent, & relevent,
		LISTBROWSER_Selected, & pos,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_IPDelete ],
		GA_Disabled, ( ! node ) ? TRUE : FALSE,
		TAG_END
	);

	mySetTags( cfg, GUIObjects[ GID_IPEdit ],
		GA_Disabled, ( ! node ) ? TRUE : FALSE,
		TAG_END
	);

	if ( ! node )
	{
		mySetTags( cfg, GUIObjects[ GID_IPUp ],
			GA_Disabled, TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_IPDown ],
			GA_Disabled, TRUE,
			TAG_END
		);
	}
	else
	{
		mySetTags( cfg, GUIObjects[ GID_IPUp ],
			GA_Disabled, ( pos != 0 ) ? FALSE : TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_IPDown ],
			GA_Disabled, ( pos < total - 1 ) ? FALSE : TRUE,
			TAG_END
		);
	}

	if (( node ) &&( relevent & LBRE_DOUBLECLICK ))
	{
		myIPEdit( cfg, node );
	}
}

// --

void GUIFunc_IPEdit( struct Config *cfg )
{
struct Node *node;

	node = NULL;

	GetAttrs( GUIObjects[ GID_IPList ],
		LISTBROWSER_SelectedNode, & node,
		TAG_END
	);

	if ( node )
	{
		myIPEdit( cfg, node );
	}
}

// --

void GUIFunc_IPAdd( struct Config *cfg )
{
	if ( cfg->cfg_IPEditStat != IPES_Unused )
	{
		return;
	}

	cfg->cfg_IPEditStat = IPES_Add;
	cfg->cfg_IPEditNode = NULL;

	if ( cfg->cfg_WinData[WIN_Main].BusyWin )
	{
		 cfg->cfg_WinData[WIN_Main].BusyWin( cfg, TRUE );
	}
	cfg->cfg_WinData[WIN_IPEdit].Status = WINSTAT_Open;
	cfg->cfg_WinData[WIN_IPEdit].OpenWin( cfg );
}

// --

void Page_Refresh_IP( struct Config *cfg )
{
struct Node *n;

	n = NULL;

	GetAttrs( GUIObjects[ GID_IPList ],
		LISTBROWSER_SelectedNode, & n,
		TAG_END
	);

	if ( n )
	{
		mySetTags( cfg, GUIObjects[ GID_IPUp ],
			GA_Disabled, ( ! List_GetPrev( n )),
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_IPDown ],
			GA_Disabled, ( ! List_GetNext( n )),
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_IPEdit ],
			GA_Disabled, FALSE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_IPDelete ],
			GA_Disabled, FALSE,
			TAG_END
		);
	}
	else
	{
		mySetTags( cfg, GUIObjects[ GID_IPUp ],
			GA_Disabled, TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_IPDown ],
			GA_Disabled, TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_IPEdit ],
			GA_Disabled, TRUE,
			TAG_END
		);

		mySetTags( cfg, GUIObjects[ GID_IPDelete ],
			GA_Disabled, TRUE,
			TAG_END
		);
	}
}

// --

void GUIFunc_IPDelete( struct Config *cfg )
{
struct IPNode *node;
struct Node *n;

	n = NULL;

	GetAttrs( GUIObjects[ GID_IPList ],
		LISTBROWSER_SelectedNode, & n,
		TAG_END
	);

	if ( n )
	{
		node = (PTR) n->ln_Name;

		Remove( n );
		mem_Free( n );

		if ( node )
		{
			Remove( (PTR) node );
			mem_Free( node );
		}

		myGUI_Main_UpdateIPList( cfg );
		Page_Refresh_IP( cfg );
	}
}

// --

#endif // GUI_RA
