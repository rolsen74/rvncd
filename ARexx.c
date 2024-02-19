 
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

static Object *			myARexxObject	= NULL;
static struct Hook		myARexxHook;
uint32					ARexxBit		= 0;
struct Library *		ARexxBase		= NULL;
Class *					ARexxClass		= NULL;

static struct Config *	ActiveConfig	= NULL;

// --

static uint32	ARexxCallBack(			struct Hook *h , Object *o, struct RexxMsg *msg );
static void		ARexx_StartServer(		struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_StopServer(		struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_RestartServer(	struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_ServerStatus(		struct ARexxCmd *ac, struct RexxMsg *msg );
static void		ARexx_Quit(				struct ARexxCmd *ac, struct RexxMsg *msg );
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
{	    "STARTSERVER",	1,	ARexx_StartServer,			NULL,		0,	NULL, 0, 0, NULL },
{	     "STOPSERVER",	2,	ARexx_StopServer,			NULL,		0,	NULL, 0, 0, NULL },
{	  "RESTARTSERVER",	3,	ARexx_RestartServer,		NULL,		0,	NULL, 0, 0, NULL },
{	   "SERVERSTATUS",	4,	ARexx_ServerStatus,			NULL,		0,	NULL, 0, 0, NULL },
{	           "QUIT",	5,	ARexx_Quit,					NULL,		0,	NULL, 0, 0, NULL },
{	     "LOADCONFIG",	6,	ARexx_LoadConfig,			"FILE/F",	0,	NULL, 0, 0, NULL },
{	     "SAVECONFIG",	7,	ARexx_SaveConfig,			NULL,		0,	NULL, 0, 0, NULL },
{	   "SAVEASCONFIG",	8,	ARexx_SaveASConfig,			"FILE/F",	0,	NULL, 0, 0, NULL },
{ "SAVEDEFAULTCONFIG",	9,	ARexx_SaveDefaultConfig,	NULL,		0,	NULL, 0, 0, NULL },
{	           "HELP", 10,	ARexx_Help,					NULL,		0,	NULL, 0, 0, NULL },
{	NULL,				0,	NULL,						NULL,		0,	NULL, 0, 0, NULL },
};

// --

int ARexx_Setup( struct Config *cfg )
{
uint32 err;
int error;

	// --

	error = FALSE;

	ARexxBase = (APTR) IIntuition->OpenClass( "arexx.class", 53, & ARexxClass );

	if ( ARexxBase == NULL )
	{
		cfg->cfg_ProgramDisableARexx = TRUE;
		Log_PrintF( cfg, LOGTYPE_Warning, "Error opening ARexx class, Disabling ARexx support" );
		goto bailout;
	}

	// --

	error = TRUE;

	ActiveConfig = cfg;

	myARexxHook.h_Entry		= (HOOKFUNC) ARexxCallBack;
	myARexxHook.h_SubEntry	= NULL;
	myARexxHook.h_Data		= NULL;

	// --

	err = 0;

	myARexxObject = IIntuition->NewObject( ARexxClass, NULL,
		AREXX_ReplyHook, & myARexxHook,
		AREXX_ErrorCode, & err,
		AREXX_HostName, "RVNCD",
		AREXX_Commands, Commands,
		AREXX_NoSlot, TRUE,
		TAG_END
	);

	if ( myARexxObject == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error creating ARexx Object (%ld)", err );
		goto bailout;
	}

	IIntuition->GetAttrs( myARexxObject,
		AREXX_SigMask, & ARexxBit,
		TAG_END
	);

	// --

	error = FALSE;

bailout:

	return( error );
}

// --

void ARexx_Cleanup( struct Config *cfg UNUSED )
{
	if ( myARexxObject )
	{
		IIntuition->DisposeObject( myARexxObject );
		myARexxObject = NULL;
		ARexxBit = 0;
	}

	if ( ARexxBase )
	{
		IIntuition->CloseClass( (APTR) ARexxBase );
		ARexxBase = NULL;
	}
}

// --

void ARexx_Handle( struct Config *cfg UNUSED )
{
	IIntuition->IDoMethod( myARexxObject, AM_HANDLEEVENT );
}

// --

static uint32 ARexxCallBack( struct Hook *h UNUSED, Object *o UNUSED, struct RexxMsg *msg UNUSED )
{
	return( 0 );
}

// --

static void ARexx_StartServer( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
int err;

	err = StartServer( ActiveConfig );

	if ( err )
	{
		ac->ac_Result = "Failed";
	}
	else
	{
		ac->ac_Result = "Successful";
	}
};

// --

static void ARexx_StopServer( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
int err;

	err = StopServer( ActiveConfig );

	if ( err )
	{
		ac->ac_Result = "Failed";
	}
	else
	{
		ac->ac_Result = "Successful";
	}
};

// --

static void ARexx_RestartServer( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
int err;

	err = RestartServer( ActiveConfig );

	if ( err )
	{
		ac->ac_Result = "Failed";
	}
	else
	{
		ac->ac_Result = "Successful";
	}
};

// --

static void ARexx_ServerStatus( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
char *txt;

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
};

// --

static void ARexx_Quit( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
	ac->ac_Result = "Exiting Program";

	ProgramRunning = FALSE;

	IExec->Signal( ProgramTask, SIGBREAKF_CTRL_C );
};

// --

static void ARexx_LoadConfig( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
struct CommandMessage *cmsg;
char *file;
char *str;
int err;

	file = (APTR) ac->ac_ArgList[0];
	str = "Failed";

	if ( file )
	{
		err = Config_ParseFile( ActiveConfig, file );

		if ( err )
		{
			Log_PrintF( ActiveConfig, LOGTYPE_Error, "Failed to load '%s' Config file", IDOS->FilePart( file ));
		}
		else
		{
			Log_PrintF( ActiveConfig, LOGTYPE_Info, "Config file '%s' loaded", IDOS->FilePart( file ) );

			str = "Successful";

			// --

			cmsg = myCalloc( sizeof( struct CommandMessage ) );

			if ( cmsg )
			{
				cmsg->cm_Command = CMD_RefreshGUI;

				IExec->PutMsg( CmdMsgPort, & cmsg->cm_Message );
			}

		}
	}

	ac->ac_Result = str;
}

// --

static void ARexx_SaveConfig( struct ARexxCmd *ac UNUSED, struct RexxMsg *msg UNUSED )
{
	Config_Save( ActiveConfig, ActiveConfig->cfg_Config_FileName );
}

// --

static void ARexx_SaveASConfig( struct ARexxCmd *ac, struct RexxMsg *msg UNUSED )
{
char *file;

	file = (APTR) ac->ac_ArgList[0];

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
char *str =	

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
