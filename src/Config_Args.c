
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32	DoVerboseLocked	= FALSE;
S32	DoVerbose = 0;

// --

enum
{
	RC_Error = -1,
	RC_Done = 0,
	RC_Okay = 1,
};

static void Config_Usage( void )
{
	SHELLBUF_PRINTF( "\n" );
	SHELLBUF_PRINTF( "Usage: rVNCd [options]\n" );
	SHELLBUF_PRINTF( "\n" );
	SHELLBUF_PRINTF( "Options:\n" );
	SHELLBUF_PRINTF( "\n" );
	SHELLBUF_PRINTF( "  -v, --verbose        Enable verbose mode\n" );
	SHELLBUF_PRINTF( "  -h, --help           Display this help message\n" );
	SHELLBUF_PRINTF( "  -c, --config <file>  Specify the config file\n" );
	SHELLBUF_PRINTF( "  -p, --port <port>    Specify the network port to listen on\n");

	#ifdef __HAVE__RVNCD__GUI__
	SHELLBUF_PRINTF( "      --nogui          Disables GUI.\n");	
	#endif // __HAVE__RVNCD__GUI__

	#ifdef HAVE_AREXX
	SHELLBUF_PRINTF( "      --noarexx        Disables ARexx.\n");	
	#endif // HAVE_AREXX

	#ifdef HAVE_CXBROKER
	SHELLBUF_PRINTF( "      --nocxbroker     Disables Commodities Broker.\n");	
	#endif // HAVE_CXBROKER

	SHELLBUF_PRINTF( "\n" );
}

// --

static S32 ArgFunc_Help( struct Config *cfg UNUSED, STR arg UNUSED )
{
	Config_Usage();

	return( RC_Done );
}

// --

static S32 ArgFunc_Verbose0( struct Config *cfg UNUSED, STR arg UNUSED )
{
	DoVerboseLocked	= TRUE;
	DoVerbose = 0;

//	PrintProgramHeader();

	return( RC_Okay );
}

// --

static S32 ArgFunc_Verbose1( struct Config *cfg UNUSED, STR arg UNUSED )
{
	DoVerboseLocked	= TRUE;
	DoVerbose = 1;

	PrintProgramHeader();

	return( RC_Okay );
}

// --

static S32 ArgFunc_Verbose2( struct Config *cfg UNUSED, STR arg UNUSED )
{
	DoVerboseLocked	= TRUE;
	DoVerbose = 2;

	PrintProgramHeader();

	return( RC_Okay );
}

// --

static S32 ArgFunc_Verbose3( struct Config *cfg UNUSED, STR arg UNUSED )
{
	DoVerboseLocked	= TRUE;
	DoVerbose = 3;

	PrintProgramHeader();

	return( RC_Okay );
}

// --

static S32 ArgFunc_Config( struct Config *cfg, STR arg )
{
S32 rc;

	rc = RC_Error;

	if ( cfg->cfg_Startup_Config_Filename )
	{
		mem_Free( cfg->cfg_Startup_Config_Filename );
	}

	cfg->cfg_Startup_Config_Filename = mem_Strdup( arg );

	if ( cfg->cfg_Startup_Config_Filename == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
		goto bailout;
	}

	rc = RC_Okay;

bailout:

	return( rc );
}

// --

static S32 ArgFunc_Port( struct Config *cfg, STR arg )
{
S32 val;
S32 rc;

	rc = RC_Error;

	val = atoi( arg );

	if (( val < 0 ) || ( 65535 < val ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Invalid Port number (%ld)", val );
		goto bailout;
	}

	cfg->cfg_Disk_Settings.Port = atoi( arg );

	rc = RC_Okay;

bailout:

	return( rc );
}

// --

#ifdef __HAVE__RVNCD__GUI__

static S32 ArgFunc_NoGUI( struct Config *cfg, STR arg UNUSED )
{
	cfg->cfg_ProgramDisableGUI = TRUE;

	return( RC_Okay );
}

#endif // __HAVE__RVNCD__GUI__

// --

#ifdef HAVE_AREXX

static S32 ArgFunc_NoARexx( struct Config *cfg, STR arg UNUSED )
{
	cfg->cfg_ProgramDisableARexx = TRUE;

	return( RC_Okay );
}

#endif // HAVE_AREXX

// --

#ifdef HAVE_CXBROKER

static S32 ArgFunc_NoBroker( struct Config *cfg, STR arg UNUSED )
{
	cfg->cfg_ProgramDisableCxBroker = TRUE;

	return( RC_Okay );
}

#endif // HAVE_CXBROKER

// --

struct myArgs
{
	STR		Name1;
	STR		Name2;
	S32		Early;
	S32		Options;
	S32		(*Function)( struct Config *cfg, STR arg );
};

// -- Early Options before Config file
struct myArgs myOptions[] =
{
{ "-h", "--help",		1, 0, ArgFunc_Help },
{ "-v0", NULL,			1, 0, ArgFunc_Verbose0 },	// Off
{ "-v1", NULL,			1, 0, ArgFunc_Verbose1 },	// Normal (Default)
{ "-v", "--verbose",	1, 0, ArgFunc_Verbose2 },	// Extra
{ "-v2", NULL,			1, 0, ArgFunc_Verbose2 },	// Extra
{ "-v3", NULL,			1, 0, ArgFunc_Verbose3 },	// Full
{ "-c", "--config",		1, 1, ArgFunc_Config },
{ "-p", "--port",		0, 1, ArgFunc_Port },

#ifdef __HAVE__RVNCD__GUI__
{ NULL, "--nogui",		0, 0, ArgFunc_NoGUI },
#endif // __HAVE__RVNCD__GUI__

#ifdef HAVE_AREXX
{ NULL, "--noarexx",	0, 0, ArgFunc_NoARexx },
#endif // HAVE_AREXX

#ifdef HAVE_CXBROKER
{ NULL, "--nocxbroker",	0, 0, ArgFunc_NoBroker },
#endif // HAVE_CXBROKER

{ NULL, NULL, 0, 0, NULL }
};

// --

S32 Config_ParseArgs( struct Config *cfg, S32 argc, STR *argv, S32 early )
{
S32 error;
S32 stat;
S32 pos;
S32 cnt;

	error = TRUE;

	for( cnt=1 ; cnt<argc ; cnt++ )
	{
		pos = 0;

		while( TRUE )
		{
			// End of List?
			if (( ! myOptions[pos].Name1 )
			&&	( ! myOptions[pos].Name2 ))
			{
				break;
			}

			// Found Argument?
			if ((( myOptions[pos].Name1 ) && ( ! strcmp( myOptions[pos].Name1, argv[cnt] )))
			||  (( myOptions[pos].Name2 ) && ( ! strcmp( myOptions[pos].Name2, argv[cnt] ))))
			{
				if ( cnt+myOptions[pos].Options >= argc )
				{
					SHELLBUF_PRINTF( "\nMissing argument for %s\n", argv[cnt] );
					Config_Usage();
					goto bailout;
				}					

				if (( early ) == ( myOptions[pos].Early ))
				{
					stat = myOptions[pos].Function( cfg, argv[cnt+1] );

					// Error or Stop
					if ( stat != RC_Okay )
					{
						goto bailout;
					}
				}

				cnt += myOptions[pos].Options;
				break;
			}

			pos++;
		}

		if (( ! myOptions[pos].Name1 )
		&&	( ! myOptions[pos].Name2 ))
		{
			SHELLBUF_PRINTF( "\nUnknown argument : %s\n", argv[cnt] );
			Config_Usage();
			goto bailout;
		}
	}

	error = FALSE;

bailout:

	return( error );
}

// --
