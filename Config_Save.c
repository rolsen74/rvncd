
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

static int myType_gstr( struct Config *cfg UNUSED, char *buf UNUSED, BPTR h, const struct Cfg_Label *Cmd )
{
char *str;
char *mem;
int error;
int stat;
int len;

	error = TRUE;

	str = (APTR) ( Cmd->Pointer );

	mem = myASPrintF( " %s = \"%s\"\n", Cmd->Name, str );

	if ( ! mem )
	{
		goto bailout;
	}

	len = strlen( mem );

	stat = IDOS->Write( h, mem, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	if ( mem )
	{
		myFree( mem );
	}

	return( error );
}

// --

static int myType_bstr( struct Config *cfg, char *buf UNUSED, BPTR h, const struct Cfg_Label *Cmd )
{
char *str;
char *mem;
int error;
int stat;
int len;

	error = TRUE;

	str = (APTR) ( (uint32) cfg + Cmd->Offset );

	mem = myASPrintF( " %s = \"%s\"\n", Cmd->Name, str );

	if ( ! mem )
	{
		goto bailout;
	}

	len = strlen( mem );

	stat = IDOS->Write( h, mem, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	if ( mem )
	{
		myFree( mem );
	}

	return( error );
}

// --

static int myType_pstr( struct Config *cfg, char *buf UNUSED, BPTR h, const struct Cfg_Label *Cmd )
{
char **strptr;
char *str;
char *mem;
int error;
int stat;
int len;

	error = TRUE;

	strptr = (APTR) ( (uint32) cfg + Cmd->Offset );

	str = ( *strptr ) ? *strptr : "";

	mem = myASPrintF( " %s = \"%s\"\n", Cmd->Name, str );

	if ( ! mem )
	{
		goto bailout;
	}

	len = strlen( mem );

	stat = IDOS->Write( h, mem, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	if ( mem )
	{
		myFree( mem );
	}

	return( error );
}

// --

static int myType_u8( struct Config *cfg, char *buf, BPTR h, const struct Cfg_Label *Cmd )
{
uint8 *u8;
int error;
int stat;
int len;

	error = TRUE;

	u8 = (APTR) ( (uint32) cfg + Cmd->Offset );

	sprintf( buf, " %s = %d\n", Cmd->Name, *u8 );

	len = strlen( buf );

	stat = IDOS->Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myType_u32( struct Config *cfg, char *buf, BPTR h, const struct Cfg_Label *Cmd )
{
uint32 *u32;
int error;
int stat;
int len;

	error = TRUE;

	u32 = (APTR) ( (uint32) cfg + Cmd->Offset );

	sprintf( buf, " %s = %ld\n", Cmd->Name, *u32 );

	len = strlen( buf );

	stat = IDOS->Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myType_s32( struct Config *cfg, char *buf, BPTR h, const struct Cfg_Label *Cmd )
{
int32 *s32;
int error;
int stat;
int len;

	error = TRUE;

	s32 = (APTR) ( (uint32) cfg + Cmd->Offset );

	sprintf( buf, " %s = %ld\n", Cmd->Name, *s32 );

	len = strlen( buf );

	stat = IDOS->Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myType_vb( struct Config *cfg UNUSED, char *buf, BPTR h, const struct Cfg_Label *Cmd UNUSED )
{
int error;
int stat;
int len;

	error = TRUE;

	if ( DoVerbose )
	{
		sprintf( buf, " Verbose = 1\n" );
	}
	else
	{
		// Writing 0 will over write the -v shell option
		// so we do not set it by default
		sprintf( buf, "; Verbose = 1\n" );
	}

	len = strlen( buf );

	stat = IDOS->Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myGroup( struct Config *cfg, BPTR h, char *buf, const char *grpname, const struct Cfg_Label *label )
{
int (*func)( struct Config *cfg, char *buf, BPTR h, const struct Cfg_Label *Cmd );
int error;
int stat;
int len;
int pos;

	error = TRUE;

	// -- Write Header

	sprintf( buf, "\n%s\n", grpname );

	len = strlen( buf );

	stat = IDOS->Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	pos = 0;

	while( label[pos].Name )
	{
		switch( label[pos].Type )
		{
//			case CLT_Config_ip:		{ func = myType_ip; break; }
			case CLT_Config_vb:		{ func = myType_vb; break; }
			case CLT_Config_u8:		{ func = myType_u8; break; }
			case CLT_Config_u32:	{ func = myType_u32; break; }
			case CLT_Config_s32:	{ func = myType_s32; break; }
			case CLT_Config_gstr:	{ func = myType_gstr; break; }
			case CLT_Config_bstr:	{ func = myType_bstr; break; }
			case CLT_Config_pstr:	{ func = myType_pstr; break; }

			default:
			{
				printf( "%s:%04d: Error unknown type (%d) for '%s'\n", __FILE__, __LINE__, label[pos].Type, label[pos].Name );
				goto bailout;
			}
		}

		if ( func( cfg, buf, h, & label[pos] ))
		{
			goto bailout;
		}

		pos++;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static int myGroups( struct Config *cfg, BPTR h, char *buf )
{
char *str;
int err;
int pos;

	err = TRUE;
	pos = 0;

	while( TRUE )
	{
		str = (APTR) ConfigGroups[pos].Name;

		if ( ! str )
		{
			break;
		}

		if (( strcmp( str, "[BLACKLIST]" )) 
		&&  ( strcmp( str, "[WHITELIST]" )))
		{
			if ( myGroup( cfg, h, buf, str, ConfigGroups[pos].Cmds ))
			{
				goto bailout;
			}
		}

		pos++;
	}

	err = FALSE;

bailout:

	return( err );
}

// --

static int myList( struct Config *cfg UNUSED, char *buf, char *name, struct List *list, BPTR h )
{
struct IPNode *node;
int error;
int stat;
int len;

	error = TRUE;

	sprintf( buf, "\n[%s]\n", name );

	len = strlen( buf );

	stat = IDOS->Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	node = (APTR) IExec->GetHead( list );

	while( node )
	{
		sprintf( buf, " IP = " );

		// -- A

		len = strlen( buf );

		if ( node->ipn_A == -1 )
		{
			sprintf( & buf[len], "*" );
		}
		else
		{
			sprintf( & buf[len], "%d", node->ipn_A );
		}

		// -- B

		len = strlen( buf );

		if ( node->ipn_B == -1 )
		{
			sprintf( & buf[len], ".*" );
		}
		else
		{
			sprintf( & buf[len], ".%d", node->ipn_B );
		}

		// -- C

		len = strlen( buf );

		if ( node->ipn_C == -1 )
		{
			sprintf( & buf[len], ".*" );
		}
		else
		{
			sprintf( & buf[len], ".%d", node->ipn_C );
		}

		// -- D

		len = strlen( buf );

		if ( node->ipn_D == -1 )
		{
			sprintf( & buf[len], ".*" );
		}
		else
		{
			sprintf( & buf[len], ".%d", node->ipn_D );
		}

		// -- Config nr

		len = strlen( buf );

		sprintf( & buf[len], "\n" );

		// -- Write

		len = strlen( buf );

		stat = IDOS->Write( h, buf, len );

		if ( stat != len )
		{
			goto bailout;
		}

		// --

		node = (APTR) IExec->GetSucc( (APTR) node );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

int Config_Save( struct Config *cfg, char *Filename )
{
char *tmpname;
char *buf;
int error;
int stat;
int len;
int h;

	h = 0;

	buf = NULL;

	error = TRUE;

	tmpname = NULL;

	if (( cfg == NULL ) || ( cfg->cfg_ID != ID_CFG ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error invalid Config Pointer" );
		goto bailout;
	}

	if ( ! Filename )
	{
		Filename = "RVNCd.config";
	}

	buf = myMalloc( 1024 );

	if ( ! buf )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
		goto bailout;
	}

	tmpname = myGetTempFilename( Filename );

	if ( ! tmpname )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
		goto bailout;
	}

	h = IDOS->Open( tmpname, MODE_NEWFILE );

	if ( ! h )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening file '%s'", Filename );
		goto bailout;
	}

	/*
	** File Header
	*/

	len = strlen( ConfigHeaderStr );

	stat = IDOS->Write( h, ConfigHeaderStr, len );

	if ( stat != len )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error writing to file" );
		goto bailout;
	}

	/*
	** Groups
	*/

	if ( myGroups( cfg, h, buf ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error writing to file" );
		goto bailout;
	}

	if ( myList( cfg, buf, "WhiteList", & cfg->WhiteList, h ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error writing to file" );
		goto bailout;
	}

	if ( myList( cfg, buf, "BlackList", & cfg->BlackList, h ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error writing to file" );
		goto bailout;
	}

	// --

	if ( ! mySameString( cfg->cfg_Config_Filename, Filename ))
	{
		if ( cfg->cfg_Config_Filename )
		{
			myFree( cfg->cfg_Config_Filename );
		}

		cfg->cfg_Config_Filename = myStrdup( Filename );
	}

	// --

	Log_PrintF( cfg, LOGTYPE_Info, "Config saved to %s", Filename );

	// --

	error = FALSE;

bailout:

	if ( h )
	{
		IDOS->Close( h );

		// --

		if ( ! error )
		{
			// Todo check if Delete fails
			IDOS->Delete( Filename );
			IDOS->Rename( tmpname, Filename );
		}
		else
		{
			IDOS->Delete( tmpname );
		}
	}

	if ( tmpname )
	{
		myFree( tmpname );
	}

	if ( buf )
	{
		myFree( buf );
	}

	return( error );
}

// --
