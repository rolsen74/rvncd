
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 Config_Read_IP( struct Config *cfg, const struct CFGNODE *Cmd, const STR grpname, STR mem, S32 line )
{
struct IPNode *node;
char str[256];
S32 retval;
S32 pos;
S32 len;
S32 a[4];
S32 i;

	retval = FALSE;

	// --

	pos = 0;

	for( i=0 ; i<4 ; i++ )
	{
		while(( mem[pos] == 9 ) || ( mem[pos] == 32 ))
		{
			pos++;
		}

		if ( mem[pos] == '*' )
		{
			a[i] = -1;

			pos++;
		}
		else
		{
			a[i] = atoi( & mem[pos] );

			if (( a[i] < 0 ) || ( a[i] > 255 ))
			{
				SHELLBUF_PRINTF3( "Config: %s '%s' invalid IP number (Line %d)\n", grpname, Cmd->Name, line );
				goto  bailout;
			}

			while(( mem[pos] >= '0' ) && ( mem[pos] <= '9' ))
			{
				pos++;
			}
		}

		if ( i < 3 )
		{
			while(( mem[pos] == 9 ) || ( mem[pos] == 32 ))
			{
				pos++;
			}

			if ( mem[pos++] != '.' )
			{
				goto bailout;
			}
		}
	}

	// --

	node = mem_AllocClr( sizeof( struct IPNode ));

	if ( ! node )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' Error allocating memory (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	node->ipn_Type = ( Cmd->ID == CFGID_IP_Allow ) ? IPT_Allow : IPT_Block ;
	node->ipn_A = a[0];
	node->ipn_B = a[1];
	node->ipn_C = a[2];
	node->ipn_D = a[3];

	ObtainSemaphore( & cfg->IPSema );
	List_AddTail( & cfg->IPList, (PTR) node );
	ReleaseSemaphore( & cfg->IPSema );

	// --

	if ( DoVerbose > 0 )
	{
		str[0] = 0;
		len = 0;

		for( i=0 ; i<4 ; i++ )
		{
			len = strlen( str );

			if ( a[i] == -1 )
			{
				sprintf( & str[len], "*" );
			}
			else
			{
				sprintf( & str[len], "%d", a[i] );
			}

			if ( i < 3 )
			{
				len = strlen( str );
				sprintf( & str[len], "." );
			}
		}

		if ( node->ipn_Type == IPT_Allow )
		{
			SHELLBUF_PRINTF2( " %s Adding : Allow IP %s\n", grpname, str );
		}
		else
		{
			SHELLBUF_PRINTF2( " %s Adding : Block IP %s\n", grpname, str );
		}
	}

	// --





	#if 0
struct IPNode *node;
char str[256];
S32 error;
S32 len;
S32 a[4];

	error = TRUE;

	if ( arglen <= 0 )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}


	#endif

	retval = TRUE;

bailout:

	return( retval );
}

// --
