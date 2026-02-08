
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --
#if 0

static S32 myType_gstr( struct Config *cfg UNUSED, STR buf UNUSED, BPTR h, const struct Cfg_Label *Cmd )
{
STR str;
STR mem;
S32 error;
S32 stat;
S32 len;

	error = TRUE;

	str = (PTR) ( Cmd->Pointer );

	mem = mem_ASPrintF( " %s = \"%s\"\n", Cmd->Name, str );

	if ( ! mem )
	{
		goto bailout;
	}

	len = strlen( mem );

	stat = Write( h, mem, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	if ( mem )
	{
		mem_Free( mem );
	}

	return( error );
}

// --

static S32 myType_bstr( struct Config *cfg, STR buf UNUSED, BPTR h, const struct Cfg_Label *Cmd )
{
STR str;
STR mem;
S32 error;
S32 stat;
S32 len;

	error = TRUE;

	str = (PTR) ( (U32) cfg + Cmd->Offset );

	mem = mem_ASPrintF( " %s = \"%s\"\n", Cmd->Name, str );

	if ( ! mem )
	{
		goto bailout;
	}

	len = strlen( mem );

	stat = Write( h, mem, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	if ( mem )
	{
		mem_Free( mem );
	}

	return( error );
}

// --

static S32 myType_pstr( struct Config *cfg, STR buf UNUSED, BPTR h, const struct Cfg_Label *Cmd )
{
STR*strptr;
STR str;
STR mem;
S32 error;
S32 stat;
S32 len;

	error = TRUE;

	strptr = (PTR) ( (U32) cfg + Cmd->Offset );

	str = ( *strptr ) ? *strptr : "";

	mem = mem_ASPrintF( " %s = \"%s\"\n", Cmd->Name, str );

	if ( ! mem )
	{
		goto bailout;
	}

	len = strlen( mem );

	stat = Write( h, mem, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	if ( mem )
	{
		mem_Free( mem );
	}

	return( error );
}

// --

static S32 myType_u8( struct Config *cfg, STR buf, BPTR h, const struct Cfg_Label *Cmd )
{
U8 *u8;
S32 error;
S32 stat;
S32 len;

	error = TRUE;

	u8 = (PTR) ( (U32) cfg + Cmd->Offset );

	sprintf( buf, " %s = %d\n", Cmd->Name, *u8 );

	len = strlen( buf );

	stat = Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static S32 myType_u32( struct Config *cfg, STR buf, BPTR h, const struct Cfg_Label *Cmd )
{
U32 *u32;
S32 error;
S32 stat;
S32 len;

	error = TRUE;

	u32 = (PTR) ( (U32) cfg + Cmd->Offset );

	sprintf( buf, " %s = %" PRIu32 "\n", Cmd->Name, *u32 );

	len = strlen( buf );

	stat = Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static S32 myType_s32( struct Config *cfg, STR buf, BPTR h, const struct Cfg_Label *Cmd )
{
S32 *s32;
S32 error;
S32 stat;
S32 len;

	error = TRUE;

	s32 = (PTR) ( (U32) cfg + Cmd->Offset );

	sprintf( buf, " %s = %" PRId32 "\n", Cmd->Name, *s32 );

	len = strlen( buf );

	stat = Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static S32 myType_vb( struct Config *cfg UNUSED, STR buf, BPTR h, const struct Cfg_Label *Cmd UNUSED )
{
S32 error;
S32 stat;
S32 len;

	error = TRUE;

//	if ( DoVerbose > 0 )
	{
		sprintf( buf, " Verbose = %" PRId32 "\n", DoVerbose );
	}
//	else
//	{
//		// Writing 0 will over write the -v shell option
//		// so we do not set it by default
//		sprintf( buf, "; Verbose = %d\n", DoVerbose );
//	}

	len = strlen( buf );

	stat = Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static S32 myGroup( struct Config *cfg, BPTR h, STR buf, const STR grpname, const struct Cfg_Label *label )
{
S32 (*func)( struct Config *cfg, STR buf, BPTR h, const struct Cfg_Label *Cmd );
S32 error;
S32 stat;
S32 len;
S32 pos;

	error = TRUE;

	// -- Write Header

	sprintf( buf, "\n%s\n", grpname );

	len = strlen( buf );

	stat = Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	pos = 0;

	while( label[pos].Name )
	{
		switch( label[pos].Type )
		{
//			case CLT_Config_ip:			{ func = myType_ip; break; }
//			case CLT_Config_in_ip4:		{ func = myType_ip4; break; }
//			case CLT_Config_ex_ip4:		{ func = myType_ip4; break; }
			case CLT_Config_vb:			{ func = myType_vb; break; }
			case CLT_Config_u8:			{ func = myType_u8; break; }
			case CLT_Config_u32:		{ func = myType_u32; break; }
			case CLT_Config_s32:		{ func = myType_s32; break; }
			case CLT_Config_gstr:		{ func = myType_gstr; break; }
			case CLT_Config_bstr:		{ func = myType_bstr; break; }
			case CLT_Config_pstr:		{ func = myType_pstr; break; }

			default:
			{
				SHELLBUF_PRINTF4( "%s:%04d: Error unknown type (%d) for '%s'\n", __FILE__, __LINE__, label[pos].Type, label[pos].Name );
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

#endif
// --

S32 Config_Save_IP(  struct Config *cfg, STR buf, BPTR h, const struct CFGNODE *Cmd );
S32 Config_Save_STR( struct Config *cfg, STR buf, BPTR h, const struct CFGNODE *Cmd );
S32 Config_Save_VAL( struct Config *cfg, STR buf, BPTR h, const struct CFGNODE *Cmd );

static S32 myGroup2( struct Config *cfg, BPTR h, STR buf, const STR grpname, const struct CFGNODE *Cmds )
{
S32 error;
S32 stat;
S32 len;
S32 pos;

	error = TRUE;

	// -- Write Header

	sprintf( buf, "\n%s\n", grpname );

	len = strlen( buf );

	stat = Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	pos = 0;

	while( Cmds[pos].Name )
	{
		switch( Cmds[pos].Type )
		{
			case CFGTYPE_VAL:
			{
				if ( ! Config_Save_VAL( cfg, buf, h, & Cmds[pos] ))
				{
					goto bailout;
				}
				break;
			}

			case CFGTYPE_STR:
			{
				if ( ! Config_Save_STR( cfg, buf, h, & Cmds[pos] ))
				{
					goto bailout;
				}
				break;
			}

			default:
			{
				SHELLBUF_PRINTF4( "%s:%04d: Error unknown type (%d) for '%s'\n", __FILE__, __LINE__, Cmds[pos].Type, Cmds[pos].Name );
				goto bailout;
			}
		}

		pos++;
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static S32 myGroups( struct Config *cfg, BPTR h, STR buf )
{
STR str;
S32 err;
S32 pos;

	err = TRUE;
	pos = 0;

	while( TRUE )
	{
		str = (PTR) ConfigGroups2[pos].Name;

		if ( ! str )
		{
			break;
		}

		// Exclude IP group
		if ( strcmp( str, "[IP]" ))
		{
			if ( myGroup2( cfg, h, buf, str, ConfigGroups2[pos].Cmds ))
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

static S32 myList( struct Config *cfg UNUSED, STR buf, STR name, struct VNCList *list, BPTR h )
{
struct IPNode *node;
S32 error;
S32 stat;
S32 len;

	error = TRUE;

	sprintf( buf, "\n[%s]\n", name );

	len = strlen( buf );

	stat = Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	node = List_GetHead( list );

	while( node )
	{
		/**/ if ( node->ipn_Type == IPT_Allow )
		{
			sprintf( buf, " Allow = " );
		}
		else if ( node->ipn_Type == IPT_Block )
		{
			sprintf( buf, " Block = " );
		}
		else // ( node->ipn_Type == IPT_Unknown )
		{
			sprintf( buf, " IP = " );
		}

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

		stat = Write( h, buf, len );

		if ( stat != len )
		{
			goto bailout;
		}

		// --

		node = List_GetNext( node );
	}

	error = FALSE;

bailout:

	return( error );
}

// --

static S32 myDelete( STR filename )
{
S32 retval = FALSE;
S32 loop = 10;
S32 stat;

//	if ( DoVerbose > 1 )
//	{
//		SHELLBUF_PRINTF( "Deleting %s\n", filename );
//	}

	SetIoErr( 0 );

	while( loop-- > 0 )
	{
		stat = DELETEFILE( filename );

		if ( stat )
		{
			retval = TRUE;
			break;
		}
		else
		{
			if ( IoErr() != ERROR_OBJECT_IN_USE )
			{
				break;
			}
			else
			{
				Delay( 2 );
			}
		}
	}

	if ( DoVerbose > 1 )
	{
		if ( retval )
		{
			SHELLBUF_PRINTF1( "Deleted %s\n", filename );
		}
		else
		{
			SHELLBUF_PRINTF2( "Failed to delete %s (%d)\n", filename, (S32) IoErr() );
		}
	}

	return( retval );
}

// --

enum RenStat
{
	RenStat_NotFound,
	RenStat_Failed,
	RenStat_Okay,
};

static enum RenStat myRename( STR oldname, STR newname )
{
enum RenStat retval;
S32 loop = 10;
S32 stat;
S32 err;

	// Set Default stat
	retval = RenStat_Failed;

//	if ( DoVerbose > 1 )
//	{
//		SHELLBUF_PRINTF( "Renaming %s -> %s\n", oldname, newname );
//	}

	SetIoErr( 0 );

	while( loop-- > 0 )
	{
		stat = Rename( oldname, newname );

		if ( stat )
		{
			retval = RenStat_Okay;
			break;
		}
		else
		{
			err = IoErr();

			/**/ if ( err == ERROR_OBJECT_IN_USE )
			{
				Delay( 2 );
				continue;
			}
			else if ( err == ERROR_OBJECT_NOT_FOUND )
			{
				retval = RenStat_NotFound;
				break;
			}
			else // Error
			{
				retval = RenStat_Failed;
				break;
			}
		}
	}

	if ( DoVerbose > 1 )
	{
		/**/ if ( retval == RenStat_Okay )
		{
			SHELLBUF_PRINTF2( "Renamed %s -> %s\n", oldname, newname );
		}
		else if ( retval == RenStat_NotFound )
		{
			SHELLBUF_PRINTF1( "Failed %s not found\n", oldname );
		}
		else
		{
			SHELLBUF_PRINTF3( "Failed to renamed %s -> %s (%d)\n", oldname, newname, (S32) IoErr() );
		}
	}

	return( retval );
}

// --

static S32 myHandle_Okay( struct Config *cfg, STR Filename, STR tmp_newname, STR tmp_oldname )
{
enum RenStat renstat;
S32 error;

	// we just Renamed Filename to tmp_oldname
	// now Rename tmp_newname to Filename and Delete tmp_oldname
	renstat = myRename( tmp_newname, Filename );

	/**/ if ( renstat == RenStat_Okay )
	{
		// All okay
		Log_PrintF( cfg, LOGTYPE_Info, "Config saved to %s", Filename );

		// Lets delete old Config file
		if ( ! myDelete( tmp_oldname ))
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Deleting %s file (%ld)", tmp_oldname, IoErr() );
			// hmm should I set error = TRUE
		}

		error = FALSE;
	}
	else if ( renstat == RenStat_Failed )
	{
		// Failed too rename tmp newname, to org filename
		Log_PrintF( cfg, LOGTYPE_Error, "Renaming %s -> %s (%ld)", tmp_newname, Filename, IoErr() );

		// Try and restore old file
		renstat = myRename( tmp_oldname, Filename );

		if ( renstat != RenStat_Okay )
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Renaming %s -> %s (%ld)", tmp_oldname, Filename, IoErr() );
		}

		// Lets delete new Config file
		if ( ! myDelete( tmp_newname ))
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Deleting %s file (%ld)", tmp_newname, IoErr() );
		}

		error = TRUE;
	}
	else // ( renstat == RenStat_NotFound )
	{
		// Something very wrong with my code
		Log_PrintF( cfg, LOGTYPE_Error, "File not found %s (%ld)", tmp_newname, IoErr() );
		error = TRUE;
	}

	return( error );
}

// --

static S32 myHandle_Failed( struct Config *cfg, STR Filename UNUSED, STR tmp_newname, STR tmp_oldname UNUSED )
{
S32 error;

	// we failed Renaming Filename to tmp_oldname

	// I guess all we can do is delete new Config file
	// And keep orginal config

	if ( ! myDelete( tmp_newname ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Deleting %s file (%ld)", tmp_newname, IoErr() );
	}

	// Always return error
	error = TRUE;

	return( error );
}

// --

static S32 myHandle_NotFound( struct Config *cfg, STR Filename, STR tmp_newname, STR tmp_oldname UNUSED )
{
enum RenStat renstat;
S32 error;

	// There was no orginal Filename

	// now Rename tmp_newname to Filename and Delete tmp_oldname
	renstat = myRename( tmp_newname, Filename );

	/**/ if ( renstat == RenStat_Okay )
	{
		// All okay
		Log_PrintF( cfg, LOGTYPE_Info, "Config saved to %s", Filename );
		error = FALSE;
	}
	else if ( renstat == RenStat_Failed )
	{
		// Failed too rename tmp newname, to org filename
		Log_PrintF( cfg, LOGTYPE_Error, "Renaming %s -> %s (%ld)", tmp_newname, Filename, IoErr() );

		// Lets delete new Config file
		if ( ! myDelete( tmp_newname ))
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Deleting %s file (%ld)", tmp_newname, IoErr() );
		}

		error = TRUE;
	}
	else // ( renstat == RenStat_NotFound )
	{
		// Something very wrong with my code
		Log_PrintF( cfg, LOGTYPE_Error, "File not found %s (%ld)", tmp_newname, IoErr() );
		error = TRUE;
	}

	return( error );
}

// --

S32 Config_Save( struct Config *cfg, STR Filename )
{
STR tmp_oldname;	// Rename old config to this tmp
STR tmp_newname;	// Create new config to this tmp
S32 error;
S32 stat;
S32 len;
STR buf;
S32 h;

	h = 0;

	buf = NULL;

	error = TRUE;

	tmp_oldname = NULL;
	tmp_newname = NULL;

	if (( ! cfg ) || ( cfg->cfg_StructID != ID_CFG ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error invalid Config Pointer" );
		goto bailout;
	}

	if ( ! Filename )
	{
		Filename = "rVNCd.config";
	}

	buf = mem_Alloc( 1024 );
	tmp_oldname = Func_GetTempFilename( Filename );
	tmp_newname = Func_GetTempFilename( Filename );

	if (( ! buf ) || ( ! tmp_oldname ) || ( ! tmp_newname ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocating memory" );
		goto bailout;
	}

	h = Open( tmp_newname, MODE_NEWFILE );

	if ( ! h )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening file '%s'", Filename );
		goto bailout;
	}

	/*
	** File Header
	*/

	len = strlen( ConfigHeaderStr );

	stat = Write( h, ConfigHeaderStr, len );

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

	ObtainSemaphore( & cfg->IPSema );
	stat = myList( cfg, buf, "IP", & cfg->IPList, h );
	ReleaseSemaphore( & cfg->IPSema );

	if ( stat )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error writing to file" );
		goto bailout;
	}

	// --

	if ( ! Func_SameString( cfg->cfg_Config_Filename, Filename ))
	{
		if ( cfg->cfg_Config_Filename )
		{
			mem_Free( cfg->cfg_Config_Filename );
		}

		cfg->cfg_Config_Filename = mem_Strdup( Filename );
	}

	// --

	error = FALSE;

bailout:

	if ( h )
	{
		Close( h );

		// --

		if ( error )
		{
			// failed to create new config
			// lets delete it
			if ( ! myDelete( tmp_newname ))
			{
				// and we also failed to delete the tmp file
				Log_PrintF( cfg, LOGTYPE_Error, "Deleting %s file (%ld)", tmp_newname, IoErr() );
			}
		}
		else
		{
			// Rename old config file, a tmp name
			enum RenStat renstat = myRename( Filename, tmp_oldname );

			/**/ if ( renstat == RenStat_Okay )
			{
				error = myHandle_Okay( cfg, Filename, tmp_newname, tmp_oldname );
			}
			else if ( renstat == RenStat_Failed )
			{
				error = myHandle_Failed( cfg, Filename, tmp_newname, tmp_oldname );
			}
			else // ( renstat == RenStat_NotFound )
			{
				error = myHandle_NotFound( cfg, Filename, tmp_newname, tmp_oldname );
			}
		}
	}

	// --

	if ( tmp_newname )
	{
		mem_Free( tmp_newname );
	}

	if ( tmp_oldname )
	{
		mem_Free( tmp_oldname );
	}

	if ( buf )
	{
		mem_Free( buf );
	}

	return( error );
}

// --
