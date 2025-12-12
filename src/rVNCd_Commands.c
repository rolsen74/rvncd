
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

struct VNCMsgPort CmdMsgPort;

// --

static S32 myCmd_HandleLogString(	struct Config *cfg, PTR msgptr );
static S32 myCmd_HandleKey(			struct Config *cfg, PTR msgptr );
static S32 myCmd_HandleEncoding(	struct Config *cfg, PTR msgptr );

// --

S32 Cmd_Init( struct Config *cfg )
{
S32 retval;

	retval = FALSE;

	if ( ! MsgPort_Init( & CmdMsgPort ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating Command MsgPort" );
		goto bailout;
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --

void Cmd_Free( void )
{
	MsgPort_Free( & CmdMsgPort );
}

// --

void Cmd_Handle( struct Config *cfg )
{
struct CommandMessage *msg;
S32 doFree;

	while( TRUE )
	{
		msg = MsgPort_GetMsg( & CmdMsgPort );

		if ( ! msg )
		{
			break;
		}

		doFree = TRUE;

		switch( msg->cm_Command )
		{
			case CMD_Key:
			{
				doFree = myCmd_HandleKey( cfg, msg );
				break;
			} 

			case CMD_PxlFmt:
			{
				if ( ActiveGUI.GUI_PxlFmtMessage )
				{
					 ActiveGUI.GUI_PxlFmtMessage( cfg, (PTR) msg );
				}
				break;
			}

			case CMD_Session:
			{
				if ( ActiveGUI.GUI_SessionMessage )
				{
					 ActiveGUI.GUI_SessionMessage( cfg, (PTR) msg );
				}
				break;
			} 

			case CMD_Encoding:
			{
				doFree = myCmd_HandleEncoding( cfg, msg );
				break;
			}

			case CMD_LogString:
			{
				doFree = myCmd_HandleLogString( cfg, msg );
				break;
			}

			case CMD_RefreshGUI:
			{
				if ( ActiveGUI.GUI_RefreshMessage )
				{
					 ActiveGUI.GUI_RefreshMessage( cfg, (PTR) msg );
				}
				break;
			} 

			case CMD_ConnectStats:
			{
				if ( ActiveGUI.GUI_RefreshStats )
				{
					 ActiveGUI.GUI_RefreshStats( cfg, (PTR) msg );
				}
				break;
			} 

			default:
			{
				break;
			}
		}

		if ( doFree )
		{
			mem_Free( msg );
		}
	}
}

// --

static S32 myCmd_HandleKey( struct Config *cfg UNUSED, PTR msgptr )
{
struct CommandKey *msg;
S32 doFree;

	List_AddTail( & KeyLoggerList, msgptr );

	while( MAX_KeyListNodes < KeyLoggerList.vl_Entries )
	{
		msg = List_RemHead( & KeyLoggerList );
		mem_Free( msg );
	}

	#ifdef __RVNCD_GUI_H__
	myGUI_AddKeyMessage( cfg, msgptr );
	#endif // __RVNCD_GUI_H__

	doFree = FALSE;

	return( doFree );
}

// --

static S32 myCmd_HandleLogString( struct Config *cfg, PTR msgptr )
{
struct CommandLogString *msg;
S32 doFree;

	List_AddTail( & LogStringList, msgptr );

	while( MAX_LogListNodes < LogStringList.vl_Entries )
	{
		msg = List_RemHead( & LogStringList );
		mem_Free( msg );
	}

	if ( ActiveGUI.Log_AddMsg )
	{
		ActiveGUI.Log_AddMsg( cfg, msgptr );
	}

	doFree = FALSE;

	return( doFree );
}

// --

static S32 myCmd_HandleEncoding( struct Config *cfg UNUSED, PTR msgptr )
{
S32 doFree;

	if ( ActiveEncoding )
	{
		mem_Free( ActiveEncoding );
	}

	ActiveEncoding = msgptr;

	#ifdef __RVNCD_GUI_H__

	if ( cfg->cfg_WinData[WIN_Encodings].Status == WINSTAT_Open )
	{
		myGUI_EncodingMessage( cfg, msgptr );
	}

	#endif // __RVNCD_GUI_H__

	doFree = FALSE;

	return( doFree );
}

// --
