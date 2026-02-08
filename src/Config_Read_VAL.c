
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 Config_Read_VAL( struct Config *cfg, const struct CFGNODE *Cmd, const STR grpname, STR mem, S32 line )
{
S32 retval;
S32 val;

	retval = FALSE;

	val = atoi( mem );

	if (( val < Cmd->MinVal ) || ( val > Cmd->MaxVal ))
	{
		SHELLBUF_PRINTF4( "Config: %s '%s' Value %" PRId32 " out of range (Line %" PRId32 ")\n", grpname, Cmd->Name, val, line );
		goto bailout;
	}

	switch( Cmd->ID )
	{
		case CFGID_Server_Port:
		{
			cfg->cfg_Disk_Settings.Port = val;
			break;
		}

		case CFGID_Server_SendTileBuffer:
		{
			cfg->cfg_Disk_Settings.TileBufferKB = val;
			break;
		}

		case CFGID_Server_TileSize:
		{
			cfg->cfg_Disk_Settings.TileSize = val;
			break;
		}

		case CFGID_Server_SendWatchDog:
		{
			cfg->cfg_Disk_Settings.SendWatchDog = val;
			break;
		}

		case CFGID_Server_DelayFrequency:
		{
			cfg->GfxRead_DelayFrequency = val;
			break;
		}

		case CFGID_Verbose:
		{
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
			break;
		}

		case CFGID_Program_DisableGUI:
		{
			cfg->cfg_Program_DisableGUI = val;
			break;
		}

		case CFGID_Program_DisableARexx:
		{
			cfg->cfg_Program_DisableARexx = val;
			break;
		}

		case CFGID_Program_DisableCxBroker:
		{
			cfg->cfg_Program_DisableCxBroker = val;
			break;
		}

		// Server
		case CFGID_Server_EncRRE:
		{
			cfg->cfg_Disk_Settings.RRE = val;
			break;
		}

		case CFGID_Server_EncZLIB:
		{
			cfg->cfg_Disk_Settings.ZLib = val;
			break;
		}

		case CFGID_Server_SendBell:
		{
			cfg->cfg_Disk_Settings.SendBell = val;
			break;
		}

		case CFGID_Server_AutoStart:
		{
			cfg->AutoStart = val;
			break;
		}

		case CFGID_Server_Protocol33:
		{
			cfg->cfg_Disk_Settings.Protocol33 = val;
			break;
		}

		case CFGID_Server_Protocol37:
		{
			cfg->cfg_Disk_Settings.Protocol37 = val;
			break;
		}

		case CFGID_Server_ScanMethod:
		{
			cfg->cfg_Disk_Settings.TileScanMethod = val;
			break;
		}

		case CFGID_Server_PointerType:
		{
			cfg->cfg_Disk_Settings.PointerType = val;
			break;
		}

		case CFGID_Server_SendClipboard:
		{
			cfg->cfg_Disk_Settings.SendClipboard = val;
			break;
		}

		case CFGID_Server_ReadClipboard:
		{
			cfg->cfg_Disk_Settings.ReadClipboard = val;
			break;
		}

		case CFGID_Server_EncRichCursor:
		{
			cfg->cfg_Disk_Settings.RichCursor = val;
			break;
		}

		case CFGID_Server_DisableBlanker:
		{
			cfg->cfg_Disk_Settings.DisableBlanker = val;
			break;
		}

		// Log
		case CFGID_Log_Enable:
		{
			cfg->cfg_LogEnable = val;
			break;
		}

		case CFGID_Log_Infos:
		{
			cfg->cfg_ProgramLogInfos = val;
			break;
		}

		case CFGID_Log_Errors:
		{
			cfg->cfg_ProgramLogErrors = val;
			break;
		}

		case CFGID_Log_Warnings:
		{
			cfg->cfg_ProgramLogWarnings = val;
			break;
		}

		case CFGID_Log_ProgramStart:
		{
			cfg->cfg_LogProgramStart = val;
			break;
		}

		case CFGID_Log_ProgramStop:
		{
			cfg->cfg_LogProgramStop = val;
			break;
		}

		case CFGID_Log_ServerStart:
		{
			cfg->cfg_LogServerStart = val;
			break;
		}

		case CFGID_Log_ServerStop:
		{
			cfg->cfg_LogServerStop = val;
			break;
		}

		case CFGID_Log_UserConnect:
		{
			cfg->cfg_LogUserConnect = val;
			break;
		}

		case CFGID_Log_UserDisconnect:
		{
			cfg->cfg_LogUserDisconnect = val;
			break;
		}

		// Actions
		case CFGID_Action_ProgramStopEnable:
		{
			cfg->cfg_ActionsProgramStopEnable = val;
			break;
		}

		case CFGID_Action_ProgramStartEnable:
		{
			cfg->cfg_ActionsProgramStartEnable = val;
			break;
		}

		case CFGID_Action_ServerStartEnable:
		{
			cfg->cfg_ActionsServerStartEnable = val;
			break;
		}

		case CFGID_Action_ServerStopEnable:
		{
			cfg->cfg_ActionsServerStopEnable = val;
			break;
		}

		case CFGID_Action_UserConnectEnable:
		{
			cfg->cfg_ActionsUserConnectEnable = val;
			break;
		}

		case CFGID_Action_UserDisconnectEnable:
		{
			cfg->cfg_ActionsUserDisconnectEnable = val;
			break;
		}

		default:
		{
			SHELLBUF_PRINTF2( "Unknown : VAL : '%s' (%" PRId32 ")\n", Cmd->Name, Cmd->ID );
			goto bailout;
		}
	}

	if (( Cmd->ID != CFGID_Verbose ) && ( DoVerbose > 0 ))
	{
		SHELLBUF_PRINTF3( " %s %s = %" PRId32 "\n", grpname, Cmd->Name, val );
	}

	retval = TRUE;

bailout:

	return( retval );
}
					
// --
