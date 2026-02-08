
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// -- Parse Type : pstr - Buffer Pointer - strdup
#if 0

static S32 myType_pstr( struct Config *cfg, STR buf, S32 line, S32 argpos, S32 arglen, const STR grpname, const struct Cfg_Label *Cmd )
{
STR *str;
STR mem;
S32 error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	str = (PTR) ( (U32) cfg + Cmd->Offset );
	mem = Config_CopyString( & buf[argpos] );

	if ( ! mem )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' error copying string (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	if ( *str )
	{
		mem_Free( *str );
	}

	*str = mem;

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF3( " %s %s = '%s'\n", grpname, Cmd->Name, *str );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : bstr - Buffer String - snprintf

static S32 myType_bstr( struct Config *cfg, STR buf, S32 line, S32 argpos, S32 arglen, const STR grpname, const struct Cfg_Label *Cmd )
{
S32 error;
S32 max;
STR mem;
STR str;

	error = TRUE;

	if ( arglen <= 0 )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	str = (PTR) ( (U32) cfg + Cmd->Offset );
	max = ( Cmd->Arg2 > 0 ) ? Cmd->Arg2+1 : INT32_MAX ;
	mem = Config_CopyString( & buf[argpos] );

	if ( ! mem )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' error copying string (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	strncpy( str, mem, max );
	str[max-1] = 0;

	mem_Free( mem );

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF3( " %s %s = '%s'\n", grpname, Cmd->Name, str );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : gstr - Global String - snprintf

static S32 myType_gstr( struct Config *cfg UNUSED, STR buf, S32 line, S32 argpos, S32 arglen, const STR grpname, const struct Cfg_Label *Cmd )
{
S32 error;
S32 max;
STR mem;
STR str;

	error = TRUE;

	if ( arglen <= 0 )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	str = (PTR) ( Cmd->Pointer );
	max = ( Cmd->Arg2 > 0 ) ? Cmd->Arg2+1 : INT32_MAX ;
	mem = Config_CopyString( & buf[argpos] );

	if ( ! mem )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' error copying string (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	strncpy( str, mem, max );
	str[max-1] = 0;

	mem_Free( mem );

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF3( " %s %s = '%s'\n", grpname, Cmd->Name, str );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : ip4 include

static S32 myType_in_ip4( struct Config *cfg, STR buf, S32 line, S32 argpos, S32 arglen, const STR grpname, const struct Cfg_Label *Cmd )
{
struct IPNode *node;
char str[256];
S32 error;
S32 pos;
S32 len;
S32 a[4];
S32 i;

	error = TRUE;

	if ( arglen <= 0 )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	// --

	pos = argpos;

	for( i=0 ; i<4 ; i++ )
	{
		while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
		{
			pos++;
		}

		if ( buf[pos] == '*' )
		{
			a[i] = -1;

			pos++;
		}
		else
		{
			a[i] = atoi( & buf[pos] );

			if (( a[i] < 0 ) || ( a[i] > 255 ))
			{
				SHELLBUF_PRINTF3( "Config: %s '%s' invalid IP number (Line %d)\n", grpname, Cmd->Name, line );
				goto  bailout;
			}

			while(( buf[pos] >= '0' ) && ( buf[pos] <= '9' ))
			{
				pos++;
			}
		}

		if ( i < 3 )
		{
			while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
			{
				pos++;
			}

			if ( buf[pos++] != '.' )
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

	node->ipn_Type = IPT_Allow;
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

		SHELLBUF_PRINTF2( " %s Adding : Allow IP %s\n", grpname, str );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : ip4 exclude
// todo: merge with include.. but for now I did clone it

static S32 myType_ex_ip4( struct Config *cfg, STR buf, S32 line, S32 argpos, S32 arglen, const STR grpname, const struct Cfg_Label *Cmd )
{
struct IPNode *node;
char str[256];
S32 error;
S32 pos;
S32 len;
S32 a[4];
S32 i;

	error = TRUE;

	if ( arglen <= 0 )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	// --

	pos = argpos;

	for( i=0 ; i<4 ; i++ )
	{
		while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
		{
			pos++;
		}

		if ( buf[pos] == '*' )
		{
			a[i] = -1;

			pos++;
		}
		else
		{
			a[i] = atoi( & buf[pos] );

			if (( a[i] < 0 ) || ( a[i] > 255 ))
			{
				SHELLBUF_PRINTF3( "Config: %s '%s' invalid IP number (Line %d)\n", grpname, Cmd->Name, line );
				goto  bailout;
			}

			while(( buf[pos] >= '0' ) && ( buf[pos] <= '9' ))
			{
				pos++;
			}
		}

		if ( i < 3 )
		{
			while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
			{
				pos++;
			}

			if ( buf[pos++] != '.' )
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

	node->ipn_Type = IPT_Block;
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

		SHELLBUF_PRINTF2( " %s Adding : Block IP %s\n", grpname, str );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : vb

static S32 myType_vb( struct Config *cfg UNUSED, STR buf UNUSED, S32 line, S32 argpos UNUSED, S32 arglen, const STR grpname, const struct Cfg_Label *Cmd )
{
S32 val;
S32 error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	if (( val < Cmd->Arg1 ) || ( val > Cmd->Arg2 ))
	{
		SHELLBUF_PRINTF4( "Config: %s '%s' Value %" PRId32 " out of range (Line %d)\n", grpname, Cmd->Name, val, line );
		goto bailout;
	}

	if ( ! DoVerboseLocked )
	{
		DoVerbose = val;

		if ( DoVerbose > 0 )
		{
			SHELLBUF_PRINTF3( " %s %s = %" PRId32 "\n", grpname, Cmd->Name, val );
		}
	}	
	else
	{
		if ( DoVerbose > 0 )
		{
			SHELLBUF_PRINTF4( " %s %s = %" PRId32 " (Locked at %ld, skipping)\n", grpname, Cmd->Name, val, DoVerbose );
		}
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : u8

static S32 myType_u8( struct Config *cfg, STR buf, S32 line, S32 argpos, S32 arglen, const STR grpname, const struct Cfg_Label *Cmd )
{
U8 *ptr;
S32 val;
S32 error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	if (( val < Cmd->Arg1 ) || ( val > Cmd->Arg2 ))
	{
		SHELLBUF_PRINTF4( "Config: %s '%s' Value %" PRId32 " out of range (Line %d)\n", grpname, Cmd->Name, val, line );
		goto bailout;
	}

	ptr = (PTR) ( (U32) cfg + Cmd->Offset );

	*ptr = val;

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF3( " %s %s = %" PRId32 "\n", grpname, Cmd->Name, val );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : u32

static S32 myType_u32( struct Config *cfg, STR buf, S32 line, S32 argpos, S32 arglen, const STR grpname, const struct Cfg_Label *Cmd )
{
U32 *ptr;
U32 val;
S32 error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	if (( val < (U32)Cmd->Arg1 ) || ( val > (U32)Cmd->Arg2 ))
	{
		SHELLBUF_PRINTF4( "Config: %s '%s' Value %" PRIu32 " out of range (Line %" PRId32 ")\n", grpname, Cmd->Name, val, line );
		goto bailout;
	}

	ptr = (PTR) ( (U32) cfg + Cmd->Offset );

	*ptr = val;

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF3( " %s %s = %" PRIu32 "\n", grpname, Cmd->Name, val );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : s32

static S32 myType_s32( struct Config *cfg, STR buf, S32 line, S32 argpos, S32 arglen, const STR grpname, const struct Cfg_Label *Cmd )
{
S32 *ptr;
S32 val;
S32 error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	if (( val < Cmd->Arg1 ) || ( val > Cmd->Arg2 ))
	{
		SHELLBUF_PRINTF4( "Config: %s '%s' Value %" PRId32 " out of range (Line %d)\n", grpname, Cmd->Name, val, line );
		goto bailout;
	}

	ptr = (PTR) ( (U32) cfg + Cmd->Offset );

	*ptr = val;

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF3( " %s %s = %" PRId32 "\n", grpname, Cmd->Name, val );
	}

	error = FALSE;

bailout:

	return( error );
}

#endif
// -- Parse Group Commands
#if 0

static S32 myParseCommands( struct Config *cfg, STR buf, S32 *p, S32 *l, const STR grpname, const struct Cfg_Label *Cmds )
{
S32 (*func)( struct Config *cfg, STR buf, S32 line, S32 argpos, S32 arglen, const STR grpname, const struct Cfg_Label *Cmd );
S32 lastline;
S32 lastpos;
S32 cmdlen;
S32 arglen;
S32 argpos;
S32 error;
S32 line;
S32 pos;
S32 cnt;
S32 c;
S32 i;

	error = TRUE;
	line = *l;
	pos = *p;

	lastline = line;
	lastpos = pos;

	// Next Line
	// Skip [] Command line

	while(( buf[pos] ) && ( buf[pos] != 10 ))
	{
		pos++;
	}

	while( buf[pos] == 10 )
	{
		line++;
		pos++;
	}

	// --

	while( buf[pos] )
	{
		// Find Start of line, skip Tabs, Space

		while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
		{
			pos++;
		}

 		if ( buf[pos] == '[' )
 		{
			line	= lastline;
			pos		= lastpos;
 			break;
 		}

		if ( buf[pos] != ';' )
		{
			cmdlen = pos;

			while(	( buf[cmdlen] != 0 )
			&&		( buf[cmdlen] != 9 )
			&&		( buf[cmdlen] != 10 )
			&&		( buf[cmdlen] != 13 )
			&&		( buf[cmdlen] != 32 ))
			{
				cmdlen++;
			}

			cmdlen -= pos;

			if (( cmdlen == 3 ) && ( Func_Strnicmp( & buf[pos], "end", 3 ) == 0 ))
			{
				break;
			}

			argpos = pos + cmdlen;

			while(( buf[argpos] == 9 ) || ( buf[argpos] == 32 ))
			{
				argpos++;
			}

 			if ( buf[argpos] == '=' )
 			{
 				argpos++;

 				while(( buf[argpos] == 9 ) || ( buf[argpos] == 32 ))
 				{
 					argpos++;
 				}

 				arglen = argpos;

 				while(	( buf[arglen] != 0 )
 				&&		( buf[arglen] != 9 )
 				&&		( buf[arglen] != 10 )
 				&&		( buf[arglen] != 13 )
 				&&		( buf[arglen] != 32 ))
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
 				&&	( Func_Strnicmp( Cmds[cnt].Name, & buf[pos], cmdlen ) == 0 ))
 				{
 					break;
 				}
 				else
 				{
 					cnt++;
 				}
 			}

			if ( Cmds[cnt].Len )
			{
				switch( Cmds[cnt].Type )
				{
					case CLT_Config_vb:		{ func = myType_vb; break; }
					case CLT_Config_u8:		{ func = myType_u8; break; }
					case CLT_Config_u32:	{ func = myType_u32; break; }
					case CLT_Config_s32:	{ func = myType_s32; break; }
					case CLT_Config_gstr:	{ func = myType_gstr; break; }
					case CLT_Config_bstr:	{ func = myType_bstr; break; }
					case CLT_Config_pstr:	{ func = myType_pstr; break; }
					case CLT_Config_in_ip4:	{ func = myType_in_ip4; break; }
					case CLT_Config_ex_ip4:	{ func = myType_ex_ip4; break; }

					default:
					{
						SHELLBUF_PRINTF4( "%s:%04d: Error unknown type (%d) for '%s'\n", __FILE__, __LINE__, Cmds[cnt].Type, Cmds[cnt].Name );
						goto bailout;
					}
				}

				if ( func( cfg, buf, line, argpos, arglen, grpname, & Cmds[cnt] ))
				{
					SHELLBUF_PRINTF2( "%s:%04d: Error\n", __FILE__, __LINE__ );
					goto bailout;
				}
			}
			else
			{
				if ( DoVerbose > 0 )
				{
					SHELLBUF_PRINTF( "Unknown Command skipping '" );

					for( i=0 ; i < 32 ; i++ )
					{
						c = buf[pos+i];

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
							SHELLBUF_PRINTF1( "%c", c );
						}
					}
					
					SHELLBUF_PRINTF1( "' (Line %d)\n", line );
				}
			}
		}

		lastline = line;
		lastpos = pos;

		// Next Line

		while(( buf[pos] ) && ( buf[pos] != 10 ))
		{
			pos++;
		}

		while( buf[pos] == 10 )
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

#endif
// -- Parse Group Commands

S32 Config_Read_IP( struct Config *cfg, const struct CFGNODE *Cmd, const STR grpname, STR mem, S32 line );
S32 Config_Read_VAL( struct Config *cfg, const struct CFGNODE *Cmd, const STR grpname, STR mem, S32 line );
S32 Config_Read_STR( struct Config *cfg, const struct CFGNODE *Cmd, const STR grpname, STR mem, S32 line );

static S32 myParseCommands2( struct Config *cfg, STR buf, S32 *p, S32 *l, const STR grpname, const struct CFGNODE *Cmds )
{
S32 lastline;
S32 lastpos;
S32 cmdlen;
S32 arglen;
S32 argpos;
S32 error;
S32 line;
S32 pos;
S32 cnt;
S32 c;
S32 i;

	error = TRUE;
	line = *l;
	pos = *p;

	lastline = line;
	lastpos = pos;

	// Next Line
	// Skip [] Command line

	while(( buf[pos] ) && ( buf[pos] != 10 ))
	{
		pos++;
	}

	while( buf[pos] == 10 )
	{
		line++;
		pos++;
	}

	// --

	while( buf[pos] )
	{
		// Find Start of line, skip Tabs, Space

		while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
		{
			pos++;
		}

 		if ( buf[pos] == '[' )
 		{
			line	= lastline;
			pos		= lastpos;
 			break;
 		}

		if ( buf[pos] != ';' )
		{
			cmdlen = pos;

			while(	( buf[cmdlen] != 0 )
			&&		( buf[cmdlen] != 9 )
			&&		( buf[cmdlen] != 10 )
			&&		( buf[cmdlen] != 13 )
			&&		( buf[cmdlen] != 32 ))
			{
				cmdlen++;
			}

			cmdlen -= pos;

			if (( cmdlen == 3 ) && ( Func_Strnicmp( & buf[pos], "end", 3 ) == 0 ))
			{
				break;
			}

			argpos = pos + cmdlen;

			while(( buf[argpos] == 9 ) || ( buf[argpos] == 32 ))
			{
				argpos++;
			}

 			if ( buf[argpos] == '=' )
 			{
 				argpos++;

 				while(( buf[argpos] == 9 ) || ( buf[argpos] == 32 ))
 				{
 					argpos++;
 				}

 				arglen = argpos;

 				while(	( buf[arglen] != 0 )
 				&&		( buf[arglen] != 9 )
 				&&		( buf[arglen] != 10 )
 				&&		( buf[arglen] != 13 )
 				&&		( buf[arglen] != 32 ))
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
 				if (( Cmds[cnt].NameLen == cmdlen )
 				&&	( Func_Strnicmp( Cmds[cnt].Name, & buf[pos], cmdlen ) == 0 ))
 				{
 					break;
 				}
 				else
 				{
 					cnt++;
 				}
 			}

			if ( Cmds[cnt].Name )
			{
				switch( Cmds[cnt].Type )
				{
					case CFGTYPE_IP:
					{
						if ( ! Config_Read_IP( cfg, & Cmds[cnt], grpname, & buf[argpos], line ))
						{
							goto bailout;
						}
						break;
					}

					case CFGTYPE_VAL:
					{
						if ( ! Config_Read_VAL( cfg, & Cmds[cnt], grpname, & buf[argpos], line ))
						{
							goto bailout;
						}
						break;
					}

					case CFGTYPE_STR:
					{
						if ( ! Config_Read_STR( cfg, & Cmds[cnt], grpname, & buf[argpos], line ))
						{
							goto bailout;
						}
						break;
					}

					default:
					{
						SHELLBUF_PRINTF1( "UNK : '%s'\n", Cmds[cnt].Name );
						break;
					}
				}
			}
			else
			{
				if ( DoVerbose > 0 )
				{
					SHELLBUF_PRINTF( "Unknown Command skipping '" );

					for( i=0 ; i < 32 ; i++ )
					{
						c = buf[pos+i];

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
							SHELLBUF_PRINTF1( "%c", c );
						}
					}
					
					SHELLBUF_PRINTF1( "' (Line %d)\n", line );
				}
			}
		}

		lastline = line;
		lastpos = pos;

		// Next Line

		while(( buf[pos] ) && ( buf[pos] != 10 ))
		{
			pos++;
		}

		while( buf[pos] == 10 )
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

// -- Parse File

S32 Config_Read( struct Config *cfg, STR Filename, S32 CfgNeeded )
{
S32 realsize;
STR buf;
S32 filesize;
S32 error;
S32 line;
S32 pos;
S32 len;
S32 cnt;
S32 h;
S32 c;
S32 i;

	h = 0;

	buf = NULL;

	error = TRUE;

	if (( ! cfg ) || ( cfg->cfg_StructID != ID_CFG ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error invalid Config Pointer" );
		goto bailout;
	}

	if ( ! Filename )
	{
		Filename = "rVNCd.config";
	}

	h = Open( Filename, MODE_OLDFILE );

	if ( ! h )
	{
		if ( CfgNeeded )
		{
			SHELLBUF_PRINTF2( "%s:%04d: Error\n", __FILE__, __LINE__ );

			Log_PrintF( cfg, LOGTYPE_Error, "Error opening file '%s'", Filename );
		}
		else
		{
			error = FALSE;

			if ( DoVerbose > 1 )
			{
				Log_PrintF( cfg, LOGTYPE_Info, "Config file '%s' not found", Filename );
			}
		}
		goto bailout;
	}

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF1( "Loading '%s' Config file\n", Filename );
	}

	if ( cfg->cfg_Config_Filename )
	{
		mem_Free( cfg->cfg_Config_Filename );
	}

	cfg->cfg_Config_Filename = mem_Strdup( Filename );

	Func_GetFileSize( cfg->cfg_Config_Filename, & realsize );

	if ( realsize < 0 )
	{
		SHELLBUF_PRINTF2( "%s:%04d: Error\n", __FILE__, __LINE__ );
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining file size for '%s'", Filename );
		goto bailout;
	}

	if ( ! realsize )
	{
		SHELLBUF_PRINTF2( "%s:%04d: Error\n", __FILE__, __LINE__ );
		Log_PrintF( cfg, LOGTYPE_Error, "Error file size is Zero for '%s'", Filename );
		goto bailout;
	}

	filesize = realsize;

	buf = mem_AllocClr( realsize+1 );

	if ( ! buf )
	{
		SHELLBUF_PRINTF2( "%s:%04d: Error\n", __FILE__, __LINE__ );
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
		goto bailout;
	}

	len = Read( h, buf, filesize );

	if ( len != filesize )
	{
		SHELLBUF_PRINTF2( "%s:%04d: Error\n", __FILE__, __LINE__ );
		Log_PrintF( cfg, LOGTYPE_Error, "Error reading file '%s'", Filename );
		goto bailout;
	}

	// ------
	// -- Need to remove IP's as they do not get over written

	#if 0
	while(( node = List_RemHead( & cfg->IPList )))
	{
		mem_Free( node );
	}
	#endif

	// ------
	// -- Parse File

	len = strlen( ConfigHeaderStr );

	if ( Func_Strnicmp( buf, ConfigHeaderStr, len ))
	{
		SHELLBUF_PRINTF2( "%s:%04d: Error\n", __FILE__, __LINE__ );
		Log_PrintF( cfg, LOGTYPE_Error, "Not a valid Config file" );
		goto bailout;
	}

	pos = len;
	line = 2;

	// --

	while(( buf[pos] ) && ( buf[pos] != 10 ))
	{
		pos++;
	}

	while( buf[pos] == 10 )
	{
		line++;
		pos++;
	}

	// --

	while( buf[pos] )
	{
		// Find Start of line, skip Tabs, Space and NL's

		while(( buf[pos] == 9 ) || ( buf[pos] == 32 ))
		{
			pos++;
		}

		if (( buf[pos] == 0 )
		||	( buf[pos] == 10 )
		||	( buf[pos] == 13 )
		||	( buf[pos] == ';' ))
		{
			// Comment line, End of Line, End of File
		}

		else if ( buf[pos] == '[' )
		{
			len = pos;

			while(	( buf[len] != 0 )
			&&		( buf[len] != 9 )
			&&		( buf[len] != 10 )
			&&		( buf[len] != 13 )
			&&		( buf[len] != 32 ))
			{
				len++;
			}

	 		len -= pos;

			cnt = 0;

			while( ConfigGroups2[cnt].Name )
			{
				if (( ConfigGroups2[cnt].NameLen == len ) 
				&&	( Func_Strnicmp( ConfigGroups2[cnt].Name, & buf[pos], ConfigGroups2[cnt].NameLen ) == 0 ))
				{
					break;
				}
				else
				{
					cnt++;
				}
			}

			if ( ConfigGroups2[cnt].Cmds )
			{
				if ( myParseCommands2( cfg, buf, & pos, & line, ConfigGroups2[cnt].Name, ConfigGroups2[cnt].Cmds ))
				{
					SHELLBUF_PRINTF2( "%s:%04d: Error\n", __FILE__, __LINE__ );
					goto bailout;
				}
			}
			else
			{
				if ( DoVerbose > 0 )
				{
					SHELLBUF_PRINTF( "Unknown Group skipping '" );

					for( i=0 ; i < 32 ; i++ )
					{
						c = buf[pos+i];

						if (( c == 0 )
						||	( c == 9 )
						||	( c == 10 )
						||	( c == 13 )
						||	( c == 32 )
						||	( c == ']' ))
						{
							if ( c == ']' )
							{
								SHELLBUF_PRINTF1( "%c", c );
							}
							break;
						}
						else
						{
							SHELLBUF_PRINTF1( "%c", c );
						}
					}
					
					SHELLBUF_PRINTF1( "' (Line %d)\n", line );
				}
			}
		}

		// Next Line

		while(( buf[pos] ) && ( buf[pos] != 10 ))
		{
			pos++;
		}

		while( buf[pos] == 10 )
		{
			line++;
			pos++;
		}
	}

	// --

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF1( "Loaded '%s' config\n", FilePart( Filename ));
	}

	error = FALSE;

bailout:

	if ( h )
	{
		Close( h );
	}

	if ( buf )
	{
		mem_Free( buf );
	}

	return( error );
}

// --
