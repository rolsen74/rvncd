
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 Config_Save_STR( struct Config *cfg, STR buf, BPTR h, const struct CFGNODE *Cmd )
{
S32 retval;
S32 stat;
S32 len;
STR str;

	retval = FALSE;

	switch( Cmd->ID )
	{
		case CFGID_Log_File:
		{
			str = cfg->cfg_LogFilename;
			break;
		}

		case CFGID_Server_PointerFile:
		{
			str = cfg->cfg_PointerFilename;
			break;
		}

		case CFGID_Server_Name:
		{
			str = cfg->cfg_Disk_Settings.Name;
			break;
		}

		case CFGID_Server_Password:
		{
			str = cfg->cfg_Disk_Settings.Password;
			break;
		}

		case CFGID_Action_ProgramStart:
		{
			str = ActionBuffer_ProgramStart;
			break;
		}

		case CFGID_Action_ProgramStop:
		{
			str = ActionBuffer_ProgramStop;
			break;
		}

		case CFGID_Action_ServerStart:
		{
			str = ActionBuffer_ServerStart;
			break;
		}

		case CFGID_Action_ServerStop:
		{
			str = ActionBuffer_ServerStop;
			break;
		}

		case CFGID_Action_UserConnect:
		{
			str = ActionBuffer_UserConnect;
			break;
		}

		case CFGID_Action_UserDisconnect:
		{
			str = ActionBuffer_UserDisconnect;
			break;
		}

		default:
		{
			SHELLBUF_PRINTF2( "Unknown : STR : '%s' (%" PRId32 ")\n", Cmd->Name, Cmd->ID );
			goto bailout;
		}
	}

	sprintf( buf, " %s = \"%s\"\n", Cmd->Name, str );

	len = strlen( buf );

	stat = Write( h, buf, len );

	if ( stat != len )
	{
		goto bailout;
	}

	retval = TRUE;

bailout:

	return( retval );
}
					
// --
