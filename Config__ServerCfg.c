 
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

#if 0
static int myIP( struct Config *cfg, char *buf, int pos2 UNUSED, int argpos, int arglen )
{
struct IPNode *node;
char str[256];
int error;
int pos;
int cnt;
int len;
int a;
int b;
int c;
int d;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: [BlackList] IP missing argument" );
		goto bailout;
	}

	pos = argpos;

	// -- Find A

	while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
	{
		pos++;
	}

	if ( buf[pos] == '*' )
	{
		a = -1;

		pos++;
	}
	else
	{
		a = atoi( & buf[pos] );

		if (( a < 0 ) || ( a > 255 ))
		{
			goto  bailout;
		}

		while(( buf[pos] >= '0' ) && ( buf[pos] <= '9' ))
		{
			pos++;
		}
	}

	while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
	{
		pos++;
	}

	if ( buf[pos++] != '.' )
	{
		goto bailout;
	}

	// -- Find B

	while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
	{
		pos++;
	}

	if ( buf[pos] == '*' )
	{
		b = -1;

		pos++;
	}
	else
	{
		b = atoi( & buf[pos] );

		if (( b < 0 ) || ( b > 255 ))
		{
			goto  bailout;
		}

		while(( buf[pos] >= '0') && ( buf[pos] <= '9' ))
		{
			pos++;
		}
	}

	while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
	{
		pos++;
	}

	if ( buf[pos++] != '.' )
	{
		goto bailout;
	}

	// -- Find C

	while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
	{
		pos++;
	}

	if ( buf[pos] == '*' )
	{
		c = -1;

		pos++;
	}
	else
	{
		c = atoi( & buf[pos] );

		if (( c < 0 ) || ( c > 255 ))
		{
			goto  bailout;
		}

		while(( buf[pos] >= '0' ) && ( buf[pos] <= '9' ))
		{
			pos++;
		}
	}

	while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
	{
		pos++;
	}

	if ( buf[pos++] != '.' )
	{
		goto bailout;
	}

	// -- Find D

	while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
	{
		pos++;
	}

	if ( buf[pos] == '*' )
	{
		d = -1;
	}
	else
	{
		d = atoi( & buf[pos] );

		if (( d < 0 ) || ( d > 255 ))
		{
			goto  bailout;
		}
	}

	// --

	node = myCalloc( 1, sizeof( struct IPNode ));

	if ( node == NULL )
	{
		goto bailout;
	}

	node->ipn_A = a;
	node->ipn_B = b;
	node->ipn_C = c;
	node->ipn_D = d;

	IExec->AddTail( & cfg->WhiteList, (APTR) node );

	// --

	if ( DoVerbose )
	{
		str[0] = 0;
		len = 0;

		if ( a == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", a );
		}

		len = strlen( str );

		if ( b == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", b );
		}

		len = strlen( str );

		if ( c == -1 )
		{
			sprintf( & str[len], "*." );
		}
		else
		{
			sprintf( & str[len], "%d.", c );
		}

		len = strlen( str );

		if ( d == -1 )
		{
			sprintf( & str[len], "*" );
		}
		else
		{
			sprintf( & str[len], "%d", d );
		}

		printf( "[WhiteList] Adding IP %s\n", str );
	}

	error = FALSE;

bailout:

	if ( error )
	{
		for( cnt=0 ; cnt<1023 ; cnt++ )
		{
			if (( buf[argpos+cnt] ) 
			&&	( buf[argpos+cnt] != 10 )
			&&	( buf[argpos+cnt] != 13 ))
			{
				str[cnt] = buf[argpos+cnt];
			}
			else
			{
				break;
			}
		}

		str[cnt] = 0;

		printf( "[WhiteList] Invalid IP '%s'\n", str );
	}

	return( error );
}
#endif

// --

struct ParseCommand ParseServerCfgCmds[] =
{
//{ 2,  "IP", myIP },
{ 0, NULL, NULL }
};

// --

