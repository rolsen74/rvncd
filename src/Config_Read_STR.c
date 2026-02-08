
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 Config_Read_STR( struct Config *cfg, const struct CFGNODE *Cmd, const STR grpname, STR mem, S32 line )
{
S32 retval;
S32 dofree;
STR str;

	retval = FALSE;
	dofree = TRUE;

	str = Config_CopyString( mem );

	if ( ! str )
	{
		SHELLBUF_PRINTF3( "Config: %s '%s' error copying string (Line %d)\n", grpname, Cmd->Name, line );
		goto bailout;
	}

	switch( Cmd->ID )
	{
		case CFGID_Log_File:
		{
			if ( cfg->cfg_LogFilename )
			{
				mem_Free( cfg->cfg_LogFilename );
			}

			cfg->cfg_LogFilename = str;
			dofree = FALSE;
			break;
		}

		case CFGID_Server_PointerFile:
		{
			if ( cfg->cfg_PointerFilename )
			{
				mem_Free( cfg->cfg_PointerFilename );
			}

			cfg->cfg_PointerFilename = str;
			dofree = FALSE;
			break;
		}

		case CFGID_Server_Name:
		{
			strncpy( cfg->cfg_Disk_Settings.Name, str, MAX_SERVERNAME );
			cfg->cfg_Disk_Settings.Name[ MAX_SERVERNAME ] = 0;
			break;
		}

		case CFGID_Server_Password:
		{
			strncpy( cfg->cfg_Disk_Settings.Password, str, MAX_SERVERPASSWORD );
			cfg->cfg_Disk_Settings.Password[ MAX_SERVERPASSWORD ] = 0;
			break;
		}

		case CFGID_Action_ProgramStart:
		{
			strncpy( ActionBuffer_ProgramStart, str, MAX_ACTIONBUFFER );
			ActionBuffer_ProgramStart[ MAX_ACTIONBUFFER-1 ] = 0;
			break;
		}

		case CFGID_Action_ProgramStop:
		{
			strncpy( ActionBuffer_ProgramStop, str, MAX_ACTIONBUFFER );
			ActionBuffer_ProgramStop[ MAX_ACTIONBUFFER-1 ] = 0;
			break;
		}

		case CFGID_Action_ServerStart:
		{
			strncpy( ActionBuffer_ServerStart, str, MAX_ACTIONBUFFER );
			ActionBuffer_ServerStart[ MAX_ACTIONBUFFER-1 ] = 0;
			break;
		}

		case CFGID_Action_ServerStop:
		{
			strncpy( ActionBuffer_ServerStop, str, MAX_ACTIONBUFFER );
			ActionBuffer_ServerStop[ MAX_ACTIONBUFFER-1 ] = 0;
			break;
		}

		case CFGID_Action_UserConnect:
		{
			strncpy( ActionBuffer_UserConnect, str, MAX_ACTIONBUFFER );
			ActionBuffer_UserConnect[ MAX_ACTIONBUFFER-1 ] = 0;
			break;
		}

		case CFGID_Action_UserDisconnect:
		{
			strncpy( ActionBuffer_UserDisconnect, str, MAX_ACTIONBUFFER );
			ActionBuffer_UserDisconnect[ MAX_ACTIONBUFFER-1 ] = 0;
			break;
		}

		default:
		{
			SHELLBUF_PRINTF2( "Unknown : STR : '%s' (%" PRId32 ")\n", Cmd->Name, Cmd->ID );
			goto bailout;
		}
	}

	if ( DoVerbose > 0 )
	{
		SHELLBUF_PRINTF3( " %s %s = '%s'\n", grpname, Cmd->Name, str );
	}

	if ( dofree )
	{
		mem_Free( str );
	}

	retval = TRUE;

bailout:

	return( retval );
}

// --
