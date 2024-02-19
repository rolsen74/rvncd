 
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

static char FmtBuffer[ MAX_ACTIONBUFFER + 256 ];

// --

static int myDoFmt( char *fmt )
{
char *buf;
int retval;
int pos1;
int pos2;
int chr;

	buf = FmtBuffer;
	pos1 = 0;
	pos2 = 0;
	*buf = 0;
	retval = FALSE;

	if (( fmt == NULL ) || ( fmt[0] == 0 ))
	{
		printf( "Empty String\n" );
		goto bailout;
	}

	while( TRUE )
	{
		if (( pos1 >= MAX_ACTIONBUFFER-1 )
		||  ( pos2 >= MAX_ACTIONBUFFER-1+256 ))
		{
			printf( "Buffer overflow\n" );
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
				printf( "Unsupported String Format\n" );
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

static void myDoCmd( char *fmt )
{
LONG stat;
BPTR i;
BPTR o;

	if ( myDoFmt( fmt ))
	{
		i = IDOS->DupFileHandle( IDOS->Input() );
		o = IDOS->DupFileHandle( IDOS->Output() );

		if (( i ) && ( o ))
		{
			stat = IDOS->SystemTags( FmtBuffer, 
				SYS_Asynch, TRUE,
				SYS_Output, o,
				SYS_Input, i,
				TAG_END
			);

			// Okay sending a Line Terminator, restores CTRL+C for CLI
			// Don't use FPuts as 'o' might be closed here
			IDOS->Delay( 5 );
			IDOS->PutStr( "" );
		}
		else
		{
			stat = -1;
		}

		if ( stat == -1 )
		{
			if ( i )	IDOS->Close( i );
			if ( o )	IDOS->Close( o );
		}
	}
}

// --

void DoAction_ProgramStart( struct Config *cfg UNUSED )
{
	IExec->ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_ProgramStart );

	IExec->ReleaseSemaphore( & ActionSema );
}

// --

void DoAction_ProgramStop( struct Config *cfg UNUSED )
{
	IExec->ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_ProgramStop );

	IExec->ReleaseSemaphore( & ActionSema );
}

// --

void DoAction_ServerStart( struct Config *cfg UNUSED )
{
	IExec->ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_ServerStart );

	IExec->ReleaseSemaphore( & ActionSema );
}

// --

void DoAction_ServerStop( struct Config *cfg UNUSED )
{
	IExec->ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_ServerStop );

	IExec->ReleaseSemaphore( & ActionSema );
}

// --

void DoAction_UserConnect( struct Config *cfg UNUSED )
{
	IExec->ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_UserConnect );

	IExec->ReleaseSemaphore( & ActionSema );
}

// --

void DoAction_UserDisconnect( struct Config *cfg UNUSED )
{
	IExec->ObtainSemaphore( & ActionSema );

	myDoCmd( ActionBuffer_UserDisconnect );

	IExec->ReleaseSemaphore( & ActionSema );
}

// --
