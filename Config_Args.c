 
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

int		DoVerbose		= FALSE;
int		DoMugWall		= FALSE;

// --

enum
{
	RC_Error = -1,
	RC_Done = 0,
	RC_Okay = 1,
};

static void Config_Usage( void )
{
	printf( "\n" );
	printf( "Usage: RVNCd [options]\n" );
	printf( "\n" );
	printf( "Options:\n" );
	printf( "\n" );
	printf( "  -v, --verbose        Enable verbose mode\n" );
	printf( "  -h, --help           Display this help message\n" );
	printf( "  -c, --config <file>  Specify the config file\n" );
	printf( "  -p, --port <port>    Specify the network port to listen on\n");	
	printf( "      --mugwall        Enables Memory MugWalls.\n");	
	printf( "      --nogui          Disables GUI.\n");	
	printf( "      --noarexx        Disables ARexx.\n");	
	printf( "      --nocxbroker     Disables Commodities Broker.\n");	
	printf( "\n" );
}

// --

static int ArgFunc_Help( struct Config *cfg UNUSED, char *arg UNUSED )
{
	Config_Usage();

	return( RC_Done );
}

// --

static int ArgFunc_Nop( struct Config *cfg UNUSED, char *arg UNUSED )
{
	// --mugwall allready set
	return( RC_Okay );
}

// --

static int ArgFunc_Verbose( struct Config *cfg UNUSED, char *arg UNUSED )
{
	DoVerbose = TRUE;

	PrintProgramHeader();

	return( RC_Okay );
}

// --

static int ArgFunc_Config( struct Config *cfg, char *arg )
{
int rc;

	rc = RC_Error;

	if ( cfg->cfg_Startup_Config_FileName )
	{
		myFree( cfg->cfg_Startup_Config_FileName );
	}

	cfg->cfg_Startup_Config_FileName = myStrdup( arg );

	if ( cfg->cfg_Startup_Config_FileName == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
		goto bailout;
	}

	rc = RC_Okay;

bailout:

	return( rc );
}

// --

static int ArgFunc_Port( struct Config *cfg, char *arg )
{
int val;
int rc;

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

static int ArgFunc_NoGUI( struct Config *cfg, char *arg UNUSED )
{
	cfg->cfg_ProgramDisableGUI = TRUE;

	return( RC_Okay );
}

// --

static int ArgFunc_NoARexx( struct Config *cfg, char *arg UNUSED )
{
	cfg->cfg_ProgramDisableARexx = TRUE;

	return( RC_Okay );
}

// --

static int ArgFunc_NoBroker( struct Config *cfg, char *arg UNUSED )
{
	cfg->cfg_ProgramDisableCxBroker = TRUE;

	return( RC_Okay );
}

// --

struct myArgs
{
	char *	Name1;
	char *	Name2;
	int		Early;
	int		Options;
	int		(*Function)( struct Config *cfg, char *arg );
};

// -- Early Options before Config file
struct myArgs myOptions[] =
{
{ "-h", "--help",		1, 0, ArgFunc_Help },
{ "-v", "--verbose",	1, 0, ArgFunc_Verbose },
{ "-c", "--config",		1, 1, ArgFunc_Config },
{ "-p", "--port",		0, 1, ArgFunc_Port },
{ NULL, "--mugwall",	1, 0, ArgFunc_Nop },
#ifdef VAL_TEST
{ NULL, "--mousetest",	1, 0, ArgFunc_Nop },
#endif
{ NULL, "--nogui",		0, 0, ArgFunc_NoGUI },
{ NULL, "--noarexx",	0, 0, ArgFunc_NoARexx },
{ NULL, "--nocxbroker",	0, 0, ArgFunc_NoBroker },
{ NULL, NULL, 0, 0, NULL }
};

// --

int Config_ParseArgs( struct Config *cfg, int argc, char **argv, int early )
{
int error;
int stat;
int pos;
int cnt;

	error = TRUE;

	for( cnt=1 ; cnt<argc ; cnt++ )
	{
		pos = 0;

		while( TRUE )
		{
			// End of List?
			if (( myOptions[pos].Name1 == NULL )
			&&	( myOptions[pos].Name2 == NULL ))
			{
				break;
			}

			// Found Argument?
			if ((( myOptions[pos].Name1 ) && ( strcmp( myOptions[pos].Name1, argv[cnt] ) == 0 ))
			||  (( myOptions[pos].Name2 ) && ( strcmp( myOptions[pos].Name2, argv[cnt] ) == 0 )))
			{
				if ( cnt+myOptions[pos].Options >= argc )
				{
					printf( "\nMissing argument for %s\n", argv[cnt] );
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

		if (( myOptions[pos].Name1 == NULL )
		&&	( myOptions[pos].Name2 == NULL ))
		{
			printf( "\nUnknown argument : %s\n", argv[cnt] );
			Config_Usage();
			goto bailout;
		}
	}

	error = FALSE;

bailout:

	return( error );
}

// --
