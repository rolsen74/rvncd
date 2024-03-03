 
/*
 * Copyright (c) 2023-2024 Rene W. Olsen < renewolsen @ gmail . com >
 *
 * This software is released under the GNU General Public License, version 3.
 * For the full text of the license, please visit:
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * You can also find a copy of the license in the LICENSE file included with this software.
 */

// --

#include "RVNCd.h"

// --

/*
** Purpose:
** - Server handles a Mouse event from Client
**
** Returns:
** - True and the socket will be closed
** - False and we continue
*/

#pragma pack(1)

struct MouseMessage
{
	uint8	mm_Type;
	uint8	mm_Buttons;
	int16	mm_X;
	int16	mm_Y;
};

#pragma pack(0)

int VNC_Mouse( struct Config *cfg )
{
struct MouseMessage *mm;
struct IEPointerPixel Pointer;
struct SocketIFace *ISocket;
struct InputEvent Event;
struct IOStdReq *IOReq;
uint32 pos;
int error;
int code;
int size;
int rc;
int v1;
int v2;

	mm = cfg->NetRead_ReadBuffer;

	error = TRUE;

	ISocket = cfg->NetRead_ISocket;

	size = sizeof( struct MouseMessage );

	rc = ISocket->recv( cfg->NetRead_ClientSocket, mm, size, MSG_WAITALL );

	if ( rc == -1 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Failed to read data (%d)", ISocket->Errno() );
		}

		Log_PrintF( cfg, LOGTYPE_Error, "Failed to read data '%s' (%ld)", myStrError( ISocket->Errno() ), ISocket->Errno() );
		goto bailout;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		cfg->cfg_ServerRunning = FALSE;

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}
		goto bailout;
	}

	cfg->SessionStatus.si_Read_Bytes += rc;

	if (( mm->mm_Type != 5 ) || ( rc != size ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid data (%d != %d)", rc, size );
		goto bailout;
	}

	// -- Check if our Active screen is Front Most

	#if 0
	if ( cfg->GfxRead_Screen_Adr != IntuitionBase->FirstScreen )
	{
		error = FALSE;
		goto bailout;
	}
	#endif

	// --

	IOReq = cfg->NetRead_InputIOReq;

	// -- Handle Mouse Position

	pos = (( mm->mm_Y << 16 ) | ( mm->mm_X ));

	if ( cfg->NetRead_OldMousePos != pos )
	{
		Pointer.iepp_Screen		= cfg->GfxRead_Screen_Adr;
		Pointer.iepp_Position.X	= mm->mm_X;	// - xadjust;
		Pointer.iepp_Position.Y	= mm->mm_Y;	// - yadjust;

		if ( cfg->GfxRead_Screen_ViewMode == VIEWMODE_View )
		{
			if ( cfg->GfxRead_Screen_Adr->ViewPort.DxOffset < 0 )
			{
				Pointer.iepp_Position.X -= cfg->GfxRead_Screen_Adr->ViewPort.DxOffset;
			}

			if ( cfg->GfxRead_Screen_Adr->ViewPort.DyOffset < 0 )
			{
				Pointer.iepp_Position.Y -= cfg->GfxRead_Screen_Adr->ViewPort.DyOffset;
			}
		}

		memset( & Event, 0, sizeof( Event ));

		Event.ie_Class			= IECLASS_NEWPOINTERPOS;
		Event.ie_SubClass		= IESUBCLASS_PIXEL;
		Event.ie_Code			= IECODE_NOBUTTON;
		Event.ie_Qualifier		= cfg->NetRead_Qualifier;
		Event.ie_EventAddress	= & Pointer;

		IOReq->io_Command		= IND_WRITEEVENT;
		IOReq->io_Data			= & Event;
		IOReq->io_Length		= sizeof( Event );
		IOReq->io_Flags			= 0;

		IExec->DoIO( (APTR) IOReq );
	
		cfg->NetRead_OldMousePos = pos;
	}

	// -- Handle Mouse Buttons

	if ( cfg->NetRead_OldMouseButtons != ( mm->mm_Buttons & 31 ))
	{
		//  1 = Left
		//  2 = Middle
		//  4 = Right
		//  8 = scroll up
		// 16 = scroll down

	#ifdef VAL_TEST
	if ( TEST_VAL )
	{
		Log_PrintF( cfg, LOGTYPE_Info, "Mouse Button : New %04lx, Old %04lx", mm->mm_Buttons, cfg->NetRead_OldMouseButtons );
	}
	#endif

		// -- Left Mouse Button

		v1 = mm->mm_Buttons & 1;
		v2 = cfg->NetRead_OldMouseButtons & 1;

		if ( v1 )
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_LEFTBUTTON;
			code = IECODE_LBUTTON;
		}
		else
		{
			cfg->NetRead_Qualifier &= ~IEQUALIFIER_LEFTBUTTON;
			code = IECODE_LBUTTON | IECODE_UP_PREFIX;
		}

		if ( v1 != v2 )
		{
			memset( & Event, 0, sizeof( Event ));

			Event.ie_Class			= IECLASS_RAWMOUSE;
			Event.ie_Qualifier		= cfg->NetRead_Qualifier;
			Event.ie_Code			= code;

			IOReq->io_Command		= IND_WRITEEVENT;
			IOReq->io_Data			= & Event;
			IOReq->io_Length		= sizeof( Event );
			IOReq->io_Flags			= 0;

			IExec->DoIO( (APTR) IOReq );
		}

		// -- Middle Mouse Button

		v1 = mm->mm_Buttons & 2;
		v2 = cfg->NetRead_OldMouseButtons & 2;

		if ( v1 )
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_MIDBUTTON;
			code = IECODE_MBUTTON;
		}
		else
		{
			cfg->NetRead_Qualifier &= ~IEQUALIFIER_MIDBUTTON;
			code = IECODE_MBUTTON | IECODE_UP_PREFIX;
		}

		if ( v1 != v2 )
		{
			memset( & Event, 0, sizeof( Event ));

			Event.ie_Class			= IECLASS_RAWMOUSE;
			Event.ie_Qualifier		= cfg->NetRead_Qualifier;
			Event.ie_Code			= code;

			IOReq->io_Command		= IND_WRITEEVENT;
			IOReq->io_Data			= & Event;
			IOReq->io_Length		= sizeof( Event );
			IOReq->io_Flags			= 0;

			IExec->DoIO( (APTR) IOReq );
		}

		// -- Right Mouse Button

		v1 = mm->mm_Buttons & 4;
		v2 = cfg->NetRead_OldMouseButtons & 4;

		if ( v1 )
		{
			cfg->NetRead_Qualifier |= IEQUALIFIER_RBUTTON;
			code = IECODE_RBUTTON;
		}
		else
		{
			cfg->NetRead_Qualifier &= ~IEQUALIFIER_RBUTTON;
			code = IECODE_RBUTTON | IECODE_UP_PREFIX;
		}

		if ( v1 != v2 )
		{
			memset( & Event, 0, sizeof( Event ));

			Event.ie_Class			= IECLASS_RAWMOUSE;
			Event.ie_Qualifier		= cfg->NetRead_Qualifier;
			Event.ie_Code			= code;

			IOReq->io_Command		= IND_WRITEEVENT;
			IOReq->io_Data			= & Event;
			IOReq->io_Length		= sizeof( Event );
			IOReq->io_Flags			= 0;

			IExec->DoIO( (APTR) IOReq );
		}

		// -- Scroll Wheel

		if ( mm->mm_Buttons & ( 8 | 16 ))
		{
			memset( & Event, 0, sizeof( Event ));

			Event.ie_Class			= IECLASS_MOUSEWHEEL;
			Event.ie_Qualifier		= cfg->NetRead_Qualifier;
			Event.ie_Y				= ( mm->mm_Buttons & 8 ) ? -1 : 1 ;

			IOReq->io_Command		= IND_WRITEEVENT;
			IOReq->io_Data			= & Event;
			IOReq->io_Length		= sizeof( Event );
			IOReq->io_Flags			= 0;

			IExec->DoIO( (APTR) IOReq );
		}

		cfg->NetRead_OldMouseButtons = ( mm->mm_Buttons & 31 );
	}

	error = FALSE;

bailout:

	return( error );
}

// --
