
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 Config_Save_VAL( struct Config *cfg, STR buf, BPTR h, const struct CFGNODE *Cmd )
{
S32 retval;
S32 stat;
S32 val;
S32 len;

	retval = FALSE;

	switch( Cmd->ID )
	{
		case CFGID_Server_Port:
		{
			val = cfg->cfg_Disk_Settings.Port;
			break;
		}

		case CFGID_Server_SendTileBuffer:
		{
			val = cfg->cfg_Disk_Settings.TileBufferKB;
			break;
		}

		case CFGID_Server_TileSize:
		{
			val = cfg->cfg_Disk_Settings.TileSize;
			break;
		}

		case CFGID_Server_SendWatchDog:
		{
			val = cfg->cfg_Disk_Settings.SendWatchDog;
			break;
		}

		case CFGID_Server_DelayFrequency:
		{
			val = cfg->GfxRead_DelayFrequency;
			break;
		}

		case CFGID_Verbose:
		{
			val = DoVerbose;
			break;
		}

		case CFGID_Program_DisableGUI:
		{
			val = cfg->cfg_Program_DisableGUI;
			break;
		}

		case CFGID_Program_DisableARexx:
		{
			val = cfg->cfg_Program_DisableARexx;
			break;
		}

		case CFGID_Program_DisableCxBroker:
		{
			val = cfg->cfg_Program_DisableCxBroker;
			break;
		}

		// Server
		case CFGID_Server_EncRRE:
		{
			val = cfg->cfg_Disk_Settings.RRE;
			break;
		}

		case CFGID_Server_EncZLIB:
		{
			val = cfg->cfg_Disk_Settings.ZLib;
			break;
		}

		case CFGID_Server_SendBell:
		{
			val = cfg->cfg_Disk_Settings.SendBell;
			break;
		}

		case CFGID_Server_AutoStart:
		{
			val = cfg->AutoStart;
			break;
		}

		case CFGID_Server_Protocol33:
		{
			val = cfg->cfg_Disk_Settings.Protocol33;
			break;
		}

		case CFGID_Server_Protocol37:
		{
			val = cfg->cfg_Disk_Settings.Protocol37;
			break;
		}

		case CFGID_Server_ScanMethod:
		{
			val = cfg->cfg_Disk_Settings.TileScanMethod;
			break;
		}

		case CFGID_Server_PointerType:
		{
			val = cfg->cfg_Disk_Settings.PointerType;
			break;
		}

		case CFGID_Server_SendClipboard:
		{
			val = cfg->cfg_Disk_Settings.SendClipboard;
			break;
		}

		case CFGID_Server_ReadClipboard:
		{
			val = cfg->cfg_Disk_Settings.ReadClipboard;
			break;
		}

		case CFGID_Server_EncRichCursor:
		{
			val = cfg->cfg_Disk_Settings.RichCursor;
			break;
		}

		case CFGID_Server_DisableBlanker:
		{
			val = cfg->cfg_Disk_Settings.DisableBlanker;
			break;
		}

		// Log
		case CFGID_Log_Enable:
		{
			val = cfg->cfg_LogEnable;
			break;
		}

		case CFGID_Log_Infos:
		{
			val = cfg->cfg_ProgramLogInfos;
			break;
		}

		case CFGID_Log_Errors:
		{
			val = cfg->cfg_ProgramLogErrors;
			break;
		}

		case CFGID_Log_Warnings:
		{
			val = cfg->cfg_ProgramLogWarnings;
			break;
		}

		case CFGID_Log_ProgramStart:
		{
			val = cfg->cfg_LogProgramStart;
			break;
		}

		case CFGID_Log_ProgramStop:
		{
			val = cfg->cfg_LogProgramStop;
			break;
		}

		case CFGID_Log_ServerStart:
		{
			val = cfg->cfg_LogServerStart;
			break;
		}

		case CFGID_Log_ServerStop:
		{
			val = cfg->cfg_LogServerStop;
			break;
		}

		case CFGID_Log_UserConnect:
		{
			val = cfg->cfg_LogUserConnect;
			break;
		}

		case CFGID_Log_UserDisconnect:
		{
			val = cfg->cfg_LogUserDisconnect;
			break;
		}

		// Actions
		case CFGID_Action_ProgramStopEnable:
		{
			val = cfg->cfg_ActionsProgramStopEnable;
			break;
		}

		case CFGID_Action_ProgramStartEnable:
		{
			val = cfg->cfg_ActionsProgramStartEnable;
			break;
		}

		case CFGID_Action_ServerStartEnable:
		{
			val = cfg->cfg_ActionsServerStartEnable;
			break;
		}

		case CFGID_Action_ServerStopEnable:
		{
			val = cfg->cfg_ActionsServerStopEnable;
			break;
		}

		case CFGID_Action_UserConnectEnable:
		{
			val = cfg->cfg_ActionsUserConnectEnable;
			break;
		}

		case CFGID_Action_UserDisconnectEnable:
		{
			val = cfg->cfg_ActionsUserDisconnectEnable;
			break;
		}

		default:
		{
			SHELLBUF_PRINTF2( "Unknown : VAL : '%s' (%" PRId32 ")\n", Cmd->Name, Cmd->ID );
			goto bailout;
		}
	}

	sprintf( buf, " %s = %" PRId32 "\n", Cmd->Name, val );

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
