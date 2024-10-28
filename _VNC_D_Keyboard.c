
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

#define KEYCODE_FIRSTQUALIFIER		0x60
#define KEYCODE_LASTQUALIFIER		0x6A

// --

#if 0

struct KeyMessage
{
	uint8	km_Type;
	uint8	km_Down;
	uint8	km_Pad;
	uint8	km_Pad2;
	uint32	km_Key;
};

#endif

// --

int VNC_Keyboard( struct Config *cfg )
{
struct SocketIFace *ISocket;
struct KeyMessage *km;
struct InputEvent Event;
struct IOStdReq *IOReq;
char *text;
char rawkey[4];
char buf[4];
int discarded;
int error;
int code;
int size;
int rc;

	km = cfg->NetRead_ReadBuffer;

	text = NULL;

	error = TRUE;

	discarded = FALSE;

	ISocket = cfg->NetRead_ISocket;

	size = sizeof( struct KeyMessage );

	rc = myNetRead( cfg, km, size, MSG_WAITALL );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	if (( km->km_Type != 4 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%ld != %ld)", rc, size );
		goto bailout;
	}

	// -- Check if our Active screen is Front Most

	switch( km->km_Key )
	{
		case 0xff08: // Backspace
		{
			text = "Backspace";
			code = 0x41;
			break;
		}

		case 0xff09: // Tab
		{
			text = "Tab";
			code = 0x42;
			break;
		}

		case 0xff0d: // Return
		{
			text = "Return";
			code = 0x44;
			break;
		}

		case 0xff13: // Pause
		{
			text = "Pause";
			code = 0x6e;
			break;
		}

		case 0xff14: // Scroll Lock
		{
			text = "Scroll Lock";
			code = 0x5f;
			break;
		}

		case 0xff15: // Sys Req -- (Not seen)
		{
			text = "Sys Req";
			code = 0x6d;
			break;
		}

		case 0xff1b: // Esc
		{
			text = "Esc";
			code = 0x45;
			break;
		}

		case 0xff50: // Home
		{
			text = "Home";
			code = 0x70;
			break;
		}

		case 0xff51: // Arrow Left
		{
			text = "Arrow Left";
			code = 0x4f;
			break;
		}

		case 0xff52: // Arrow Up
		{
			text = "Arrow Up";
			code = 0x4c;
			break;
		}

		case 0xff53: // Arrow Right
		{
			text = "Arrow Right";
			code = 0x4e;
			break;
		}

		case 0xff54: // Arrow Down
		{
			text = "Arrow Down";
			code = 0x4d;
			break;
		}

		case 0xff55: // Page Up
		{
			text = "Page Up";
			code = 0x48;
			break;
		}

		case 0xff56: // Page Down
		{
			text = "Page Down";
			code = 0x49;
			break;
		}

		case 0xff57: // End
		{
			text = "End";
			code = 0x71;
			break;
		}

		case 0xff61: // Print Screen
		{
			text = "Print Screen";
			code = 0x6d;
			break;
		}

		case 0xff67: // Menu
		{
			text = "Menu";
			code = 0x5f;
			break;
		}

		case 0xff7f: // Num Lock
		{
			text = "NumLock";
			code = 0xff;
			break;
		}

		case 0xff8d: // Keypad Enter
		{
//			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Enter";
			code = 0x43;
			break;
		}

		case 0xff95: // Keypad Home
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Home";
			code = 0x3d;
			break;
		}

		case 0xff96: // Keypad Left
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Left";
			code = 0x2d;
			break;
		}

		case 0xff97: // Keypad Up
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Up";
			code = 0x3e;
			break;
		}

		case 0xff98: // Keypad Right
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Right";
			code = 0x2f;
			break;
		}

		case 0xff99: // Keypad Down
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Down";
			code = 0x1e;
			break;
		}

		case 0xff9a: // Keypad Page Up
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Page Up";
			code = 0x3f;
			break;
		}

		case 0xff9b: // Keypad Page Down
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Page Down";
			code = 0x1f;
			break;
		}

		case 0xff9c: // Keypad End
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad End";
			code = 0x1d;
			break;
		}

		case 0xff9d: // Keypad Begin '5'
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Begin";
			code = 0x2e;
			break;
		}

		case 0xff9e: // Keypad Insert
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Insert";
			code = 0x0f;
			break;
		}

		case 0xff9f: // Keypad Delete
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_NUMERICPAD;
			text = "Keypad Delete";
			code = 0x3c;
			break;
		}

		case 0xffaa: // Keypad * (Multiply)
		{
			text = "Keypad *";
			code = 0x5d;
			break;
		}

		case 0xffab: // Keypad + (Add)
		{
			text = "Keypad +";
			code = 0x5e;
			break;
		}

		case 0xffac: // Keypad , (Comma)
		{
			text = "Keypad ,";
			code = 0x3c;
			break;
		}

		case 0xffad: // Keypad - (Subtract)
		{
			text = "Keypad -";
			code = 0x4a;
			break;
		}

		case 0xffaf: // Keypad / (Divide)
		{
			text = "Keypad /";
			code = 0x5c;
			break;
		}

		case 0xffb0: // Keypad 0
		{
			text = "Keypad 0";
			code = 0x0f;
			break;
		}

		case 0xffb1: // Keypad 1
		{
			text = "Keypad 1";
			code = 0x1d;
			break;
		}

		case 0xffb2: // Keypad 2
		{
			text = "Keypad 2";
			code = 0x1e;
			break;
		}

		case 0xffb3: // Keypad 3
		{
			text = "Keypad 3";
			code = 0x1f;
			break;
		}

		case 0xffb4: // Keypad 4
		{
			text = "Keypad 4";
			code = 0x2d;
			break;
		}

		case 0xffb5: // Keypad 5
		{
			text = "Keypad 5";
			code = 0x2e;
			break;
		}

		case 0xffb6: // Keypad 6
		{
			text = "Keypad 6";
			code = 0x2f;
			break;
		}

		case 0xffb7: // Keypad 7
		{
			text = "Keypad 7";
			code = 0x3d;
			break;
		}

		case 0xffb8: // Keypad 8
		{
			text = "Keypad 8";
			code = 0x3e;
			break;
		}

		case 0xffb9: // Keypad 9
		{
			text = "Keypad 9";
			code = 0x3f;
			break;
		}

		case 0xffbe: // F1
		{
			text = "F1";
			code = 0x50;
			break;
		}

		case 0xffbf: // F2
		{
			text = "F2";
			code = 0x51;
			break;
		}

		case 0xffc0: // F3
		{
			text = "F3";
			code = 0x52;
			break;
		}

		case 0xffc1: // F4
		{
			text = "F4";
			code = 0x53;
			break;
		}

		case 0xffc2: // F5
		{
			text = "F5";
			code = 0x54;
			break;
		}

		case 0xffc3: // F6
		{
			text = "F6";
			code = 0x55;
			break;
		}

		case 0xffc4: // F7
		{
			text = "F7";
			code = 0x56;
			break;
		}

		case 0xffc5: // F8
		{
			text = "F8";
			code = 0x57;
			break;
		}

		case 0xffc6: // F9
		{
			text = "F9";
			code = 0x58;
			break;
		}

		case 0xffc7: // F10
		{
			text = "F10";
			code = 0x59;
			break;
		}

		case 0xffc8: // F11
		{
			text = "F11";
			code = 0x4b;
			break;
		}

		case 0xffc9: // F12
		{
			text = "F12";
			code = 0x6f;
			break;
		}

		case 0xffe1: // Left Shift
		{
			if ( km->km_Down )
			{
				cfg->NetRead_Qualifier |= IEQUALIFIER_LSHIFT;
			}
			else
			{
				cfg->NetRead_Qualifier &= ~IEQUALIFIER_LSHIFT;
			}

			text = "Left Shift";
			code = 0x60;
			break;
		}

		case 0xffe2: // Right Shift -- (Not seen)
		{
			if ( km->km_Down )
			{
				cfg->NetRead_Qualifier |= IEQUALIFIER_RSHIFT;
			}
			else
			{
				cfg->NetRead_Qualifier &= ~IEQUALIFIER_RSHIFT;
			}

			text = "Right Shift";
			code = 0x61;
			break;
		}

		case 0xffe3: // Left Ctrl
		{
			if ( km->km_Down )
			{
				cfg->NetRead_Qualifier |= IEQUALIFIER_CONTROL;
			}
			else
			{
				cfg->NetRead_Qualifier &= ~IEQUALIFIER_CONTROL;
			}

			text = "Left Ctrl";
			code = 0x63;
			break;
		}

		case 0xffe4: // Right Ctrl
		{
			if ( km->km_Down )
			{
				cfg->NetRead_Qualifier |= IEQUALIFIER_CONTROL;
			}
			else
			{
				cfg->NetRead_Qualifier &= ~IEQUALIFIER_CONTROL;
			}

			text = "Right Ctrl";
			code = 0x63;
			break;
		}

		case 0xffe5: // Caps Lock
		{
			text = "Caps Lock";
			code = 0x62;
			break;
		}

		case 0xffe9: // Left Alt
		{
			if ( km->km_Down )
			{
				cfg->NetRead_Qualifier |= IEQUALIFIER_LALT;
			}
			else
			{
				cfg->NetRead_Qualifier &= ~IEQUALIFIER_LALT;
			}

			text = "Left Alt";
			code = 0x64;
			break;
		}

		case 0xffea: // Right Alt
		{
			if ( km->km_Down )
			{
				cfg->NetRead_Qualifier |= IEQUALIFIER_RALT;
			}
			else
			{
				cfg->NetRead_Qualifier &= ~IEQUALIFIER_RALT;
			}

			text = "Right Alt";
			code = 0x65;
			break;
		}

		case 0xffeb: // Left Amiga
		{
			if ( km->km_Down )
			{
				cfg->NetRead_Qualifier |= IEQUALIFIER_LCOMMAND;
			}
			else
			{
				cfg->NetRead_Qualifier &= ~IEQUALIFIER_LCOMMAND;
			}

			text = "Left Amiga";
			code = 0x66;
			break;
		}

		case 0xffec: // Right Amiga
		{
			if ( km->km_Down )
			{
				cfg->NetRead_Qualifier |= IEQUALIFIER_RCOMMAND;
			}
			else
			{
				cfg->NetRead_Qualifier &= ~IEQUALIFIER_RCOMMAND;
			}

			text = "Right Amiga";
			code = 0x67;
			break;
		}

		case 0xffff: // Delete
		{
			text = "Delete";
			code = 0x46;
			break;
		}

		default:
		{
			// I have seen a 000000a4 key

// IExec->DebugPrintF( "Key Event: 0x%08lx\n", km->km_Key );

			rawkey[0] = km->km_Key;
			rawkey[1] = 0;

			if ( IKeymap->MapANSI( (APTR) rawkey, strlen( rawkey ), buf, sizeof( buf ), NULL ) <= 0 )
			{
				discarded = TRUE;
			}

			text = "";
			code = buf[0];
		}
	}

	if ( ! discarded )
	{
		if ( km->km_Down == 0 )
		{
			code |= IECODE_UP_PREFIX;
		}

		if ((( IEQUALIFIER_CONTROL | IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND ) & cfg->NetRead_Qualifier )
		==	(( IEQUALIFIER_CONTROL | IEQUALIFIER_LCOMMAND | IEQUALIFIER_RCOMMAND )))
		{
			ISocket->CloseSocket( cfg->NetRead_ClientSocket );
			ISocket->CloseSocket( cfg->NetSend_ClientSocket );
			IDOS->Delay( 2 );
			IExec->ColdReboot();
		}

		if ((( IEQUALIFIER_CONTROL | IEQUALIFIER_LALT | IEQUALIFIER_RALT ) & cfg->NetRead_Qualifier )
		==	(( IEQUALIFIER_CONTROL | IEQUALIFIER_LALT | IEQUALIFIER_RALT )))
		{
			ISocket->CloseSocket( cfg->NetRead_ClientSocket );
			ISocket->CloseSocket( cfg->NetSend_ClientSocket );
			IDOS->Delay( 2 );
			IExec->IceColdReboot();
		}

		memset( & Event, 0, sizeof( Event ));

		Event.ie_Class			= IECLASS_RAWKEY;
		Event.ie_Qualifier		= cfg->NetRead_Qualifier;
		Event.ie_Code			= code;
		Event.ie_Prev1DownCode	= cfg->NetRead_KeyboardPrevCode[0];
		Event.ie_Prev1DownQual	= cfg->NetRead_KeyboardPrevQual[0];
		Event.ie_Prev2DownCode	= cfg->NetRead_KeyboardPrevCode[1];
		Event.ie_Prev2DownQual	= cfg->NetRead_KeyboardPrevQual[1];

		IOReq = cfg->NetRead_InputIOReq;

		IOReq->io_Command		= IND_ADDEVENT;
		IOReq->io_Data			= & Event;
		IOReq->io_Length		= sizeof( Event );
		IOReq->io_Flags			= 0;

		IExec->DoIO( (APTR) IOReq );

		if ( km->km_Down == 0 )
		{
			if (( code < KEYCODE_FIRSTQUALIFIER ) || ( code > KEYCODE_LASTQUALIFIER ))
			{
				// Update down key history fields
				cfg->NetRead_KeyboardPrevCode[1] = cfg->NetRead_KeyboardPrevCode[0];
				cfg->NetRead_KeyboardPrevCode[0] = Event.ie_Code;
				cfg->NetRead_KeyboardPrevQual[1] = cfg->NetRead_KeyboardPrevQual[0];
				cfg->NetRead_KeyboardPrevQual[0] = Event.ie_Qualifier;
			}
		}
	}

	// --

{
	struct CommandKey *msg;

	msg = myCalloc( sizeof( struct CommandKey ) );

	if ( msg )
	{
		msg->ck_Command = CMD_Key;

		// --

		if ( ! discarded )
		{
			snprintf( msg->ck_Client, 64, "Amiga: $%02X, Client: $%04lX : %s : %s",
				code,
				km->km_Key, 
				( km->km_Down ) ? "Down" : "Up  ",
				text
			);

			snprintf( msg->ck_Amiga, 64, "%s %s %s %s %s %s %s %s",
				( cfg->NetRead_Qualifier & IEQUALIFIER_NUMERICPAD	) ? "NUMPAD" : "------",
				( cfg->NetRead_Qualifier & IEQUALIFIER_CONTROL		) ? "CTRL"   : "----",
				( cfg->NetRead_Qualifier & IEQUALIFIER_RCOMMAND		) ? "RAMIGA" : "------",
				( cfg->NetRead_Qualifier & IEQUALIFIER_LCOMMAND		) ? "LAMIGA" : "------",
				( cfg->NetRead_Qualifier & IEQUALIFIER_RALT			) ? "RALT"   : "----",
				( cfg->NetRead_Qualifier & IEQUALIFIER_LALT			) ? "LALT"   : "----",
				( cfg->NetRead_Qualifier & IEQUALIFIER_RSHIFT		) ? "RSHIFT" : "------",
				( cfg->NetRead_Qualifier & IEQUALIFIER_LSHIFT		) ? "LSHIFT" : "------"
			);
		}
		else
		{
			snprintf( msg->ck_Client, 64, "Amiga: $--, Client: $%04lX : %s - %s",
				km->km_Key, 
				( km->km_Down ) ? "Down" : "Up",
				text
			);

			snprintf(  msg->ck_Amiga, 64, "Discarded" );
		}

		msg->ck_Client[63] = 0;
		msg->ck_Amiga[63] = 0;

		// --

		IExec->PutMsg( CmdMsgPort, & msg->ck_Message );
	}
}

	// --

	cfg->NetRead_Qualifier &= ~IEQUALIFIER_NUMERICPAD;

	error = FALSE;

bailout:

	return( error );
}

// --
