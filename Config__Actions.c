 
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

static int myServerStartEnable( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] ServerStartEnable missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ActionsServerStartEnable = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Actions] ServerStartEnable: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myServerStopEnable( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] ServerStopEnable missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ActionsServerStopEnable = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Actions] ServerStopEnable: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myProgramStartEnable( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] ProgramStartEnable missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ActionsProgramStartEnable = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Actions] ProgramStartEnable: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myProgramStopEnable( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] ProgramStopEnable missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ActionsProgramStopEnable = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Actions] ProgramStopEnable: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myUserConnectEnable( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] UserConnectEnable missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ActionsUserConnectEnable = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Actions] UserConnectEnable: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myUserDisconnectEnable( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] UserDisconnectEnable missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ActionsUserDisconnectEnable = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Actions] UserDisconnectEnable: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myServerStopBuffer( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
char *str;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] ServerStopBuffer missing argument\n" );
		goto bailout;
	}

	str = Config_CopyString( & buf[argpos] );

	if ( str == NULL )
	{
		printf( "Config: [Actions] ServerStopBuffer error copying string\n" );
		goto bailout;
	}

	strncpy( ActionBuffer_ServerStop, str, MAX_ACTIONBUFFER-1 );

	myFree( str );

	if ( DoVerbose )
	{
		printf( "[Actions] ServerStopBuffer: '%s'\n", ActionBuffer_ServerStop );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myServerStartBuffer( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
char *str;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] ServerStartBuffer missing argument\n" );
		goto bailout;
	}

	str = Config_CopyString( & buf[argpos] );

	if ( str == NULL )
	{
		printf( "Config: [Actions] ServerStartBuffer error copying string\n" );
		goto bailout;
	}

	strncpy( ActionBuffer_ServerStart, str, MAX_ACTIONBUFFER-1 );

	myFree( str );

	if ( DoVerbose )
	{
		printf( "[Actions] ServerStartBuffer: '%s'\n", ActionBuffer_ServerStart );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myProgramStopBuffer( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
char *str;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] ProgramStopBuffer missing argument\n" );
		goto bailout;
	}

	str = Config_CopyString( & buf[argpos] );

	if ( str == NULL )
	{
		printf( "Config: [Actions] ProgramStopBuffer error copying string\n" );
		goto bailout;
	}

	strncpy( ActionBuffer_ProgramStop, str, MAX_ACTIONBUFFER-1 );

	myFree( str );

	if ( DoVerbose )
	{
		printf( "[Actions] ProgramStopBuffer: '%s'\n", ActionBuffer_ProgramStop );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myProgramStartBuffer( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
char *str;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] ProgramStartBuffer missing argument\n" );
		goto bailout;
	}

	str = Config_CopyString( & buf[argpos] );

	if ( str == NULL )
	{
		printf( "Config: [Actions] ProgramStartBuffer error copying string\n" );
		goto bailout;
	}

	strncpy( ActionBuffer_ProgramStart, str, MAX_ACTIONBUFFER-1 );

	myFree( str );

	if ( DoVerbose )
	{
		printf( "[Actions] ProgramStartBuffer: '%s'\n", ActionBuffer_ProgramStart );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myUserConnectBuffer( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
char *str;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] UserConnectBuffer missing argument\n" );
		goto bailout;
	}

	str = Config_CopyString( & buf[argpos] );

	if ( str == NULL )
	{
		printf( "Config: [Actions] UserConnectBuffer error copying string\n" );
		goto bailout;
	}

	strncpy( ActionBuffer_UserConnect, str, MAX_ACTIONBUFFER-1 );

	myFree( str );

	if ( DoVerbose )
	{
		printf( "[Actions] UserConnectBuffer: '%s'\n", ActionBuffer_UserConnect );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myUserDisconnectBuffer( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
char *str;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Actions] UserDisconnectBuffer missing argument\n" );
		goto bailout;
	}

	str = Config_CopyString( & buf[argpos] );

	if ( str == NULL )
	{
		printf( "Config: [Actions] UserDisconnectBuffer error copying string\n" );
		goto bailout;
	}

	strncpy( ActionBuffer_UserDisconnect, str, MAX_ACTIONBUFFER-1 );

	myFree( str );

	if ( DoVerbose )
	{
		printf( "[Actions] UserDisconnectBuffer: '%s'\n", ActionBuffer_UserDisconnect );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

struct ParseCommand ParseActionsCmds[] =
{
{ 16, "ServerStopEnable", myServerStopEnable },
{ 16, "ServerStopBuffer", myServerStopBuffer },
{ 17, "ProgramStopEnable", myProgramStopEnable },
{ 17, "ProgramStopBuffer", myProgramStopBuffer },
{ 17, "ServerStartEnable", myServerStartEnable },
{ 17, "ServerStartBuffer", myServerStartBuffer },
{ 17, "UserConnectEnable", myUserConnectEnable },
{ 17, "UserConnectBuffer", myUserConnectBuffer },
{ 18, "ProgramStartEnable", myProgramStartEnable },
{ 18, "ProgramStartBuffer", myProgramStartBuffer },
{ 20, "UserDisconnectEnable", myUserDisconnectEnable },
{ 20, "UserDisconnectBuffer", myUserDisconnectBuffer },
{ 0, NULL, NULL }
};

// --
