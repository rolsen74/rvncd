
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

const char *ConfigHeaderStr = "[RVNC SERVER CONFIG]\n";

// --

static const struct Cfg_Label myBlackList[] =
{
{  2, "IP",						CLT_Config_ip,		CLT_B,	0,					0, NULL },
{  0, NULL, 0, 0, 0, 0, NULL },
};

static const struct Cfg_Label myWhiteList[] =
{
{  2, "IP",						CLT_Config_ip,		CLT_W,	0,					0, NULL },
{  0, NULL, 0, 0, 0, 0, NULL },
};

static const struct Cfg_Label myProgram[] =
{
{  7, "Verbose",				CLT_Config_vb,		0,		1,					0, NULL },
// --
{ 10, "DisableGUI",				CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ProgramDisableGUI ), NULL },
{ 12, "DisableARexx",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ProgramDisableARexx ), NULL },
{ 15, "DisableCxBroker",		CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ProgramDisableCxBroker ), NULL },

{ 13, "WinMainStatus",			CLT_Config_u8,		0,		WINSTAT_Last,		offsetof( struct Config, MainWinState ), NULL },
{ 14, "WinAboutStatus",			CLT_Config_s32,		0,		WINSTAT_Last,		offsetof( struct Config, cfg_WinData[WIN_About].Status ), NULL },
{ 12, "WinKeyStatus",			CLT_Config_s32,		0,		WINSTAT_Last,		offsetof( struct Config, cfg_WinData[WIN_KeyLogger].Status ), NULL },
{ 14, "WinPixelStatus",			CLT_Config_s32,		0,		WINSTAT_Last,		offsetof( struct Config, cfg_WinData[WIN_PixelFormat].Status ), NULL },
{ 16, "WinSessionStatus",		CLT_Config_s32,		0,		WINSTAT_Last,		offsetof( struct Config, cfg_WinData[WIN_Session].Status ), NULL },
{ 18, "WinEncodingsStatus",		CLT_Config_s32,		0,		WINSTAT_Last,		offsetof( struct Config, cfg_WinData[WIN_Encodings].Status ), NULL },

{  0, NULL, 0, 0, 0, 0, NULL },
};

static const struct Cfg_Label myActions[] =
{
{ 17, "ProgramStopEnable",		CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ActionsProgramStopEnable ), NULL },
{ 17, "ProgramStopBuffer",		CLT_Config_gstr,	0,		MAX_ACTIONBUFFER,	0, ActionBuffer_ProgramStop },
{ 18, "ProgramStartEnable",		CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ActionsProgramStartEnable ), NULL },
{ 18, "ProgramStartBuffer",		CLT_Config_gstr,	0,		MAX_ACTIONBUFFER,	0, ActionBuffer_ProgramStart },

{ 17, "ServerStartEnable",		CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ActionsServerStartEnable ), NULL },
{ 17, "ServerStartBuffer",		CLT_Config_gstr,	0,		MAX_ACTIONBUFFER,	0, ActionBuffer_ServerStart },
{ 16, "ServerStopEnable",		CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ActionsServerStopEnable ), NULL },
{ 16, "ServerStopBuffer",		CLT_Config_gstr,	0,		MAX_ACTIONBUFFER,	0, ActionBuffer_ServerStop },

{ 17, "UserConnectEnable",		CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ActionsUserConnectEnable ), NULL },
{ 17, "UserConnectBuffer",		CLT_Config_gstr,	0,		MAX_ACTIONBUFFER,	0, ActionBuffer_UserConnect },
{ 20, "UserDisconnectEnable",	CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ActionsUserDisconnectEnable ), NULL },
{ 20, "UserDisconnectBuffer",	CLT_Config_gstr,	0,		MAX_ACTIONBUFFER,	0, ActionBuffer_UserDisconnect },

{  0, NULL, 0, 0, 0, 0, NULL },
};

static const struct Cfg_Label myServer[] =
{
{  4, "Port",					CLT_Config_u32,		0,		65535,				offsetof( struct Config, cfg_Disk_Settings.Port ), NULL },
{  4, "Name",					CLT_Config_bstr,	0,		MAX_SERVERNAME,		offsetof( struct Config, cfg_Disk_Settings.Name ), NULL },
{  7, "EncZLIB",				CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_Disk_Settings.ZLib ), NULL },
{  8, "Password",				CLT_Config_bstr,	0,		MAX_SERVERPASSWORD,	offsetof( struct Config, cfg_Disk_Settings.Password ), NULL },
{  8, "TileSize",				CLT_Config_s32,		16,		256,				offsetof( struct Config, cfg_Disk_Settings.TileSize ), NULL },
{  9, "AutoStart",				CLT_Config_u8,		0,		1,					offsetof( struct Config, AutoStart ), NULL },
{ 10, "Protocol33",				CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_Disk_Settings.Protocol33 ), NULL },
{ 10, "Protocol37",				CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_Disk_Settings.Protocol37 ), NULL },
{ 10, "ScanMethod",				CLT_Config_u8,		0,		SCANMethod_Last,	offsetof( struct Config, cfg_Disk_Settings.TileScanMethod ), NULL },
{ 11, "PointerFile",			CLT_Config_pstr,	0,		0,					offsetof( struct Config, cfg_PointerFilename ), NULL },
{ 11, "PointerType",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_Disk_Settings.PointerType ), NULL },
{ 12, "SendWatchDog",			CLT_Config_u32,		0,		1000,				offsetof( struct Config, cfg_Disk_Settings.SendWatchDog ), NULL },
{ 13, "SendClipboard",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_Disk_Settings.SendClipboard ), NULL },
{ 13, "ReadClipboard",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_Disk_Settings.ReadClipboard ), NULL },
{ 13, "EncRichCursor",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_Disk_Settings.RichCursor ), NULL },
{ 14, "DisableBlanker",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_Disk_Settings.DisableBlanker ), NULL },
{ 14, "DelayFrequency",			CLT_Config_s32,		0,		500,				offsetof( struct Config, GfxRead_DelayFrequency ), NULL },
{  0, NULL, 0, 0, 0, 0, NULL },
};

static const struct Cfg_Label myLog[] =
{
{  6, "Enable",					CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_LogEnable ), NULL },
{  7, "LogFile",				CLT_Config_pstr,	0,		0,					offsetof( struct Config, cfg_LogFilename ), NULL },
// --
{  5, "Infos",					CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ProgramLogInfos ), NULL },
{  6, "Errors",					CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ProgramLogWarnings ), NULL },
{  8, "Warnings",				CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ProgramLogErrors ), NULL },

{ 12, "ProgramStart",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_LogProgramStart ), NULL },
{ 11, "ProgramStop",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_LogProgramStop ), NULL },

{ 11, "ServerStart",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_LogServerStart ), NULL },
{ 10, "ServerStop",				CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_LogServerStop ), NULL },

{ 11, "UserConnect",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_LogUserConnect ), NULL },
{ 14, "UserDisconnect",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_LogUserDisconnect ), NULL },

{  0, NULL, 0, 0, 0, 0, NULL },
};

const struct Cfg_Group ConfigGroups[] =
{
{  9, "[PROGRAM]",		myProgram },	// Should be first.. to handle Verbose
// --
{  5, "[LOG]",			myLog },
{  8, "[SERVER]",		myServer },
{  9, "[ACTIONS]",		myActions },
{ 11, "[BLACKLIST]",	myBlackList },
{ 11, "[WHITELIST]",	myWhiteList },
{  0, NULL,				NULL }
};

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

	if ( cfg->cfg_LogFilename )
	{
		myFree( cfg->cfg_LogFilename );
		cfg->cfg_LogFilename = NULL;
	}

	if ( cfg->cfg_PointerFilename )
	{
		myFree( cfg->cfg_PointerFilename );
		cfg->cfg_PointerFilename = NULL;
	}

	// cfg_PointerFilename2 may be in use

	// --

	cfg->cfg_LogInit = 1;
	cfg->cfg_LogEnable = 1;
	cfg->cfg_LogFilename = myStrdup( "Logdata" );
	cfg->cfg_LogFileLimit = 1024 * 1024;
	cfg->cfg_LogProgramStart = 0;
	cfg->cfg_LogProgramStop = 0;
	cfg->cfg_LogServerStart = 1;
	cfg->cfg_LogServerStop = 1;
	cfg->cfg_LogUserConnect = 1;
	cfg->cfg_LogUserDisconnect = 1;

	cfg->cfg_ProgramLogInfos = 1;
	cfg->cfg_ProgramLogErrors = 1;
	cfg->cfg_ProgramLogWarnings = 1;

	cfg->cfg_ActionsProgramStartEnable = 0;
	cfg->cfg_ActionsProgramStopEnable = 0;
	cfg->cfg_ActionsServerStartEnable = 0;
	cfg->cfg_ActionsServerStopEnable = 0;
	cfg->cfg_ActionsUserConnectEnable = 0;
	cfg->cfg_ActionsUserDisconnectEnable = 0;

	cfg->GfxRead_DelayFrequency = 150;

	// --
	cfg->AutoStart = FALSE;
	cfg->MainWinState = WINSTAT_Open;
	cfg->cfg_Disk_Settings.Port = 5900;
	cfg->cfg_Disk_Settings.ZLib = TRUE;
	cfg->cfg_Disk_Settings.TileSize = 64;	// Default 64x64px tile size
	cfg->cfg_Disk_Settings.TileScanMethod = SCANMethod_Linear;
	cfg->cfg_Disk_Settings.RichCursor = TRUE;
	cfg->cfg_Disk_Settings.SendClipboard = TRUE;
	cfg->cfg_Disk_Settings.ReadClipboard = TRUE;
	cfg->cfg_Disk_Settings.DisableBlanker = TRUE;
	sprintf( cfg->cfg_Disk_Settings.Name, "Amiga VNC Server" );
	cfg->cfg_Disk_Settings.Password[0] = 0;
	cfg->cfg_Disk_Settings.ScreenViewMode = VIEWMODE_Page;
	cfg->cfg_Disk_Settings.Protocol33 = TRUE;
	cfg->cfg_Disk_Settings.Protocol37 = TRUE;

	// --

	cfg->cfg_WinData[WIN_Main].Status = WINSTAT_Open;
	cfg->cfg_WinData[WIN_Main].BusyWin = myGUI_BusyMainWindow;
	cfg->cfg_WinData[WIN_Main].OpenWin = myGUI_OpenMainWindow;
	cfg->cfg_WinData[WIN_Main].CloseWin = myGUI_CloseMainWindow;
	cfg->cfg_WinData[WIN_Main].HandleWin = myGUI_HandleMainWindow;

	cfg->cfg_WinData[WIN_About].Status = WINSTAT_Open;
	cfg->cfg_WinData[WIN_About].BusyWin = myGUI_BusyAboutWindow;
	cfg->cfg_WinData[WIN_About].OpenWin = myGUI_OpenAboutWindow;
	cfg->cfg_WinData[WIN_About].CloseWin = myGUI_CloseAboutWindow;
	cfg->cfg_WinData[WIN_About].HandleWin = myGUI_HandleAboutWindow;

	cfg->cfg_WinData[WIN_Encodings].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_Encodings].BusyWin = myGUI_BusyEncodingsWindow;
	cfg->cfg_WinData[WIN_Encodings].OpenWin = myGUI_OpenEncodingsWindow;
	cfg->cfg_WinData[WIN_Encodings].CloseWin = myGUI_CloseEncodingsWindow;
	cfg->cfg_WinData[WIN_Encodings].HandleWin = myGUI_HandleEncodingsWindow;

	cfg->cfg_WinData[WIN_KeyLogger].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_KeyLogger].BusyWin = myGUI_BusyKeyWindow;
	cfg->cfg_WinData[WIN_KeyLogger].OpenWin = myGUI_OpenKeyWindow;
	cfg->cfg_WinData[WIN_KeyLogger].CloseWin = myGUI_CloseKeyWindow;
	cfg->cfg_WinData[WIN_KeyLogger].HandleWin = myGUI_HandleKeyWindow;

	cfg->cfg_WinData[WIN_Session].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_Session].BusyWin = myGUI_BusySessionWindow;
	cfg->cfg_WinData[WIN_Session].OpenWin = myGUI_OpenSessionWindow;
	cfg->cfg_WinData[WIN_Session].CloseWin = myGUI_CloseSessionWindow;
	cfg->cfg_WinData[WIN_Session].HandleWin = myGUI_HandleSessionWindow;

	cfg->cfg_WinData[WIN_PixelFormat].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_PixelFormat].BusyWin = myGUI_BusyPixelWindow;
	cfg->cfg_WinData[WIN_PixelFormat].OpenWin = myGUI_OpenPixelWindow;
	cfg->cfg_WinData[WIN_PixelFormat].CloseWin = myGUI_ClosePixelWindow;
	cfg->cfg_WinData[WIN_PixelFormat].HandleWin = myGUI_HandlePixelWindow;

	cfg->cfg_WinData[WIN_IPEdit].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_IPEdit].BusyWin = myGUI_BusyIPEditWindow;
	cfg->cfg_WinData[WIN_IPEdit].OpenWin = myGUI_OpenIPEditWindow;
	cfg->cfg_WinData[WIN_IPEdit].CloseWin = myGUI_CloseIPEditWindow;
	cfg->cfg_WinData[WIN_IPEdit].HandleWin = myGUI_HandleIPEditWindow;

	cfg->cfg_WinData[WIN_Quit].Status = WINSTAT_Close;
	cfg->cfg_WinData[WIN_Quit].BusyWin = myGUI_BusyQuitWindow;
	cfg->cfg_WinData[WIN_Quit].OpenWin = myGUI_OpenQuitWindow;
	cfg->cfg_WinData[WIN_Quit].CloseWin = myGUI_CloseQuitWindow;
	cfg->cfg_WinData[WIN_Quit].HandleWin = myGUI_HandleQuitWindow;

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
	IExec->InitSemaphore( & cfg->Server_UpdateSema );
	IExec->NewList( & cfg->Server_UpdateList );
	IExec->NewList( & cfg->Server_UpdateFree );
	IExec->NewList( & cfg->WhiteList );
	IExec->NewList( & cfg->BlackList );

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

	if ( cfg->cfg_Startup_Config_Filename )
	{
		myFree( cfg->cfg_Startup_Config_Filename );
	}

	if ( cfg->cfg_LogFilename )
	{
		myFree( cfg->cfg_LogFilename );
	}

	if ( cfg->cfg_PointerFilename )
	{
		myFree( cfg->cfg_PointerFilename );
	}

	if ( cfg->cfg_PointerFilename2 )
	{
		myFree( cfg->cfg_PointerFilename2 );
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
	if ( Config_Read( cfg, cfg->cfg_Startup_Config_Filename, FALSE ))
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

char *Config_CopyString( char *buffer )
{
char *str;
int mark;
int pos;
int len;

	str = NULL;

	pos = 0;

	while(( buffer[pos] == 9 ) || ( buffer[pos] == 32 ))
	{
		pos++;
	}

	if (( buffer[pos] == '"' ) || ( buffer[pos] == '\'' ))
	{
		mark = buffer[ pos++ ];

		len = pos;

		while(	( buffer[ len ] != 0 )
		&&		( buffer[ len ] != 10 )
		&&		( buffer[ len ] != mark ))
		{
			len += 1;
		}

		if ( buffer[ len ] != '"' )
		{
			printf( "Missing string terminator\n" );
			goto bailout;
		}

		len -= pos;
	}
	else
	{
		len = pos;

		while(	( buffer[ len ] != 0 )
		&&		( buffer[ len ] != 9 ) 
		&&		( buffer[ len ] != 10 )
		&&		( buffer[ len ] != 32 ))
		{
			len += 1;
		}

		len -= pos;
	}

	#if 0

	// Not sure if creating empty strings should be allowed or not

	if ( len == 0 )
	{
		IExec->DebugPrintF( "Zero length string not allowed\n" );
		goto bailout;
	}

	#endif

	str = myCalloc( len + 1 );

	if ( str == NULL )
	{
		printf( "Error allocating %d bytes\n", len + 1 );
		goto bailout;
	}

	if ( len )
	{
		memcpy( str, & buffer[ pos ], len );
	}

bailout:

	return( str );
}

// --
