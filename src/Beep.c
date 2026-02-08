
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

static PTR				oldFunc		= NULL;
static struct Config *	Config		= NULL;

// --
#ifdef _AOS4_

static void newFunc_aos4( struct IntuitionIFace *self, struct Screen *scr )
{
void (*func)(struct IntuitionIFace *self, struct Screen *);

	func = oldFunc;

	// --
	// Only send Message, if enabled and Client connected

	if (( Config->cfg_Active_Settings.SendBell )
	&&	( Config->cfg_NetSendStatus == PROCESS_Running ))
	{
		ObtainSemaphore( & Config->Server_UpdateSema );

		struct UpdateNode *un = List_RemHead( & Config->Server_UpdateFree );

		if ( ! un )
		{
			un = mem_AllocClr( sizeof( struct UpdateNode ) );

			if ( ! un )
			{
				SHELLBUF_PRINTF( "Yikes Calloc failed\n" );
			}
		}

		if ( un )
		{
			un->un_Type = UT_Beep;
			List_AddTail( & Config->Server_UpdateList, & un->un_Node );
		}

		ReleaseSemaphore( & Config->Server_UpdateSema );
	}

	// --

	if ( func )
	{
		func( self, scr );
	}
}

#endif
// --

void Beep_Patch_Install( struct Config *cfg )
{
	Config = cfg;

	// ptr oldfunc = SetFunction( ifc, offset, newfunc )
	// DisplayBeep( iintuition, scr )

	#ifdef _AOS4_
	oldFunc = SetMethod( (PTR) IIntuition, offsetof( struct IntuitionIFace, DisplayBeep ), newFunc_aos4 );
	CacheClearU();
	#endif
}

// --

void Beep_Patch_Remove( struct Config *cfg UNUSED )
{
	if ( oldFunc )
	{
		#ifdef _AOS4_
		SetMethod( (PTR) IIntuition, offsetof( struct IntuitionIFace, DisplayBeep ), oldFunc );
		#endif
	}
}

// --
