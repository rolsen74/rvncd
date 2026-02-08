
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

void GUIFunc_Handle( struct Config *cfg )
{
//	DEBUGPRINTF( DebugPrintF( "RA: GUIFunc_Handle\n" );

	#if 0

// Problem is we remove the imsg from the MsgPort
// Then calling the WinObject with WM_HANDLEINPUT will fail.
// If its was a classic Intuition Window and not a Reaction
// window, we would handle each idcmp message one at a time.

struct IntuiMessage *msg;
void (*func)( struct Config *cfg );

	while( TRUE )
	{
		msg = MsgPort_GetMsg( &  WinMsgPort );

		if ( ! msg )
		{
			break;
		}

		if (( msg->IDCMPWindow ) && (( func = (PTR) msg->IDCMPWindow->UserData )))
		{
			func( cfg );
		}

		ReplyMsg( & msg->ExecMessage );
	}

	#else

S32 cnt;

	for( cnt=0 ; cnt<WIN_LAST ; cnt++ )
	{
		if ( cfg->cfg_WinData[cnt].HandleWin )
		{
			 cfg->cfg_WinData[cnt].HandleWin( cfg );
		}
	}

	#endif
}

// --
