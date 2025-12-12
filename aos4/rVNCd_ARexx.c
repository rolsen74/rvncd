
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifdef HAVE_AREXX

// --

#include "rVNCd.h"

// --

static Object *			myARexxObject	= NULL;
static struct Hook		myARexxHook;
U32					ARexxBit		= 0;
struct Library *		ARexxBase		= NULL;
Class *					ARexxClass		= NULL;

static struct Config *	ActiveConfig	= NULL;

// --

static U32	ARexxCallBack(			struct Hook *h , Object *o, struct RexxMsg *msg );
static void		ARexx_StartServer(		struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_StopServer(		struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_RestartServer(	struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_ServerStatus(		struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_Quit(				struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_ForceQuit(		struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_LoadConfig(		struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_SaveConfig(		struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_SaveASConfig(		struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_SaveDefaultConfig(struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_Help(				struct ARexxCmd *ac, struct RexxMsg *msg );

// Ideas!!
// Users connected	.. 0-x .. if we expand it to a real sharing vnc server
// Disconnect user	.. disconnect a client if there is one (pjs)
// query user info	.. eg. ip address, connect time, online time, screen info

// CLIENTS
// QUERY # TYPE
// DISCONNECT #



static struct ARexxCmd Commands[] =
{
{		"STARTSERVER",	1,	ARexx_StartServer,			NULL,		0,	NULL, 0, 0, NULL },
{		 "STOPSERVER",	2,	ARexx_StopServer,			NULL,		0,	NULL, 0, 0, NULL },
{	  "RESTARTSERVER",	3,	ARexx_RestartServer,		NULL,		0,	NULL, 0, 0, NULL },
{	   "SERVERSTATUS",	4,	ARexx_ServerStatus,			NULL,		0,	NULL, 0, 0, NULL },
{		       "QUIT",	5,	ARexx_Quit,					NULL,		0,	NULL, 0, 0, NULL },
{		 "LOADCONFIG",	6,	ARexx_LoadConfig,			"FILE/F",	0,	NULL, 0, 0, NULL },
{		 "SAVECONFIG",	7,	ARexx_SaveConfig,			NULL,		0,	NULL, 0, 0, NULL },
{	   "SAVEASCONFIG",	8,	ARexx_SaveASConfig,			"FILE/F",	0,	NULL, 0, 0, NULL },
{ "SAVEDEFAULTCONFIG",	9,	ARexx_SaveDefaultConfig,	NULL,		0,	NULL, 0, 0, NULL },
{		       "HELP", 10,	ARexx_Help,					NULL,		0,	NULL, 0, 0, NULL },
{		  "FORCEQUIT", 11,	ARexx_ForceQuit,			NULL,		0,	NULL, 0, 0, NULL },
{                NULL,	0,	NULL,						NULL,		0,	NULL, 0, 0, NULL },
};

// --

S32 ARexx_Init( struct Config *cfg )
{
U32 err;
S32 retval;

	// --

	retval = FALSE;

	ARexxBase = (PTR) OpenClass( "arexx.class", 53, & ARexxClass );

	if ( ! ARexxBase )
	{
		cfg->cfg_ProgramDisableARexx = TRUE;
		Log_PrintF( cfg, LOGTYPE_Warning, "Error opening ARexx class, Disabling ARexx support" );
		goto bailout;
	}

	// --

	ActiveConfig = cfg;

	myARexxHook.h_Entry		= (HOOKFUNC) ARexxCallBack;
	myARexxHook.h_SubEntry	= NULL;
	myARexxHook.h_Data		= NULL;

	// --

	err = 0;

	myARexxObject = NewObject( ARexxClass, NULL,
		AREXX_ReplyHook, & myARexxHook,
		AREXX_ErrorCode, & err,
		AREXX_HostName, "rVNCd",
		AREXX_Commands, Commands,
		AREXX_NoSlot, TRUE,
		TAG_END
	);

	if ( myARexxObject == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating ARexx Object (%ld)", err );
		goto bailout;
	}

	GetAttrs( myARexxObject,
		AREXX_SigMask, & ARexxBit,
		TAG_END
	);

	// --

	retval = TRUE;

bailout:

	return( retval );
}

// --

void ARexx_Free( struct Config *cfg UNUSED )
{
	if ( myARexxObject )
	{
		DisposeObject( myARexxObject );
		myARexxObject = NULL;
		ARexxBit = 0;
	}

	if ( ARexxBase )
	{
		CloseClass( (PTR) ARexxBase );
		ARexxBase = NULL;
	}
}

// --

void ARexx_Handle( struct Config *cfg UNUSED )
{
	IDoMethod( myARexxObject, AM_HANDLEEVENT );
}

// --

static U32 ARexxCallBack( struct Hook *h UNUSED, Object *o UNUSED, struct RexxMsg *msg UNUSED )
{
	return( 0 );
}

// --

static void ARexx_StartServer( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
S32 err;

	err = StartServer( ActiveConfig );

	if ( err )
	{
		ac->ac_Result = "Failed";
	}
	else
	{
		ac->ac_Result = "Successful";
	}
}

// --

static void ARexx_StopServer( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
S32 err;

	err = StopServer( ActiveConfig );

	if ( err )
	{
		ac->ac_Result = "Failed";
	}
	else
	{
		ac->ac_Result = "Successful";
	}
}

// --

static void ARexx_RestartServer( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
S32 err;

	err = RestartServer( ActiveConfig );

	if ( err )
	{
		ac->ac_Result = "Failed";
	}
	else
	{
		ac->ac_Result = "Successful";
	}
}

// --

static void ARexx_ServerStatus( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
STR txt;

	/**/ if ( ActiveConfig->cfg_ServerStatus == PROCESS_Stopped )
	{
		txt = "Stopped";
	}
	else if ( ActiveConfig->cfg_ServerStatus == PROCESS_Starting )
	{
		txt = "Starting";
	}
	else if ( ActiveConfig->cfg_ServerStatus == PROCESS_Running )
	{
		txt = "Running";
	}
	else if ( ActiveConfig->cfg_ServerStatus == PROCESS_Stopping )
	{
		txt = "Shutting down";
	}
	else
	{
		txt = "Unknown";
	}

	ac->ac_Result = txt;
}

// --

static void ARexx_Quit( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
	ac->ac_Result = "Exiting Program";

	Func_Quit( ActiveConfig );
}

// --

static void ARexx_ForceQuit( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
	ac->ac_Result = "Exiting Program";

	Func_ForceQuit( ActiveConfig );
}

// --

static void ARexx_LoadConfig( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
struct CommandMessage *cmsg;
STR file;
STR str;
S32 err;

	file = (PTR) ac->ac_ArgList[0];
	str = "Failed";

	if ( file )
	{
		Config_Reset( ActiveConfig );
		err = Config_Read( ActiveConfig, file, TRUE );

		if ( err )
		{
			Log_PrintF( ActiveConfig, LOGTYPE_Error, "Failed to load '%s' Config file", FilePart( file ));
		}
		else
		{
			Log_PrintF( ActiveConfig, LOGTYPE_Info, "Config file '%s' loaded", FilePart( file ) );

			if ( ActiveGUI.Refresh_Settings )
			{
				ActiveGUI.Refresh_Settings( ActiveConfig );
			}

			if ( ActiveGUI.Check_Settings )
			{
				#ifdef DEBUG
				ActiveGUI.Check_Settings( ActiveConfig, __FILE__, __LINE__ );
				#else
				ActiveGUI.Check_Settings( ActiveConfig );
				#endif
			}

			str = "Successful";

			// --

			cmsg = mem_AllocClr( sizeof( struct CommandMessage ) );

			if ( cmsg )
			{
				cmsg->cm_Command = CMD_RefreshGUI;

				MsgPort_PutMsg( & CmdMsgPort, & cmsg->cm_Message );
			}

		}
	}

	ac->ac_Result = str;
}

// --

static void ARexx_SaveConfig( struct ARexxCmd *ac UNUSED, struct RexxMsg *msg UNUSED )
{
	Config_Save( ActiveConfig, ActiveConfig->cfg_Config_Filename );
}

// --

static void ARexx_SaveASConfig( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
STR file;

	file = (PTR) ac->ac_ArgList[0];

	if ( file )
	{
		Config_Save( ActiveConfig, file );
	}
}

// --

static void ARexx_SaveDefaultConfig( struct ARexxCmd *ac UNUSED, struct RexxMsg *msg UNUSED )
{
	Config_Save( ActiveConfig, NULL );
}

// --

static void ARexx_Help( struct ARexxCmd *ac UNUSED, struct RexxMsg *msg UNUSED )
{
STR str =	

	"STARTSERVER"
	" STOPSERVER"
	" RESTARTSERVER"
	" SERVERSTATUS"
	" QUIT"
	" LOADCONFIG"
	" SAVECONFIG"
	" SAVEASCONFIG"
	" SAVEDEFAULTCONFIG"
	" HELP"
	;

	ac->ac_Result = str;
}

// --

#endif // HAVE_AREXX
