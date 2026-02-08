
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

const STR ConfigHeaderStr = "[RVNC SERVER CONFIG]\n";

// --

static const struct CFGNODE myIP2[] =
{
	CFG_IP(  "Allow",				CFGID_IP_Allow ),
	CFG_IP(  "Block",				CFGID_IP_Block ),
	CFG_END(),
};

static const struct CFGNODE myProgram2[] =
{
	CFG_VAL( "Verbose",				CFGID_Verbose, 0, 3 ),
	// --
	CFG_VAL( "DisableGUI",			CFGID_Program_DisableGUI, 0, 1 ),
	CFG_VAL( "DisableARexx",		CFGID_Program_DisableARexx, 0, 1 ),
	CFG_VAL( "DisableCxBroker",		CFGID_Program_DisableCxBroker, 0, 1 ),
	CFG_END(),
};

static const struct CFGNODE myServer2[] =
{
	CFG_VAL( "AutoStart",			CFGID_Server_AutoStart, 0, 1 ),
	CFG_VAL( "Port",				CFGID_Server_Port, 0, 65535 ),
	CFG_STR( "Name",				CFGID_Server_Name, MAX_SERVERNAME ),
	CFG_STR( "Password",			CFGID_Server_Password, MAX_SERVERPASSWORD ),

	CFG_VAL( "EncRRE",				CFGID_Server_EncRRE, 0, 1 ),
	CFG_VAL( "EncZLIB",				CFGID_Server_EncZLIB, 0, 1 ),
	CFG_VAL( "EncRichCursor",		CFGID_Server_EncRichCursor, 0, 1 ),

	CFG_VAL( "Protocol33",			CFGID_Server_Protocol33, 0, 1 ),
	CFG_VAL( "Protocol37",			CFGID_Server_Protocol37, 0, 1 ),

	CFG_VAL( "SendBell",			CFGID_Server_SendBell, 0, 1 ),
	CFG_VAL( "SendWatchDog",		CFGID_Server_SendWatchDog, 0, 1000 ),
	CFG_VAL( "SendTileBuffer",		CFGID_Server_SendTileBuffer, 0, 1024 ),

	CFG_VAL( "DisableBlanker",		CFGID_Server_DisableBlanker, 0, 1 ),

	CFG_VAL( "SendClipboard",		CFGID_Server_SendClipboard, 0, 1 ),
	CFG_VAL( "ReadClipboard",		CFGID_Server_ReadClipboard, 0, 1 ),

	CFG_VAL( "PointerType",			CFGID_Server_PointerType, 0, 1 ),
	CFG_STR( "PointerFile",			CFGID_Server_PointerFile, 0 ),

	CFG_VAL( "TileSize",			CFGID_Server_TileSize, 16, 256 ),
	CFG_VAL( "ScanMethod",			CFGID_Server_ScanMethod, 0, SCANMethod_Last ),
	CFG_VAL( "DelayFrequency",		CFGID_Server_DelayFrequency, 0, 500 ),

	CFG_END(),
};

static const struct CFGNODE myLog2[] =
{
	CFG_VAL( "Enable",				CFGID_Log_Enable, 0, 1 ),
	CFG_STR( "LogFile",				CFGID_Log_File, 0 ),
	
	CFG_VAL( "Infos",				CFGID_Log_Infos, 0, 1 ),
	CFG_VAL( "Errors",				CFGID_Log_Errors, 0, 1 ),
	CFG_VAL( "Warnings",			CFGID_Log_Warnings, 0, 1 ),
	
	CFG_VAL( "ProgramStart",		CFGID_Log_ProgramStart, 0, 1 ),
	CFG_VAL( "ProgramStop",			CFGID_Log_ProgramStop, 0, 1 ),
	
	CFG_VAL( "ServerStart",			CFGID_Log_ServerStart, 0, 1 ),
	CFG_VAL( "ServerStop",			CFGID_Log_ServerStop, 0, 1 ),
	
	CFG_VAL( "UserConnect",			CFGID_Log_UserConnect, 0, 1 ),
	CFG_VAL( "UserDisconnect",		CFGID_Log_UserDisconnect, 0, 1 ),

	CFG_END(),
};

static const struct CFGNODE myActions2[] =
{
	CFG_VAL( "ProgramStartEnable",		CFGID_Action_ProgramStartEnable, 0, 1 ),
	CFG_STR( "ProgramStartBuffer",		CFGID_Action_ProgramStart, MAX_ACTIONBUFFER ),

	CFG_VAL( "ProgramStopEnable",		CFGID_Action_ProgramStopEnable, 0, 1 ),
	CFG_STR( "ProgramStopBuffer",		CFGID_Action_ProgramStop, MAX_ACTIONBUFFER ),

	CFG_VAL( "ServerStartEnable",		CFGID_Action_ServerStartEnable, 0, 1 ),
	CFG_STR( "ServerStartBuffer",		CFGID_Action_ServerStart, MAX_ACTIONBUFFER ),

	CFG_VAL( "ServerStopEnable",		CFGID_Action_ServerStopEnable, 0, 1 ),
	CFG_STR( "ServerStopBuffer",		CFGID_Action_ServerStop, MAX_ACTIONBUFFER ),

	CFG_VAL( "UserConnectEnable",		CFGID_Action_UserConnectEnable, 0, 1 ),
	CFG_STR( "UserConnectBuffer",		CFGID_Action_UserConnect, MAX_ACTIONBUFFER ),

	CFG_VAL( "UserDisconnectEnable",	CFGID_Action_UserDisconnectEnable, 0, 1 ),
	CFG_STR( "UserDisconnectBuffer",	CFGID_Action_UserDisconnect, MAX_ACTIONBUFFER ),
	CFG_END(),
};

const struct GPRNODE ConfigGroups2[] =
{
	GPR_STR( "[PROGRAM]",	myProgram2 ),
	// --
	GPR_STR( "[SERVER]",	myServer2 ),
	GPR_STR( "[LOG]",		myLog2 ),
	GPR_STR( "[ACTIONS]",	myActions2 ),
	GPR_STR( "[IP]",		myIP2 ),
	GPR_END()
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
	cfg->AutoStart = TRUE;
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
		DEBUGPRINTF( DebugPrintF( "Zero length string not allowed\n" );
		goto bailout;
	}

	#endif

	str = mem_AllocClr( len + 1 );

	if ( str == NULL )
	{
		SHELLBUF_PRINTF1( "Error allocating %d bytes\n", len + 1 );
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
