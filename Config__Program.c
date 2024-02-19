 
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

static int myWinPixelStatus( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Program] WinPixelStatus missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_WinData[WIN_PixelFormat].Status = val;

	if ( DoVerbose )
	{
		printf( "[Program] WinPixelStatus: %d\n", val );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myWinSessionStatus( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Program] WinSessionStatus missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_WinData[WIN_Session].Status = val;

	if ( DoVerbose )
	{
		printf( "[Program] WinSessionStatus: %d\n", val );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myWinKeyStatus( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Program] WinKeyStatus missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_WinData[WIN_KeyLogger].Status = val;

	if ( DoVerbose )
	{
		printf( "[Program] WinKeyStatus: %d\n", val );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myWinMainStatus( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Program] WinMainStatus missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_WinData[WIN_Main].Status = val;

	if ( DoVerbose )
	{
		printf( "[Program] WinMainStatus: %d\n", val );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myWinEncodingsStatus( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Program] WinEncodingsStatus missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_WinData[WIN_Encodings].Status = val;

	if ( DoVerbose )
	{
		printf( "[Program] WinEncodingsStatus: %d\n", val );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myWinAboutStatus( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Program] WinAboutStatus missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_WinData[WIN_About].Status = val;

	if ( DoVerbose )
	{
		printf( "[Program] WinAboutStatus: %d\n", val );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myVerbose( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Program] Verbose missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	DoVerbose = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		PrintProgramHeader();
		printf( "[Program] Verbose: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myDisableGUI( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Program] DiableGUI missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ProgramDisableGUI = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Program] Disabling GUI: %s\n", ( val ) ? "Yes" : "No" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myDisableARexx( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Program] DiableARexx missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ProgramDisableARexx = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Program] Disabling ARexx: %s\n", ( val ) ? "Yes" : "No" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myDisableCxBroker( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Program] DiableCxBroker missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_ProgramDisableCxBroker = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Program] Disabling CxBroker: %s\n", ( val ) ? "Yes" : "No" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

struct ParseCommand ParseProgramCmds[] =
{
{  7, "Verbose", myVerbose },
{ 10, "DisableGUI", myDisableGUI },
{ 12, "DisableARexx", myDisableARexx },
{ 12, "WinKeyStatus", myWinKeyStatus },
{ 13, "WinMainStatus", myWinMainStatus },
{ 14, "WinAboutStatus", myWinAboutStatus },
{ 14, "WinPixelStatus", myWinPixelStatus },
{ 15, "DisableCxBroker", myDisableCxBroker },
{ 16, "WinSessionStatus", myWinSessionStatus },
{ 18, "WinEncodingsStatus", myWinEncodingsStatus },
{ 0, NULL, NULL }
};

// --

