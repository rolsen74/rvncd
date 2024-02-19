 
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

void Config_Reset( struct Config *cfg )
{
APTR node;

	if (( cfg == NULL ) || ( cfg->cfg_ID != ID_CFG ))
	{
		goto bailout;
	}

	while(( node = IExec->RemHead( & cfg->BlackList )))
	{
		myFree( node );
	}

	while(( node = IExec->RemHead( & cfg->WhiteList )))
	{
		myFree( node );
	}

	if ( cfg->cfg_LogFileName )
	{
		myFree( cfg->cfg_LogFileName );
	}

	if ( cfg->cfg_PointerFileName )
	{
		myFree( cfg->cfg_PointerFileName );
		cfg->cfg_PointerFileName = NULL;
	}

	// --

	cfg->cfg_TileSize = 64;	// Default 64x64 tile size

	cfg->cfg_LogInit = 1;
	cfg->cfg_LogEnable = 1;
	cfg->cfg_LogFileName = myStrdup( "Logdata" );
	cfg->cfg_LogFileLimit = 1024 * 1024;
	cfg->cfg_LogProgramStart = 0;
	cfg->cfg_LogProgramStop = 0;
	cfg->cfg_LogServerStart = 1;
	cfg->cfg_LogServerStop = 1;
	cfg->cfg_LogUserConnect = 1;
	cfg->cfg_LogUserDisconnect = 1;

	cfg->cfg_ProgramLogInfos = 0;
	cfg->cfg_ProgramLogErrors = 1;
	cfg->cfg_ProgramLogWarnings = 1;

	cfg->cfg_ActionsProgramStartEnable = 0;
	cfg->cfg_ActionsProgramStopEnable = 0;
	cfg->cfg_ActionsServerStartEnable = 0;
	cfg->cfg_ActionsServerStopEnable = 0;
	cfg->cfg_ActionsUserConnectEnable = 0;
	cfg->cfg_ActionsUserDisconnectEnable = 0;

	cfg->GfxRead_BufferScanMethod = SCANMethod_Linear;
	cfg->GfxRead_DelayFrequency = 150;

	// --
	cfg->cfg_Disk_Settings.Port = 5900;
	cfg->cfg_Disk_Settings.ZLib = TRUE;
	cfg->cfg_Disk_Settings.RichCursor = TRUE;
	cfg->cfg_Disk_Settings.BufferSync = TRUE;
	cfg->cfg_Disk_Settings.SendClipboard = TRUE;
	cfg->cfg_Disk_Settings.ReadClipboard = TRUE;
	cfg->cfg_Disk_Settings.DisableBlanker = TRUE;
	sprintf( cfg->cfg_Disk_Settings.Name, "Amiga VNC Server" );
	cfg->cfg_Disk_Settings.Password[0] = 0;
	cfg->cfg_Disk_Settings2.AutoStart = TRUE;

	// --

	cfg->cfg_WinData[WIN_Main].Status = WINSTAT_Open;
	cfg->cfg_WinData[WIN_Main].OpenWin = myGUI_OpenMainWindow;
	cfg->cfg_WinData[WIN_Main].CloseWin = myGUI_CloseMainWindow;
	cfg->cfg_WinData[WIN_Main].HandleWin = myGUI_HandleMainWindow;

	cfg->cfg_WinData[WIN_About].Status = WINSTAT_Open;
	cfg->cfg_WinData[WIN_About].OpenWin = myGUI_OpenAboutWindow;
	cfg->cfg_WinData[WIN_About].CloseWin = myGUI_CloseAboutWindow;
	cfg->cfg_WinData[WIN_About].HandleWin = myGUI_HandleAboutWindow;

	cfg->cfg_WinData[WIN_Encodings].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_Encodings].OpenWin = myGUI_OpenEncodingsWindow;
	cfg->cfg_WinData[WIN_Encodings].CloseWin = myGUI_CloseEncodingsWindow;
	cfg->cfg_WinData[WIN_Encodings].HandleWin = myGUI_HandleEncodingsWindow;

	cfg->cfg_WinData[WIN_KeyLogger].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_KeyLogger].OpenWin = myGUI_OpenKeyWindow;
	cfg->cfg_WinData[WIN_KeyLogger].CloseWin = myGUI_CloseKeyWindow;
	cfg->cfg_WinData[WIN_KeyLogger].HandleWin = myGUI_HandleKeyWindow;

	cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_Session].OpenWin = myGUI_OpenSessionWindow;
	cfg->cfg_WinData[WIN_Session].CloseWin = myGUI_CloseSessionWindow;
	cfg->cfg_WinData[WIN_Session].HandleWin = myGUI_HandleSessionWindow;

	cfg->cfg_WinData[WIN_PixelFormat].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_PixelFormat].OpenWin = myGUI_OpenPixelWindow;
	cfg->cfg_WinData[WIN_PixelFormat].CloseWin = myGUI_ClosePixelWindow;
	cfg->cfg_WinData[WIN_PixelFormat].HandleWin = myGUI_HandlePixelWindow;

	// --

bailout:

	return;
}

// --

struct Config *Config_Create( void )
{
struct Config *cfg;

	cfg = myCalloc( sizeof( struct Config ));

	if ( cfg == NULL )
	{
		goto bailout;
	}

	cfg->cfg_ID = ID_CFG;

	IExec->InitSemaphore( & cfg->GfxRead_Screen_Sema );
	IExec->InitSemaphore( & cfg->cfg_LogPrintFSema );
	IExec->NewList( & cfg->WhiteList );
	IExec->NewList( & cfg->BlackList );
	IExec->NewList( & cfg->cfg_ServerCfg_Groups );

	Config_Reset( cfg );

bailout:

	return( cfg );
}

// --

void Config_Delete( struct Config *cfg )
{
APTR node;

	if (( cfg == NULL ) || ( cfg->cfg_ID != ID_CFG ))
	{
		goto bailout;
	}

	cfg->cfg_ID = 0;

	while(( node = IExec->RemHead( & cfg->BlackList )))
	{
		myFree( node );
	}

	while(( node = IExec->RemHead( & cfg->WhiteList )))
	{
		myFree( node );
	}

	if ( cfg->cfg_Startup_Config_FileName )
	{
		myFree( cfg->cfg_Startup_Config_FileName );
	}

	if ( cfg->cfg_LogFileName )
	{
		myFree( cfg->cfg_LogFileName );
	}

//	if ( cfg->cfg_ServerName )
//	{
//		myFree( cfg->cfg_ServerName );
//	}

//	if ( cfg->cfg_ServerPassword )
//	{
//		myFree( cfg->cfg_ServerPassword );
//	}

	if ( cfg->cfg_PointerFileName )
	{
		myFree( cfg->cfg_PointerFileName );
	}

	myFree( cfg );

bailout:

	return;
}

// --

int Config_Parse( struct Config *cfg, int argc, char **argv )
{
int error;

	error = TRUE;

	if (( cfg == NULL ) || ( cfg->cfg_ID != ID_CFG ))
	{
		goto bailout;
	}

	// Parse early user arguments
	if ( Config_ParseArgs( cfg, argc, argv, TRUE ))
	{
		goto bailout;
	}

	// Load and Parse Config file
	if ( Config_ParseFile( cfg, cfg->cfg_Startup_Config_FileName ))
	{
		goto bailout;
	}

	// Parse user arguments and overwrite Settings
	if ( Config_ParseArgs( cfg, argc, argv, FALSE ))
	{
		goto bailout;
	}

	error = FALSE;

bailout:

	return( error );
}

// --
