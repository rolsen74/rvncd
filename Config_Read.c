
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// -- Parse Type : pstr - Buffer Pointer - strdup

static int myType_pstr( struct Config *cfg, char *buf, int line, int argpos, int arglen, const char *grpname, const struct Cfg_Label *Cmd )
{
char **str;
char *mem;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	str = (APTR) ( (uint32) cfg + Cmd->Offset );
	mem = Config_CopyString( & buf[argpos] );

	if ( ! mem )
	{
		printf( "Config: %s '%s' error copying string (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	if ( *str )
	{
		myFree( *str );
	}

	*str = mem;

	if ( DoVerbose )
	{
		printf( " %s %s = '%s'\n", grpname, Cmd->Name, *str );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : bstr - Buffer String - snprintf

static int myType_bstr( struct Config *cfg, char *buf, int line, int argpos, int arglen, const char *grpname, const struct Cfg_Label *Cmd )
{
int error;
int max;
char *mem;
char *str;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	str = (APTR) ( (uint32) cfg + Cmd->Offset );
	max = ( Cmd->Arg2 > 0 ) ? Cmd->Arg2 : INT32_MAX ;
	mem = Config_CopyString( & buf[argpos] );

	if ( ! mem )
	{
		printf( "Config: %s '%s' error copying string (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	strncpy( str, mem, max );
	str[max-1] = 0;

	myFree( mem );

	if ( DoVerbose )
	{
		printf( " %s %s = '%s'\n", grpname, Cmd->Name, str );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : gstr - Global String - snprintf

static int myType_gstr( struct Config *cfg UNUSED, char *buf, int line, int argpos, int arglen, const char *grpname, const struct Cfg_Label *Cmd )
{
int error;
int max;
char *mem;
char *str;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	str = (APTR) ( Cmd->Pointer );
	max = ( Cmd->Arg2 > 0 ) ? Cmd->Arg2 : INT32_MAX ;
	mem = Config_CopyString( & buf[argpos] );

	if ( ! mem )
	{
		printf( "Config: %s '%s' error copying string (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	strncpy( str, mem, max );
	str[max-1] = 0;

	myFree( mem );

	if ( DoVerbose )
	{
		printf( " %s %s = '%s'\n", grpname, Cmd->Name, str );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : ip

static int myType_ip( struct Config *cfg, char *buf, int line, int argpos, int arglen, const char *grpname, const struct Cfg_Label *Cmd )
{
struct IPNode *node;
char str[256];
int error;
int pos;
int len;
int a[4];
int i;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
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
				printf( "Config: %s '%s' invalid IP number (Line %d)\n", grpname, Cmd->Name, line );
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

	node = myCalloc( sizeof( struct IPNode ));

	if ( ! node )
	{
		printf( "Config: %s '%s' Error allocating memory (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	node->ipn_A = a[0];
	node->ipn_B = a[1];
	node->ipn_C = a[2];
	node->ipn_D = a[3];

	/**/ if ( Cmd->Arg1 == CLT_B )
	{
		IExec->AddTail( & cfg->BlackList, (APTR) node );
	}
	else if ( Cmd->Arg1 == CLT_W )
	{
		IExec->AddTail( & cfg->WhiteList, (APTR) node );
	}
	else
	{
		printf( "Config: %s '%s' invalid list (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	// --

	if ( DoVerbose )
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

		printf( " %s Adding IP %s\n", grpname, str );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : vb

static int myType_vb( struct Config *cfg UNUSED, char *buf UNUSED, int line, int argpos UNUSED, int arglen, const char *grpname, const struct Cfg_Label *Cmd )
{
int32 val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	if (( val < Cmd->Arg1 ) || ( val > Cmd->Arg2 ))
	{
		printf( "Config: %s '%s' Value %ld out of range (Line %d)\n", grpname, Cmd->Name, val, line );
		goto bailout;
	}

	DoVerbose = val;

	printf( " %s %s = %ld\n", grpname, Cmd->Name, val );

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : u8

static int myType_u8( struct Config *cfg, char *buf, int line, int argpos, int arglen, const char *grpname, const struct Cfg_Label *Cmd )
{
uint8 *ptr;
int32 val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	if (( val < Cmd->Arg1 ) || ( val > Cmd->Arg2 ))
	{
		printf( "Config: %s '%s' Value %ld out of range (Line %d)\n", grpname, Cmd->Name, val, line );
		goto bailout;
	}

	ptr = (APTR) ( (uint32) cfg + Cmd->Offset );

	*ptr = val;

	if ( DoVerbose )
	{
		printf( " %s %s = %ld\n", grpname, Cmd->Name, val );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : u32

static int myType_u32( struct Config *cfg, char *buf, int line, int argpos, int arglen, const char *grpname, const struct Cfg_Label *Cmd )
{
uint32 *ptr;
uint32 val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	if (( val < (uint32)Cmd->Arg1 ) || ( val > (uint32)Cmd->Arg2 ))
	{
		printf( "Config: %s '%s' Value %ld out of range (Line %d)\n", grpname, Cmd->Name, val, line );
		goto bailout;
	}

	ptr = (APTR) ( (uint32) cfg + Cmd->Offset );

	*ptr = val;

	if ( DoVerbose )
	{
		printf( " %s %s = %ld\n", grpname, Cmd->Name, val );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Type : s32

static int myType_s32( struct Config *cfg, char *buf, int line, int argpos, int arglen, const char *grpname, const struct Cfg_Label *Cmd )
{
int32 *ptr;
int32 val;
int error;

	error = TRUE;

	if ( arglen <= 0 )
	{
		printf( "Config: %s '%s' argument missing (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	val = atoi( & buf[argpos] );

	if (( val < Cmd->Arg1 ) || ( val > Cmd->Arg2 ))
	{
		printf( "Config: %s '%s' Value %ld out of range (Line %d)\n", grpname, Cmd->Name, val, line );
		goto bailout;
	}

	ptr = (APTR) ( (uint32) cfg + Cmd->Offset );

	*ptr = val;

	if ( DoVerbose )
	{
		printf( " %s %s = %ld\n", grpname, Cmd->Name, val );
	}

	error = FALSE;

bailout:

	return( error );
}

// -- Parse Group Commands

static int myParseCommands( struct Config *cfg, char *buf, int *p, int *l, const char *grpname, const struct Cfg_Label *Cmds )
{
int (*func)( struct Config *cfg, char *buf, int line, int argpos, int arglen, const char *grpname, const struct Cfg_Label *Cmd );
int lastline;
int lastpos;
int cmdlen;
int arglen;
int argpos;
int error;
int line;
int pos;
int cnt;
int c;
int i;

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

			if (( cmdlen == 3 ) && ( strnicmp( & buf[pos], "end", 3 ) == 0 ))
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
 				&&	( strnicmp( Cmds[cnt].Name, & buf[pos], cmdlen ) == 0 ))
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
					case CLT_Config_ip:		{ func = myType_ip; break; }
					case CLT_Config_vb:		{ func = myType_vb; break; }
					case CLT_Config_u8:		{ func = myType_u8; break; }
					case CLT_Config_u32:	{ func = myType_u32; break; }
					case CLT_Config_s32:	{ func = myType_s32; break; }
					case CLT_Config_gstr:	{ func = myType_gstr; break; }
					case CLT_Config_bstr:	{ func = myType_bstr; break; }
					case CLT_Config_pstr:	{ func = myType_pstr; break; }

					default:
					{
						printf( "%s:%04d: Error unknown type (%d) for '%s'\n", __FILE__, __LINE__, Cmds[cnt].Type, Cmds[cnt].Name );
						goto bailout;
					}
				}

				if ( func( cfg, buf, line, argpos, arglen, grpname, & Cmds[cnt] ))
				{
					printf( "%s:%04d: Error\n", __FILE__, __LINE__ );
					goto bailout;
				}
			}
			else
			{
				if ( DoVerbose )
				{
					printf( "Unknown Command skipping '" );

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
							printf( "%c", c );
						}
					}
					
					printf( "' (Line %d)\n", line );
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

int Config_Read( struct Config *cfg, char *Filename, int CfgNeeded )
{
int64 realsize;
//void *node;
char *buf;
int filesize;
int error;
int line;
int pos;
int len;
int cnt;
int h;
int c;
int i;

	h = 0;

	buf = NULL;

	error = TRUE;

	if (( cfg == NULL ) || ( cfg->cfg_ID != ID_CFG ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error invalid Config Pointer" );
		goto bailout;
	}

	if ( ! Filename )
	{
		Filename = "RVNCd.config";
	}

	if ( DoVerbose )
	{
		printf( "Loading '%s' Config file\n", Filename );
	}

	h = IDOS->Open( Filename, MODE_OLDFILE );

	if ( ! h )
	{
		if ( CfgNeeded )
		{
			printf( "%s:%04d: Error\n", __FILE__, __LINE__ );
			Log_PrintF( cfg, LOGTYPE_Error, "Error opening file '%s'", Filename );
		}
		else
		{
			error = FALSE;
			Log_PrintF( cfg, LOGTYPE_Info, "Config file '%s' not found", Filename );
		}
		goto bailout;
	}

	if ( cfg->cfg_Config_Filename )
	{
		myFree( cfg->cfg_Config_Filename );
	}

	cfg->cfg_Config_Filename = myStrdup( Filename );

	realsize = IDOS->GetFileSize( h );

	if ( realsize < 0 )
	{
		printf( "%s:%04d: Error\n", __FILE__, __LINE__ );
		Log_PrintF( cfg, LOGTYPE_Error, "Error obtaining file size for '%s'", Filename );
		goto bailout;
	}

	if ( ! realsize )
	{
		printf( "%s:%04d: Error\n", __FILE__, __LINE__ );
		Log_PrintF( cfg, LOGTYPE_Error, "Error file size is Zero for '%s'", Filename );
		goto bailout;
	}

	filesize = realsize;

	buf = myCalloc( filesize+1 );

	if ( ! buf )
	{
		printf( "%s:%04d: Error\n", __FILE__, __LINE__ );
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
		goto bailout;
	}

	len = IDOS->Read( h, buf, filesize );

	if ( len != filesize )
	{
		printf( "%s:%04d: Error\n", __FILE__, __LINE__ );
		Log_PrintF( cfg, LOGTYPE_Error, "Error reading file '%s'", Filename );
		goto bailout;
	}

	// ------
	// -- Need to remove IP's as they do not get over written

	#if 0
	while(( node = IExec->RemHead( & cfg->BlackList )))
	{
		myFree( node );
	}

	while(( node = IExec->RemHead( & cfg->WhiteList )))
	{
		myFree( node );
	}
	#endif

	// ------
	// -- Parse File

	len = strlen( ConfigHeaderStr );

	if ( strnicmp( buf, ConfigHeaderStr, len ))
	{
		printf( "%s:%04d: Error\n", __FILE__, __LINE__ );
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

			while( ConfigGroups[cnt].Name )
			{
				if (( ConfigGroups[cnt].Len == len ) 
				&&	( strnicmp( ConfigGroups[cnt].Name, & buf[pos], ConfigGroups[cnt].Len ) == 0 ))
				{
					break;
				}
				else
				{
					cnt++;
				}
			}

			if ( ConfigGroups[cnt].Cmds )
			{
				if ( myParseCommands( cfg, buf, & pos, & line, ConfigGroups[cnt].Name, ConfigGroups[cnt].Cmds ))
				{
					printf( "%s:%04d: Error\n", __FILE__, __LINE__ );
					goto bailout;
				}
			}
			else
			{
				if ( DoVerbose )
				{
					printf( "Unknown Group skipping '" );

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
								printf( "%c", c );
							}
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

	if ( DoVerbose )
	{
		printf( "Loaded '%s' config\n", IDOS->FilePart( Filename ));
	}

	error = FALSE;

bailout:

	if ( h )
	{
		IDOS->Close( h );
	}

	if ( buf )
	{
		myFree( buf );
	}

	return( error );
}

// --
