
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 List_Init( struct VNCList *l )
{
S32 retval;

	retval = FALSE;

	while( l )
	{
		memset( l, 0, sizeof( struct VNCList ));

		l->vl_StructID			= ID_VNCLIST;
		l->vl_List.lh_Head		= (PTR) & l->vl_List.lh_Tail;
		l->vl_List.lh_Tail		= (PTR) NULL;
		l->vl_List.lh_TailPred	= (PTR) & l->vl_List.lh_Head;

		if ( DoVerbose > 2 )
		{
			SHELLBUF_PRINTF( "Setup List %p\n", (PTR) l );
		}

		retval = TRUE;
		break;
	}

	return(	retval );
}

// --

void List_Free( struct VNCList *l )
{
	if ( ! l )
	{
		return;
	}

	if ( ! l->vl_StructID )
	{
		return;
	}

	if ( l->vl_StructID != ID_VNCLIST )
	{
		SHELLBUF_PRINTF( "Invalid VNC List (%p)\n", l );
		return;
	}

	l->vl_StructID = 0;

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "Clear List %p\n", (PTR) l );
	}
}

// --

PTR List_GetHead( struct VNCList *l )
{
PTR node = NULL;

	if ( l )
	{
		if ( ! IsListEmpty( & l->vl_List ))
		{
			node = l->vl_List.lh_Head;
		}
	}

	return( node );
}

// --

PTR List_GetNext( PTR nodeptr )
{
struct Node *np = nodeptr;
PTR n;

	if (( np )
	&&	( np->ln_Succ )
	&&	( np->ln_Succ->ln_Succ ))
	{
		n = np->ln_Succ;
	}
	else
	{
		n = NULL;
	}

	return( n );
}

// --

PTR List_GetPrev( PTR nodeptr )
{
struct Node *np = nodeptr;
PTR n;

	if (( np )
	&&	( np->ln_Pred )
	&&	( np->ln_Pred->ln_Pred ))
	{
		n = np->ln_Pred;
	}
	else
	{
		n = NULL;
	}

	return( n );
}

// --

void List_AddTail( struct VNCList *l, PTR node )
{
	if (( ! l ) || ( ! node ))
	{
		return;
	}

	if ( l->vl_StructID != ID_VNCLIST )
	{
		SHELLBUF_PRINTF( "Invalid VNC List (%p)\n", l );
		return;
	}

	AddTail( & l->vl_List, node );

	l->vl_Entries++;
}

// --

PTR List_RemHead( struct VNCList *l )
{
PTR node;

	node = NULL;

	if ( ! l )
	{
		goto bailout;
	}

	if ( l->vl_StructID != ID_VNCLIST )
	{
		SHELLBUF_PRINTF( "Invalid VNC List (%p)\n", l );
		goto bailout;
	}

	node = RemHead( & l->vl_List );

	if ( node )
	{
		l->vl_Entries--;
	}
	else
	{
		l->vl_Entries = 0;
	}

	bailout:

	return( node );
}

// --

void List_Insert( struct VNCList *l, PTR n1, PTR n2 )
{
	if (( ! l ) || ( ! n1 ) || ( ! n2 ))
	{
		return;
	}

	if ( l->vl_StructID != ID_VNCLIST )
	{
		SHELLBUF_PRINTF( "Invalid VNC List (%p)\n", l );
		return;
	}

	Insert( & l->vl_List, n1, n2 );

	l->vl_Entries++;
}

// --
