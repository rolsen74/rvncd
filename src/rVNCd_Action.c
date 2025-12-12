
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

struct SignalSemaphore		ActionSema;
char						ActionBuffer_ProgramStart[MAX_ACTIONBUFFER];
char						ActionBuffer_ProgramStop[MAX_ACTIONBUFFER];
char						ActionBuffer_ServerStart[MAX_ACTIONBUFFER];
char						ActionBuffer_ServerStop[MAX_ACTIONBUFFER];
char						ActionBuffer_UserConnect[MAX_ACTIONBUFFER];
char						ActionBuffer_UserDisconnect[MAX_ACTIONBUFFER];
static char					FmtBuffer[ MAX_ACTIONBUFFER + 256 ];

// --

static S32 myDoFmt( STR fmt )
{
S32 retval;
S32 pos1;
S32 pos2;
STR buf;
S32 chr;

	buf = FmtBuffer;
	pos1 = 0;
	pos2 = 0;
	*buf = 0;
	retval = FALSE;

	if (( fmt == NULL ) || ( fmt[0] == 0 ))
	{
		SHELLBUF_PRINTF( "Empty String\n" );
		goto bailout;
	}

	while( TRUE )
	{
		if (( pos1 >= MAX_ACTIONBUFFER-1 )
		||  ( pos2 >= MAX_ACTIONBUFFER-1+256 ))
		{
			SHELLBUF_PRINTF( "Buffer overflow\n" );
			goto bailout;
		}

		chr = fmt[pos1++];

		if ( chr != '%' )
		{
			buf[pos2++] = chr;

			if ( chr == 0 )
			{
				break;
			}
			else
			{
				continue;
			}
		}

		chr = fmt[pos1++];

		switch( chr )
		{
			case '%':
			{
				buf[pos2++] = chr;
				break;
			}

			default:
			{
				SHELLBUF_PRINTF( "Unsupported String Format\n" );
				goto bailout;
			}
		}
	}

	if ( pos2 == 0 )
	{
		goto bailout;
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --

static void myDoCmd( STR fmt )
{
LONG stat;
BPTR i;
BPTR o;

	if ( myDoFmt( fmt ))
	{
		#ifdef _AOS4_
		i = DupFileHandle( Input() );
		o = DupFileHandle( Output() );
		#else
		i = Open( "NIL:", MODE_NEWFILE );
		o = 0;
		#endif

		if (( i ) && ( o ))
		{
			struct TagItem tags[] =
			{
				{ SYS_UserShell, TRUE },
				{ SYS_Asynch, TRUE },
				{ SYS_Output, o },
				{ SYS_Input, i },
				{ TAG_END }
			};

			stat = SystemTagList( FmtBuffer, tags );

			// Okay sending a Line Terminator, restores CTRL+C for CLI
			// Don't use FPuts as 'o' might be closed here
			Delay( 5 );
			PutStr( "" );
		}
		else
		{
			stat = -1;
		}

		if ( stat == -1 )
		{
			if ( i )	Close( i );
			if ( o )	Close( o );
		}
	}
}

// --

void DoAction_ProgramStart( struct Config *cfg UNUSED )
{
	#ifdef DEBUG
	SHELLBUF_PRINTF( "DoAction_ProgramStart\n" );
	#endif

	ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_ProgramStart );

	ReleaseSemaphore( & ActionSema );
}

// --

void DoAction_ProgramStop( struct Config *cfg UNUSED )
{
	#ifdef DEBUG
	SHELLBUF_PRINTF( "DoAction_ProgramStop\n" );
	#endif

	ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_ProgramStop );

	ReleaseSemaphore( & ActionSema );
}

// --

void DoAction_ServerStart( struct Config *cfg UNUSED )
{
	#ifdef DEBUG
	SHELLBUF_PRINTF( "DoAction_ServerStart\n" );
	#endif

	ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_ServerStart );

	ReleaseSemaphore( & ActionSema );
}

// --

void DoAction_ServerStop( struct Config *cfg UNUSED )
{
	#ifdef DEBUG
	SHELLBUF_PRINTF( "DoAction_ServerStop\n" );
	#endif

	ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_ServerStop );

	ReleaseSemaphore( & ActionSema );
}

// --

void DoAction_UserConnect( struct Config *cfg UNUSED )
{
	#ifdef DEBUG
	SHELLBUF_PRINTF( "DoAction_UserConnect\n" );
	#endif

	ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_UserConnect );

	ReleaseSemaphore( & ActionSema );
}

// --

void DoAction_UserDisconnect( struct Config *cfg UNUSED )
{
	#ifdef DEBUG
	SHELLBUF_PRINTF( "DoAction_UserDisconnect\n" );
	#endif

	ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_UserDisconnect );

	ReleaseSemaphore( & ActionSema );
}

// --
