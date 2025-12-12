
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

const STR ConfigHeaderStr = "[RVNC SERVER CONFIG]\n";

// --

#ifdef __RVNCD_GUI_H__

#endif // __RVNCD_GUI_H__

static const struct Cfg_Label myIPList[] =
{
{  5, "Allow",	CLT_Config_in_ip4,	0,	0,	0, NULL },
{  5, "Block",	CLT_Config_ex_ip4,	0,	0,	0, NULL },
{  0, NULL, 0, 0, 0, 0, NULL },
};

// yes, vbcc's offsetof macro do not like ..  offsetof( struct Config, cfg_Disk_Settings.Port )
#define DoSettingOffset(member)		( offsetof( struct Config, cfg_Disk_Settings ) + offsetof( struct Settings, member ))

// yes, vbcc's offsetof macro do not like ..  offsetof( struct Config, cfg_Disk_Settings.Port )
#define DoSettingOffset2(xx,yy)		( offsetof( struct Config, cfg_WinData ) + ( sizeof( struct WinNode ) * xx ) + offsetof( struct WinNode, yy ))

static const struct Cfg_Label myProgram[] =
{
{  7, "Verbose",				CLT_Config_vb,		0,		3,					0, NULL },
// --
{ 10, "DisableGUI",				CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ProgramDisableGUI ), NULL },
{ 12, "DisableARexx",			CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ProgramDisableARexx ), NULL },
{ 15, "DisableCxBroker",		CLT_Config_u8,		0,		1,					offsetof( struct Config, cfg_ProgramDisableCxBroker ), NULL },

#ifdef __RVNCD_GUI_H__
{ 13, "WinMainStatus",			CLT_Config_u8,		0,		WINSTAT_LAST,		offsetof( struct Config, MainWinState ), NULL },
{ 14, "WinAboutStatus",			CLT_Config_s32,		0,		WINSTAT_LAST,		DoSettingOffset2( WIN_About, Status ), NULL },
{ 12, "WinKeyStatus",			CLT_Config_s32,		0,		WINSTAT_LAST,		DoSettingOffset2( WIN_KeyLogger, Status ), NULL },
{ 14, "WinPixelStatus",			CLT_Config_s32,		0,		WINSTAT_LAST,		DoSettingOffset2( WIN_PixelFormat, Status ), NULL },
{ 16, "WinSessionStatus",		CLT_Config_s32,		0,		WINSTAT_LAST,		DoSettingOffset2( WIN_Session, Status ), NULL },
{ 18, "WinEncodingsStatus",		CLT_Config_s32,		0,		WINSTAT_LAST,		DoSettingOffset2( WIN_Encodings, Status ), NULL },
//{ 14, "WinAboutStatus",		CLT_Config_s32,		0,		WINSTAT_LAST,		offsetof( struct Config, cfg_WinData[WIN_About].Status ), NULL },
//{ 12, "WinKeyStatus",			CLT_Config_s32,		0,		WINSTAT_LAST,		offsetof( struct Config, cfg_WinData[WIN_KeyLogger].Status ), NULL },
//{ 14, "WinPixelStatus",		CLT_Config_s32,		0,		WINSTAT_LAST,		offsetof( struct Config, cfg_WinData[WIN_PixelFormat].Status ), NULL },
//{ 16, "WinSessionStatus",		CLT_Config_s32,		0,		WINSTAT_LAST,		offsetof( struct Config, cfg_WinData[WIN_Session].Status ), NULL },
//{ 18, "WinEncodingsStatus",	CLT_Config_s32,		0,		WINSTAT_LAST,		offsetof( struct Config, cfg_WinData[WIN_Encodings].Status ), NULL },
#endif // __RVNCD_GUI_H__

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
{  4, "Port",					CLT_Config_u32,		0,		65535,				DoSettingOffset( Port ), NULL },
{  4, "Name",					CLT_Config_bstr,	0,		MAX_SERVERNAME,		DoSettingOffset( Name ), NULL },
{  6, "EncRRE",					CLT_Config_u8,		0,		1,					DoSettingOffset( RRE ), NULL },
{  7, "EncZLIB",				CLT_Config_u8,		0,		1,					DoSettingOffset( ZLib ), NULL },
{  8, "SendBell",				CLT_Config_u8,		0,		1,					DoSettingOffset( SendBell ), NULL },
{  8, "Password",				CLT_Config_bstr,	0,		MAX_SERVERPASSWORD,	DoSettingOffset( Password ), NULL },
{  8, "TileSize",				CLT_Config_s32,		16,		256,				DoSettingOffset( TileSize ), NULL },
{  9, "AutoStart",				CLT_Config_u8,		0,		1,					offsetof( struct Config, AutoStart ), NULL },
{ 10, "Protocol33",				CLT_Config_u8,		0,		1,					DoSettingOffset( Protocol33 ), NULL },
{ 10, "Protocol37",				CLT_Config_u8,		0,		1,					DoSettingOffset( Protocol37 ), NULL },
{ 10, "ScanMethod",				CLT_Config_u8,		0,		SCANMethod_Last,	DoSettingOffset( TileScanMethod ), NULL },
{ 11, "PointerFile",			CLT_Config_pstr,	0,		0,					offsetof( struct Config, cfg_PointerFilename ), NULL },
{ 11, "PointerType",			CLT_Config_u8,		0,		1,					DoSettingOffset( PointerType ), NULL },
{ 12, "SendWatchDog",			CLT_Config_u32,		0,		1000,				DoSettingOffset( SendWatchDog ), NULL },
{ 13, "SendClipboard",			CLT_Config_u8,		0,		1,					DoSettingOffset( SendClipboard ), NULL },
{ 13, "ReadClipboard",			CLT_Config_u8,		0,		1,					DoSettingOffset( ReadClipboard ), NULL },
{ 13, "EncRichCursor",			CLT_Config_u8,		0,		1,					DoSettingOffset( RichCursor ), NULL },
{ 14, "SendTileBuffer",			CLT_Config_u32,		0,		1024,				DoSettingOffset( TileBufferKB ), NULL },
{ 14, "DisableBlanker",			CLT_Config_u8,		0,		1,					DoSettingOffset( DisableBlanker ), NULL },
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
{  4, "[IP]",			myIPList },
{  5, "[LOG]",			myLog },
{  8, "[SERVER]",		myServer },
{  9, "[ACTIONS]",		myActions },
{  0, NULL,				NULL }
};

// --

void Config_Reset( struct Config *cfg )
{
PTR node;

	if (( ! cfg ) || ( cfg->cfg_StructID != ID_CFG ))
	{
		goto bailout;
	}

	ObtainSemaphore( & cfg->IPSema );
	while(( node = List_RemHead( & cfg->IPList )))
	{
		mem_Free( node );
	}
	ReleaseSemaphore( & cfg->IPSema );

	if ( cfg->cfg_LogFilename )
	{
		mem_Free( cfg->cfg_LogFilename );
		cfg->cfg_LogFilename = NULL;
	}

	if ( cfg->cfg_PointerFilename )
	{
		mem_Free( cfg->cfg_PointerFilename );
		cfg->cfg_PointerFilename = NULL;
	}

	// cfg_PointerFilename2 may be in use

	// --

	cfg->cfg_LogInit = 1;
	cfg->cfg_LogEnable = 1;
	cfg->cfg_LogFilename = mem_Strdup( "Logdata" );
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
	cfg->cfg_Disk_Settings.Port = 5900;
	cfg->cfg_Disk_Settings.RRE = TRUE;
	cfg->cfg_Disk_Settings.ZLib = TRUE;
	cfg->cfg_Disk_Settings.TileSize = 64;	// Default 64x64px tile size
	cfg->cfg_Disk_Settings.TileScanMethod = SCANMethod_Linear;
	cfg->cfg_Disk_Settings.TileBufferKB = 64;
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

	if ( ActiveGUI.Set_Default_Config )
	{
		ActiveGUI.Set_Default_Config( cfg );
	}

	// --

bailout:

	return;
}

// --

struct Config *Config_Init( void )
{
struct Config *cfg;

	cfg = mem_AllocClr( sizeof( struct Config ));

	if ( ! cfg )
	{
		goto bailout;
	}

	cfg->cfg_StructID = ID_CFG;

	InitSemaphore( & cfg->GfxRead_Screen_Sema );
	InitSemaphore( & cfg->cfg_LogPrintFSema );
	InitSemaphore( & cfg->Server_UpdateSema );
	InitSemaphore( & cfg->IPSema );

	List_Init( & cfg->Server_UpdateList );
	List_Init( & cfg->Server_UpdateFree );
	List_Init( & cfg->IPList );

	Config_Reset( cfg );

bailout:

	return( cfg );
}

// --

void Config_Free( struct Config *cfg )
{
PTR node;

	if (( ! cfg ) || ( cfg->cfg_StructID != ID_CFG ))
	{
		goto bailout;
	}

	cfg->cfg_StructID = 0;

	ObtainSemaphore( & cfg->IPSema );
	while(( node = List_RemHead( & cfg->IPList )))
	{
		mem_Free( node );
	}
	ReleaseSemaphore( & cfg->IPSema );

	if ( cfg->cfg_Startup_Config_Filename )
	{
		mem_Free( cfg->cfg_Startup_Config_Filename );
	}

	if ( cfg->cfg_LogFilename )
	{
		mem_Free( cfg->cfg_LogFilename );
	}

	if ( cfg->cfg_PointerFilename )
	{
		mem_Free( cfg->cfg_PointerFilename );
	}

	if ( cfg->cfg_PointerFilename2 )
	{
		mem_Free( cfg->cfg_PointerFilename2 );
	}

	mem_Free( cfg );

bailout:

	return;
}

// --

S32 Config_Parse( struct Config *cfg, S32 argc, STR *argv )
{
S32 error;

	error = TRUE;

	if (( ! cfg ) || ( cfg->cfg_StructID != ID_CFG ))
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

STR Config_CopyString( STR buffer )
{
STR str;
S32 mark;
S32 pos;
S32 len;

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

		if ( buffer[ len ] != mark )
		{
			SHELLBUF_PRINTF( "Missing string terminator\n" );
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
		DebugPrintF( "Zero length string not allowed\n" );
		goto bailout;
	}

	#endif

	str = mem_AllocClr( len + 1 );

	if ( str == NULL )
	{
		SHELLBUF_PRINTF( "Error allocating %d bytes\n", len + 1 );
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
