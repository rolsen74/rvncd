 
/*
 * Copyright (c) 2023-2024 Rene W. Olsen < renewolsen @ gmail . com >
 *
 * This software is released under the GNU General Public License, version 3.
 * For the full text of the license, please visit:
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * You can also find a copy of the license in the LICENSE file included with this software.
 */

/*
** This file holds functions for
** parsing config file Log section
*/

// --

#include "RVNCd.h"

// --

static int myUserDisconnect( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Log] UserDisconnect missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_LogUserDisconnect = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Log] UserDisconnect: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myUserConnect( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Log] UserConnect missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_LogUserConnect = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Log] UserConnect: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myEnable( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Log] Enable missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_LogEnable = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Log] Enable: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myProgramStart( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Log] ProgramStart missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_LogProgramStart = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Log] ProgramStart: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myProgramStop( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Log] ProgramStop missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_LogProgramStop = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Log] ProgramStop: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myServerStart( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Log] ServerStart missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_LogServerStart = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Log] ServerStart: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myServerStop( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Log] ServerStop missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_LogServerStop = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Log] ServerStop: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myFile( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
char *name;
int error;
int len;

	error = TRUE;

	if ( arglen <= 0 )
	{
		IExec->DebugPrintF( "Missing Argument\n" );
		goto bailout;
	}

	name = Config_CopyString( & buf[argpos] );

	if ( name == NULL )
	{
		IExec->DebugPrintF( "Error copying string\n" );
		goto bailout;
	}

	// -- Remove .txt

	len = strlen( name );

	if ( len > 4 )
	{
		if ( stricmp( & name[len-4], ".txt" ) == 0 )
		{
			name[len-4] = 0;
		}
	}

	// --

	if ( cfg->cfg_LogFileName )
	{
		myFree( cfg->cfg_LogFileName );
	}

	cfg->cfg_LogFileName = name;

	if ( DoVerbose )
	{
		printf( "[Log] LogFile: '%s'\n", name );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myInfos( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Log] Infos missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ProgramLogInfos = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Log] Infos: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myWarnings( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Log] Warnings missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ProgramLogWarnings = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Log] Warnings: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myErrors( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Log] Errors missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ProgramLogErrors = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Log] Errors: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

struct ParseCommand ParseLogCmds[] =
{
{ 5,  "Infos", myInfos },
{ 6,  "Errors", myErrors },
{ 6,  "Enable", myEnable },
{ 7,  "LogFile", myFile },
{ 8,  "Warnings", myWarnings },
{ 10, "ServerStop", myServerStop },
{ 11, "ServerStart", myServerStart },
{ 11, "ProgramStop", myProgramStop },
{ 11, "UserConnect", myUserConnect },
{ 12, "ProgramStart", myProgramStart },
{ 14, "UserDisconnect", myUserDisconnect },
{ 0, NULL, NULL }
};

// --

