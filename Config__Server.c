 
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
** parsing config file Server section
*/

// --

#include "RVNCd.h"

// --

static int myPointer( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
char *str;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] PointerFile missing argument\n" );
		goto bailout;
	}

	str = Config_CopyString( & buf[argpos] );

	if ( str == NULL )
	{
		printf( "Config: [Server] PointerFile error copying string\n" );
		goto bailout;
	}

	if ( cfg->cfg_PointerFileName )
	{
		myFree( cfg->cfg_PointerFileName );
	}

	cfg->cfg_PointerFileName = str;

	if ( DoVerbose )
	{
		printf( "[Server] PointerFile: '%s'\n", str );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myPassword( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
char *str;
int error;

	str = NULL;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] Password missing argument\n" );
		goto bailout;
	}

	str = Config_CopyString( & buf[argpos] );

	if ( str == NULL )
	{
		printf( "Config: [Server] Password error copying string\n" );
		goto bailout;
	}

	strncpy( cfg->cfg_Disk_Settings.Password, str, MAX_SERVERPASSWORD );
	cfg->cfg_Disk_Settings.Password[MAX_SERVERPASSWORD] = 0;

	if ( strlen( str ) > 8 )
	{
		printf( "Password exceeds 8 chars. Truncating to 8.\n" );
	}

	// Proberly not too smart to print the password to shell

//	if ( DoVerbose )
//	{
//		printf( "[Server] Password: '%s'\n", str );
//	}

	error = FALSE;

bailout:

	if ( str )
	{
		myFree( str );
	}

	return( error );
}

// --

static int myAutoStart( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] AutoStart missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_Disk_Settings2.AutoStart = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Server] AutoStart: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int mySendClipboard( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] SendClipboard missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_Disk_Settings.SendClipboard = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Server] SendClipboard: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myReadClipboard( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] ReadClipboard missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_Disk_Settings.ReadClipboard = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Server] ReadClipboard: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myDelayFrequency( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] DelayFrequency missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->GfxRead_DelayFrequency = val;

	if ( DoVerbose )
	{
		printf( "[Server] DelayFrequency: %d\n", val );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myEncZLIB( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] EncZLIB missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_Disk_Settings.ZLib = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Server] EncZLIB: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myEncRichCursor( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] EncRichCursor missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_Disk_Settings.RichCursor = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Server] EncRichCursor: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myUpdateSync( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] UpdateSync missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_Disk_Settings.BufferSync = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Server] UpdateSync: %s\n", ( val ) ? "On" : "Off" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myDisableBlanker( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] DisableBlanker missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->cfg_Disk_Settings.DisableBlanker = ( val ) ? TRUE : FALSE;

	if ( DoVerbose )
	{
		printf( "[Server] DisableBlanker: %s\n", ( val ) ? "Yes" : "No" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myScanMethod( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] ScanMethod missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	cfg->GfxRead_BufferScanMethod = val;

	if ( DoVerbose )
	{
		printf( "[Server] ScanMethod: %s\n", ( val == SCANMethod_Linear ) ? "Linear" : "Random" );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myTileSize( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] TileSize missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	if ( val < 16 )
	{
		val = 16;
		printf( "Config: [Server] TileSize out of range .. Clamping to 16\n" );
	}

	if ( 256 < val )
	{
		val = 256;
		printf( "Config: [Server] TileSize out of range .. Clamping to 256\n" );
	}

	cfg->cfg_TileSize = val;

	if ( DoVerbose )
	{
		printf( "[Server] TileSize: %dx%d\n", val, val );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myPort( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
int val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] Port missing argument\n" );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	if (( val < 0 ) && ( 65535 < val ))
	{
		printf( "Config: [Server] Invalid Port number\n" );
		goto bailout;
	}

	cfg->cfg_Disk_Settings.Port = val;

	if ( DoVerbose )
	{
		printf( "[Server] Port: %d\n", val );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myName( struct Config *cfg UNUSED, char *buf, int pos UNUSED, int argpos, int arglen, uint32 nr UNUSED )
{
char *name;
int error;

	name = NULL;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [Server] Name missing argument\n" );
		goto bailout;
	}

	name = Config_CopyString( & buf[argpos] );

	if ( name == NULL )
	{
		printf( "Config: [Server] Name error copying string\n" );
		goto bailout;
	}

	strncpy( cfg->cfg_Disk_Settings.Name, name, MAX_SERVERNAME );
	cfg->cfg_Disk_Settings.Name[MAX_SERVERNAME] = 0;

//	cfg->cfg_ServerName = name;

	if ( DoVerbose )
	{
		printf( "[Server] Name: '%s'\n", cfg->cfg_Disk_Settings.Name );
	}

	error = FALSE;

bailout:

	if ( name )
	{
		myFree( name );
	}

	return( error );
}

// --

struct ParseCommand ParseServerCmds[] =
{
{  4, "Port", myPort },
{  4, "Name", myName },
{  7, "EncZLIB", myEncZLIB },
{  8, "Password", myPassword },
{  8, "TileSize", myTileSize },
//{  8, "EncCursor", myEncCursor },
{  9, "AutoStart", myAutoStart },
{ 10, "UpdateSync", myUpdateSync },
{ 10, "ScanMethod", myScanMethod },
{ 11, "PointerFile", myPointer },
{ 13, "SendClipboard", mySendClipboard },
{ 13, "ReadClipboard", myReadClipboard },
{ 13, "EncRichCursor", myEncRichCursor },
{ 14, "DisableBlanker", myDisableBlanker },
{ 14, "DelayFrequency", myDelayFrequency },
{ 0, NULL, NULL }
};

// --
