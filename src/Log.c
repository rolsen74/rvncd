
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

void Log_Setup( struct Config *cfg )
{
STR name;
BPTR file;
S32 nr;

	nr = 1;

	while( TRUE )
	{
		if ( nr < 2 )
		{
			name = mem_ASPrintF( "%s.txt", cfg->cfg_LogFilename );
		}
		else
		{
			name = mem_ASPrintF( "%s.%03d", cfg->cfg_LogFilename, nr );
		}

		file = Open( name, MODE_OLDFILE );

		if ( file == 0 )
		{
			nr--;
			break;
		}

		Close( file );

		if ( name )
		{
			mem_Free( name );
			name = NULL;
		}

		nr++;
	}

	if ( name )
	{
		mem_Free( name );
	}

	if ( nr < 2 )
	{
		nr = 1;
	}

	cfg->cfg_LogFileNumber = nr;
	cfg->cfg_LogInit = 0;
}

// --

static BPTR myOpenFile( struct Config *cfg, S32 needed, S32 *size )
{
STR curname;
BPTR file;
S32 cursize;
S32 curnr;
S32 len;

	// --

	file = 0;

	curname		= NULL;
	cursize		= 0;
	curnr		= cfg->cfg_LogFileNumber;

	// -- Load Current

	if ( curnr < 2 )
	{
		curname = mem_ASPrintF( "%s.txt", cfg->cfg_LogFilename );
		curnr = 1;
	}
	else
	{
		curname = mem_ASPrintF( "%s.%03d", cfg->cfg_LogFilename, curnr );
	}

	len = Func_GetFileSize( curname, & cursize );

	if ( len < 0 )
	{
		goto bailout;
	}

	// -- Do we need to split the File?

	if ( cursize + needed >= cfg->cfg_LogFileLimit )
	{
		curnr++;

		curname = mem_ASPrintF( "%s.%03d", cfg->cfg_LogFilename, curnr );

		if ( len < 0 )
		{
			goto bailout;
		}

	}

	// --

	file = Open( curname, MODE_READWRITE );

	if ( ! file )
	{
		goto bailout;
	}

	#ifdef _AOS4_

	ChangeFilePosition( file, 0, OFFSET_END );

	*size = (S32) GetFilePosition( file );

	#else

	/**/    Seek( file, 0, OFFSET_END );
	*size = Seek( file, 0, OFFSET_CURRENT );

	#endif

	// --

	cfg->cfg_LogFileNumber = curnr;

bailout:

	// --

	if ( curname )
	{
		mem_Free( curname );
	}

	return( file );
}

// --

static void LogAddString( struct Config *cfg, S32 type, time_t mytime, const STR str )
{
struct tm *tm;
BPTR file;
STR s;
char buf[32];
S32 size;
S32 len;
S32 pos;
S32 h;

	if ( ! cfg->cfg_LogFilename )
	{
		goto bailout;
	}

	if ( cfg->cfg_LogInit )
	{
		Log_Setup( cfg );
	}

	// -- Build Date and Time header

	tm = localtime( & mytime );

//	"\n[ 17:59:55 30-Jun-2023 ] x: ";
//	  123456789.123456789.1234567890

	/**/ if (( type & 0xf ) == LOGTYPE_Error )	s = " E: ";
	else if (( type & 0xf ) == LOGTYPE_Info )	s = " I: ";
	else /* Warning */					s = " W: ";

	strftime( buf, sizeof(buf), "\n[ %H:%M:%S %d-%b-%Y ]", tm );

	strcat( buf, s );

	h = strlen( buf );

	// --

	pos = 0;

	while( str[pos] )
	{
		len = pos;

		while( TRUE )
		{
			if (( str[len] == 0 )
			||	( str[len] == 10 ))
			{
				break;
			}
			else
			{
				len++;
			}
		}

		len -= pos;

		// --

		// We need to write Header (h) + String (len)
		size = 0; // avoid gcc warning
		file = myOpenFile( cfg, h + len, & size );

		if ( file )
		{
			if ( size )
			{
				// Header with NewLine
				Write( file, buf, h );
			}
			else
			{
				// Header Skip NewLine
				Write( file, & buf[1], h-1 );
			}

			Write( file, & str[pos], len );
			Close( file );
		}

		// --

		pos += len;

		if ( str[pos] == 10 )
		{
			pos++;
		}
	}

bailout:

	return;
}

// --

static void GUIAddString( struct Config *cfg UNUSED, S32 type, time_t t, STR buf )
{
struct CommandLogString *msg;
struct tm *tm;
S32 len;

	len = strlen( buf );


	msg = mem_AllocClr( sizeof( struct CommandLogString ) + len );

	if ( ! msg )
	{
		goto bailout;
	}

	msg->cl_Command = CMD_LogString;
	msg->cl_Type = type;

	// --

	tm = localtime( & t );

	strftime( msg->cl_Time, sizeof( msg->cl_Time ), "%H:%M:%S", tm );

	// --

	memcpy( msg->cl_String, buf, len );

	// --

	MsgPort_PutMsg( & CmdMsgPort, & msg->cl_Message );

bailout:

	return;
}

// --

void Log_PrintF( struct Config *cfg, S32 type, const STR fmt, ... )
{
va_list args;
time_t mytime;
STR buf;
S32 size;
S32 t;

	// --

	ObtainSemaphore( & cfg->cfg_LogPrintFSema );

	// --

	buf = NULL;

	if ( ! cfg->cfg_LogFilename )
	{
		goto bailout;
	}

//	buf = NULL;
//	buf = IUtility->VASPrintf( fmt, (PTR) va_getlinearva( ap, U32 ));

	// ----

	va_start( args, fmt );
	size = vsnprintf( NULL, 0, fmt, args );
	va_end( args );

	if ( size <= 0 )
	{
		SHELLBUF_PRINTF( "Error calculating Log buffer size\n" );
		goto bailout;
	}

	size += 1;

	buf = (STR) malloc( size );

	if ( ! buf )
	{
		SHELLBUF_PRINTF( "Error allocating Log buffer\n" );
		goto bailout;
	}

	va_start( args, fmt );
	vsnprintf( buf, size, fmt, args );
	va_end( args );

	// ----

	if ( cfg->cfg_LogEnable )
	{
		t = type & 0xf;

		if ((( t == LOGTYPE_Info    ) && ( cfg->cfg_ProgramLogInfos ))
		||  (( t == LOGTYPE_Error   ) && ( cfg->cfg_ProgramLogErrors ))
		||  (( t == LOGTYPE_Warning ) && ( cfg->cfg_ProgramLogWarnings ))
		||	 ( type & LOGTYPE_Event ))
		{
			time( & mytime );

			LogAddString( cfg, type, mytime, buf );

			GUIAddString( cfg, type, mytime, buf );
		}
	}

	if ( DoVerbose > 0 )
	{
		// Shell test
		SHELLBUF_PRINTF( "%s\n", buf );
//		ShellBuf_Insert( buf, size );
//		ShellBuf_Insert( "\n", 1 );
	}

bailout:

	if ( buf )
	{
//		FreeVec( buf );
//		mem_Free( buf );
		free( buf );
	}

	// --

	ReleaseSemaphore( & cfg->cfg_LogPrintFSema );

	// --
}

// --
