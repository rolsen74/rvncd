
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifdef HAVE_APPLIB

// --

#include "rVNCd.h"

// --

U32							AppID			= 0;
U32							AppBit			= 0;
static struct MsgPort *		AppPort			= NULL;
struct Library *			AppBase			= NULL;
struct ApplicationIFace * 	IApplication	= NULL;


// --

S32 App_Init( struct Config *cfg, struct WBStartup *wbmsg )
{
S32 retval;

	retval = FALSE;

	AppBase = OpenLibrary( "application.library", 53 );
	IApplication = (PTR) GetInterface( AppBase, "application", 2, NULL );

	if ( ! IApplication )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening application library (v53)" );
		goto bailout;
	}

	AppID = RegisterApplication( "rVNCd",
		REGAPP_AppNotifications, TRUE,
		REGAPP_WBStartup, (U32) wbmsg,
		REGAPP_LoadPrefs, FALSE,
		REGAPP_SavePrefs, FALSE,
		TAG_END
	);

	if ( ! AppID )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to register application" );
		goto bailout;
	}

	GetApplicationAttrs( AppID,
		APPATTR_Port, & AppPort,
		TAG_END
	);

	if ( ! AppPort )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error failed to obtain App MsgPort" );
		goto bailout;
	}

	AppBit = 1UL << AppPort->mp_SigBit;

	retval = TRUE;

bailout:

	return( retval );
}

// --

void App_Free( void )
{
	if ( AppID )
	{
		UnregisterApplication( AppID, TAG_END );
		AppPort = NULL;
		AppBit = 0;
		AppID = 0;
	}

	if ( IApplication )
	{
		DropInterface( (PTR) IApplication );
		IApplication = NULL;
	}

	if ( AppBase )
	{
		CloseLibrary( AppBase );
		AppBase = NULL;
	}
}

// --

void App_Handle( struct Config *cfg UNUSED )
{
struct ApplicationMsg *msg;

	while(( msg = (PTR) GetMsg( AppPort )))
	{
		switch( msg->type )
		{
			case APPLIBMT_Quit:
			{
				Func_Quit( cfg );
				break;
			}

			case APPLIBMT_ForceQuit:
			{
				Func_ForceQuit( cfg );
				break;
			}

			case APPLIBMT_Hide:
			{
				SHELLBUF_PRINTF( "AppLib: Hide\n" );
				break;
			}

			case APPLIBMT_Unhide:
			{
				SHELLBUF_PRINTF( "AppLib: Unhide\n" );
				break;
			}

			case APPLIBMT_ToFront:
			{
				SHELLBUF_PRINTF( "AppLib: ToFront\n" );
				break;
			}

			case APPLIBMT_OpenPrefs:
			{
				SHELLBUF_PRINTF( "AppLib: OpenPrefs\n" );
				break;
			}

			default:
			{
				break;
			}
		}

		ReplyMsg( (PTR) msg );
	}
}

// --

#endif // HAVE_APPLIB
