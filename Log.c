
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

void Log_Setup( struct Config *cfg )
{
char *name;
BPTR file;
int nr;

	nr = 1;

	while( TRUE )
	{
		if ( nr < 2 )
		{
			name = myASPrintF( "%s.txt", cfg->cfg_LogFilename );
		}
		else
		{
			name = myASPrintF( "%s.%03d", cfg->cfg_LogFilename, nr );
		}

		file = IDOS->Open( name, MODE_OLDFILE );

		if ( file == 0 )
		{
			nr--;
			break;
		}

		IDOS->Close( file );

		if ( name )
		{
			myFree( name );
			name = NULL;
		}

		nr++;
	}

	if ( name )
	{
		myFree( name );
	}

	if ( nr < 2 )
	{
		nr = 1;
	}

	cfg->cfg_LogFileNumber = nr;
	cfg->cfg_LogInit = 0;
}

// --

static BPTR myOpenFile( struct Config *cfg, int needed, int *size )
{
char *curname;
BPTR file;
int cursize;
int curnr;
int len;

	// --

	file = 0;

	curname		= NULL;
	cursize		= 0;
	curnr		= cfg->cfg_LogFileNumber;

	// -- Load Current

	if ( curnr < 2 )
	{
		curname = myASPrintF( "%s.txt", cfg->cfg_LogFilename );
		curnr = 1;
	}
	else
	{
		curname = myASPrintF( "%s.%03d", cfg->cfg_LogFilename, curnr );
	}

	len = myGetFileSize( curname, & cursize );

	if ( len < 0 )
	{
		goto bailout;
	}

	// -- Do we need to split the File?

	if ( cursize + needed >= cfg->cfg_LogFileLimit )
	{
		curnr++;

		curname = myASPrintF( "%s.%03d", cfg->cfg_LogFilename, curnr );

		if ( len < 0 )
		{
			goto bailout;
		}

	}

	// --

	file = IDOS->Open( curname, MODE_READWRITE );

	if ( file == 0 )
	{
		goto bailout;
	}

	IDOS->ChangeFilePosition( file, 0, OFFSET_END );

	*size = (int) IDOS->GetFilePosition( file );

	// --

	cfg->cfg_LogFileNumber = curnr;

bailout:

	// --

	if ( curname )
	{
		myFree( curname );
	}

	return( file );
}

// --

static void LogAddString( struct Config *cfg, int type, time_t mytime, const char *str )
{
struct tm *tm;
BPTR file;
char *s;
char buf[32];
int size;
int len;
int pos;
int h;

	if ( cfg->cfg_LogFilename == NULL )
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
				IDOS->Write( file, buf, h );
			}
			else
			{
				// Header Skip NewLine
				IDOS->Write( file, & buf[1], h-1 );
			}

			IDOS->Write( file, & str[pos], len );
			IDOS->Close( file );
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

static void GUIAddString( struct Config *cfg UNUSED, int type, time_t t, char *buf )
{
struct CommandLogString *msg;
struct tm *tm;
int len;

	len = strlen( buf );


	msg = myCalloc( sizeof( struct CommandLogString ) + len );

	if ( msg == NULL )
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

	IExec->PutMsg( CmdMsgPort, & msg->cl_Message );

bailout:

	return;
}

// --

void VARARGS68K Log_PrintF( struct Config *cfg, int type, const char *fmt, ... )
{
va_list ap;
time_t mytime;
char *buf;
int t;

	// --

	IExec->ObtainSemaphore( & cfg->cfg_LogPrintFSema );

	// --

	buf = NULL;

	va_start( ap, fmt );

	if ( cfg->cfg_LogFilename == NULL )
	{
		goto bailout;
	}

	buf = IUtility->VASPrintf( fmt, (APTR) va_getlinearva( ap, uint32 ));

	if ( buf == NULL )
	{
		printf( "Error building Log buffer\n" );
		goto bailout;
	}

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

	if ( DoVerbose )
	{
		printf( "%s\n", buf );
	}

bailout:

	va_end( ap );

	if ( buf )
	{
		IExec->FreeVec( buf );
//		myFree( buf );
//		free( buf );
	}

	// --

	IExec->ReleaseSemaphore( & cfg->cfg_LogPrintFSema );

	// --
}

// --
