 
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
** This file holds functions
** for parsing Config file
*/

// --

#include "RVNCd.h"

// --

static const char *	ConfigHeaderStr	= "[RVNC SERVER CONFIG]\n";

// --

struct Section
{
	int		Len;
	char *	Name;
	struct ParseCommand *Cmds;
};

struct Section mySec[] =
{
{ 11, "[SERVERCFG:", ParseServerCfgCmds },
{ 11, "[BLACKLIST]", ParseBlackCmds },
{ 11, "[WHITELIST]", ParseWhiteCmds },
{  9, "[ACTIONS]", ParseActionsCmds },
{  9, "[PROGRAM]", ParseProgramCmds },
{  8, "[SERVER]", ParseServerCmds },
{  5, "[LOG]", ParseLogCmds },
{  0, NULL, NULL }
};

// --

char *Config_CopyString( char *buffer )
{
char *str;
int mark;
int pos;
int len;

	str = NULL;

	pos = 0;

	while(( buffer[pos] == 9 ) || ( buffer[pos] == 32 ))
	{
		pos++;
	}

	if (( buffer[pos] == '"' ) || ( buffer[pos] == '\'' ))
	{
		mark = buffer[ pos++ ];

		len = pos;

		while(	( buffer[ len ] != 0 )
		&&		( buffer[ len ] != 10 )
		&&		( buffer[ len ] != mark ))
		{
			len += 1;
		}

		if ( buffer[ len ] != '"' )
		{
			printf( "Missing string terminator\n" );
			goto bailout;
		}

		len -= pos;
	}
	else
	{
		len = pos;

		while(	( buffer[ len ] != 0 )
		&&		( buffer[ len ] != 9 ) 
		&&		( buffer[ len ] != 10 )
		&&		( buffer[ len ] != 32 ))
		{
			len += 1;
		}

		len -= pos;
	}

	#if 0

	// Not sure if creating empty strings should be allowed or not

	if ( len == 0 )
	{
		IExec->DebugPrintF( "Zero length string not allowed\n" );
		goto bailout;
	}

	#endif

	str = myCalloc( len + 1 );

	if ( str == NULL )
	{
		printf( "Error allocating %d bytes\n", len + 1 );
		goto bailout;
	}

	if ( len )
	{
		memcpy( str, & buffer[ pos ], len );
	}

bailout:

	return( str );
}

// --

static int Config_ParseCommands( struct Config *cfg, char *buffer, int *p, int *l, struct ParseCommand *Cmds, uint32 nr )
{
int lastline;
int cmdlen;
int arglen;
int argpos;
int error;
int line;
int pos;
int cnt;
//int c;

	error = TRUE;
	line = *l;
	pos = *p;

	lastline = line;

	// Next Line
	// Skip [] Command line

	while(	( buffer[pos] != 0 )
	&&		( buffer[pos] != 10 ))
	{
		pos++;
	}

	while( buffer[pos] == 10 )
	{
		line++;
		pos++;
	}

	// --

	while( buffer[pos] )
	{
		// Find Start of line, skip Tabs, Space

		while(	( buffer[pos] == 9 )
		||		( buffer[pos] == 32 ))
		{
			pos++;
		}

		if ( buffer[pos] == '[' )
		{
			pos = lastline;
			break;
		}

		if ( buffer[pos] != ';' )
		{
			cmdlen = pos;

			while(	( buffer[cmdlen] != 0 )
			&&		( buffer[cmdlen] != 9 )
			&&		( buffer[cmdlen] != 10 )
			&&		( buffer[cmdlen] != 13 )
			&&		( buffer[cmdlen] != 32 ))
			{
				cmdlen++;
			}

			cmdlen -= pos;

			if (( cmdlen == 3 ) && ( strnicmp( & buffer[pos], "end", 3 ) == 0 ))
			{
				break;
			}

			argpos = pos + cmdlen;

			while(( buffer[argpos] == 9 ) || ( buffer[argpos] == 32 ))			
			{
				argpos++;
			}

			if ( buffer[argpos] == '=' )
			{
				argpos++;

				while(( buffer[argpos] == 9 ) || ( buffer[argpos] == 32 ))			
				{
					argpos++;
				}

				arglen = argpos;

				while(	( buffer[arglen] != 0 )
				&&		( buffer[arglen] != 9 )
				&&		( buffer[arglen] != 10 )
				&&		( buffer[arglen] != 13 )
				&&		( buffer[arglen] != 32 ))
				{
					arglen++;
				}

				arglen -= argpos;
			}
			else
			{
				arglen = 0;
			}

			// --

			cnt = 0;

			while( Cmds[cnt].Name )
			{
				if (( Cmds[cnt].Len == cmdlen ) 
				&&	( strnicmp( Cmds[cnt].Name, & buffer[pos], cmdlen ) == 0 ))
				{
					break;
				}
				else
				{
					cnt++;
				}
			}

			if ( Cmds[cnt].Func )
			{
				if ( Cmds[cnt].Func( cfg, buffer, pos, argpos, arglen, nr ))
				{
					goto bailout;
				}
			}
			else
			{
				if ( DoVerbose )
				{
					printf( "Unknown Command skipping '" );

					for( int i=0 ; i < 32 ; i++ )
					{
						int c = buffer[pos+i];

						if (( c == 0 )
						||	( c == 9 )
						||	( c == 10 )
						||	( c == 13 )
						||	( c == 32 ))
						{
							break;
						}
						else
						{
							printf( "%c", c );
						}
					}
					
					printf( "' (Line %d)\n", line );
				}
			}
		}

		lastline = pos;

		// Next Line

		while(	( buffer[pos] != 0 )
		&&		( buffer[pos] != 10 ))
		{
			pos++;
		}

		while( buffer[pos] == 10 )
		{
			line++;
			pos++;
		}
	}

	// --

	error = FALSE;

bailout:

	*p = pos;
	*l = line;

	return( error );
}

// --

static int myParseFile( struct Config *cfg, char *buffer, int bufsize UNUSED )
{
int linenr;
int error;
int len;
int pos;
int cnt;

	error = TRUE;

	// --

	len = strlen( ConfigHeaderStr );

	if ( strnicmp( buffer, ConfigHeaderStr, len ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Not a valid Config file" );
		goto bailout;
	}

	Config_Reset( cfg );

    pos = len;

	linenr = 1;

	while(( buffer[pos] ) && ( buffer[pos] != 10 ))
	{
		pos++;
	}

	while( buffer[pos] == 10 )
	{
		linenr++;
		pos++;
	}

	// --

	while( buffer[pos] )
	{
		// Find Start of line, skip Tabs, Space and NL's

		while(	( buffer[pos] == 9 )
		||		( buffer[pos] == 32 ))
	    {
		    pos++;
	    }

		if (( buffer[pos] == ';' )
		||	( buffer[pos] == 10 )
		||	( buffer[pos] == 13 ))
		{
			// Comment line or End of Line
		}

		else if ( buffer[pos] == '[' )
		{
			len = pos;

			while(	( buffer[len] != 0 )
			&&		( buffer[len] != 9 )
			&&		( buffer[len] != 10 )
			&&		( buffer[len] != 13 )
			&&		( buffer[len] != 32 )
			&&		( buffer[len] != ':' ))
			{
				len++;
			}

			if ( buffer[len] == ':' )
			{
				len++;
			}

			len -= pos;

			cnt = 0;

			while( mySec[cnt].Name )
			{
				if (( mySec[cnt].Len == len ) 
				&&	( strnicmp( mySec[cnt].Name, & buffer[pos], mySec[cnt].Len ) == 0 ))
				{
					break;
				}
				else
				{
					cnt++;
				}
			}

			if ( mySec[cnt].Cmds )
			{
				char *str = & buffer[ pos + len - 1 ];
				uint32 nr = 0;

				if ( *str == ':' )
				{
					int read;
					int stat;

					stat = sscanf( str, ":%ld]%n", & nr, & read );

					// %n do not increase the return count
					if ( stat != 1 )
					{
						// Log_PrintF( "Invalid group number" );
						goto bailout;
					}

					if ( str[read-1] != ']' )
					{
						// Log_PrintF( "Invalid group termination" );
						goto bailout;
					}
				}

				if ( Config_ParseCommands( cfg, buffer, & pos, & linenr, mySec[cnt].Cmds, nr ))
				{
					goto bailout;
				}
			}
		}

		// Next Line

		while(	( buffer[pos] != 0 )
		&&		( buffer[pos] != 10 ))
		{
			pos++;
		}

		while( buffer[pos] == 10 )
		{
			linenr++;
			pos++;
		}
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myLoadFile( char *name, char *buffer, int size )
{
int error;
BPTR h;

	error = TRUE;

	h = IDOS->Open( name, MODE_OLDFILE );

	if ( h == 0 )
	{
		printf( "File '%s' not found\n", name );
		goto bailout;
	}

	if ( IDOS->Read( h, buffer, size ) != size )
	{
		printf( "Error reading file\n" );
		goto bailout;
	}

	error = FALSE;

bailout:

	if ( h )
	{
		IDOS->Close( h );
	}

	return( error );
}

// --

int Config_ParseFile( struct Config *cfg, char *filename )
{
char *buffer;
int cfgsize;
int error;

	// --

	buffer = NULL;
	error = TRUE;

	if ( filename )
	{
		if ( myGetFileSize( filename, & cfgsize ) < 1 )
		{
			if (( DoVerbose ) || ( ! ProgramRunning ))
			{
				printf( "Config file '%s' not found\n", filename );
			}
			goto bailout;
		}
	}
	else
	{
		filename = "RVNCd.Config";

		if ( myGetFileSize( filename, & cfgsize ) < 1 )
		{
			error = FALSE;
			goto bailout;
		}
	}

	buffer = myCalloc( cfgsize + 1 );

	if ( buffer == NULL )
	{
		if (( DoVerbose ) || ( ! ProgramRunning ))
		{
			printf( "Error allocating memory (%d)\n", cfgsize + 1 );
		}
		goto bailout;
	}

	if ( DoVerbose )
	{
		printf( "Loading '%s' Config file\n", filename );
	}

	if ( myLoadFile( filename, buffer, cfgsize ))
	{
		goto bailout;
	}

	if ( myParseFile( cfg, buffer, cfgsize ))
	{
		goto bailout;
	}

	if ( cfg->cfg_Config_FileName )
	{
		myFree( cfg->cfg_Config_FileName );
	}

	cfg->cfg_Config_FileName = myStrdup( filename );

	if ( DoVerbose )
	{
		printf( "Loaded '%s' config\n", IDOS->FilePart( filename ));
	}

	error = FALSE;

bailout:

	if ( buffer )
	{
		myFree( buffer );
	}

	// --

	return( error );
}

// --
