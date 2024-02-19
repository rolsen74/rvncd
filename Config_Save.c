 
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

static const char *	ConfigHeaderStr	= "[RVNC SERVER CONFIG]\n";

enum myLabelType
{
	LT_Config_vb,		// uint8  
	LT_Config_u8,		// uint8  
	LT_Config_s32,		//  int32 
	LT_Config_str,		// char []
	LT_Config_strptr,	// char *
	LT_Config_gstr,		// char * (global)
};

struct myLabel
{
	const char *	Name;
	const int		Type;
	const int		Offset;
	const void *	Pointer;
};

struct myGroup
{
	const char *Name;
	const struct myLabel *Labels;
};

static const struct myLabel myProgram[] =
{
{ "Verbose",				LT_Config_vb, 0, NULL },
{ "DisableGUI",				LT_Config_u8, offsetof( struct Config, cfg_ProgramDisableGUI ), NULL },
{ "DisableARexx",			LT_Config_u8, offsetof( struct Config, cfg_ProgramDisableARexx ), NULL },
{ "DisableCxBroker",		LT_Config_u8, offsetof( struct Config, cfg_ProgramDisableCxBroker ), NULL },
{ "WinKeyStatus",			LT_Config_s32, offsetof( struct Config, cfg_WinData[WIN_KeyLogger].Status ), NULL },
{ "WinMainStatus",			LT_Config_s32, offsetof( struct Config, cfg_WinData[WIN_Main].Status ), NULL },
{ "WinAboutStatus",			LT_Config_s32, offsetof( struct Config, cfg_WinData[WIN_About].Status ), NULL },
{ "WinEncodingsStatus",		LT_Config_s32, offsetof( struct Config, cfg_WinData[WIN_Encodings].Status ), NULL },
{ "WinSessionStatus",		LT_Config_s32, offsetof( struct Config, cfg_WinData[WIN_Session].Status ), NULL },
{ "WinPixelStatus",			LT_Config_s32, offsetof( struct Config, cfg_WinData[WIN_PixelFormat].Status ), NULL },
{ NULL, 0, 0, NULL },
};

static const struct myLabel myActions[] =
{
{ "ProgramStartEnable",		LT_Config_u8, offsetof( struct Config, cfg_ActionsProgramStartEnable ), NULL },
{ "ProgramStartBuffer",		LT_Config_gstr, 0, ActionBuffer_ProgramStart },
{ "ProgramStopEnable",		LT_Config_u8, offsetof( struct Config, cfg_ActionsProgramStopEnable ), NULL },
{ "ProgramStopBuffer",		LT_Config_gstr, 0, ActionBuffer_ProgramStop },
{ "ServerStartEnable",		LT_Config_u8, offsetof( struct Config, cfg_ActionsServerStartEnable ), NULL },
{ "ServerStartBuffer",		LT_Config_gstr, 0, ActionBuffer_ServerStart },
{ "ServerStopEnable",		LT_Config_u8, offsetof( struct Config, cfg_ActionsServerStopEnable ), NULL },
{ "ServerStopBuffer",		LT_Config_gstr, 0, ActionBuffer_ServerStop },
{ "UserConnectEnable",		LT_Config_u8, offsetof( struct Config, cfg_ActionsUserConnectEnable ), NULL },
{ "UserConnectBuffer",		LT_Config_gstr, 0, ActionBuffer_UserConnect },
{ "UserDisconnectEnable",	LT_Config_u8, offsetof( struct Config, cfg_ActionsUserDisconnectEnable ), NULL },
{ "UserDisconnectBuffer",	LT_Config_gstr, 0, ActionBuffer_UserDisconnect },
{ NULL, 0, 0, NULL },
};

static const struct myLabel myLog[] =
{
{ "LogFile",				LT_Config_strptr, offsetof( struct Config, cfg_LogFileName ), NULL },
{ "Enable",					LT_Config_u8, offsetof( struct Config, cfg_LogEnable ), NULL },
{ "ProgramStart",			LT_Config_u8, offsetof( struct Config, cfg_LogProgramStart ), NULL },
{ "ProgramStop",			LT_Config_u8, offsetof( struct Config, cfg_LogProgramStop ), NULL },
{ "ServerStart",			LT_Config_u8, offsetof( struct Config, cfg_LogServerStart ), NULL },
{ "ServerStop",				LT_Config_u8, offsetof( struct Config, cfg_LogServerStop ), NULL },
{ "UserConnect",			LT_Config_u8, offsetof( struct Config, cfg_LogUserConnect ), NULL },
{ "UserDisconnect",			LT_Config_u8, offsetof( struct Config, cfg_LogUserDisconnect ), NULL },
{ "Infos",					LT_Config_u8, offsetof( struct Config, cfg_ProgramLogInfos ), NULL },
{ "Warnings",				LT_Config_u8, offsetof( struct Config, cfg_ProgramLogErrors ), NULL },
{ "Errors",					LT_Config_u8, offsetof( struct Config, cfg_ProgramLogWarnings ), NULL },
{ NULL, 0, 0, NULL },
};

static const struct myLabel myServer[] =
{
{ "Port",					LT_Config_s32, offsetof( struct Config, cfg_Disk_Settings.Port ), NULL },
{ "Name",					LT_Config_str, offsetof( struct Config, cfg_Disk_Settings.Name ), NULL },
{ "Password",				LT_Config_str, offsetof( struct Config, cfg_Disk_Settings.Password ), NULL },
{ "EncZLIB",				LT_Config_u8, offsetof( struct Config, cfg_Disk_Settings.ZLib ), NULL },
{ "UpdateSync",				LT_Config_u8, offsetof( struct Config, cfg_Disk_Settings.BufferSync ), NULL },
{ "DisableBlanker",			LT_Config_u8, offsetof( struct Config, cfg_Disk_Settings.DisableBlanker ), NULL },
{ "SendClipboard",			LT_Config_u8, offsetof( struct Config, cfg_Disk_Settings.SendClipboard ), NULL },
{ "ReadClipboard",			LT_Config_u8, offsetof( struct Config, cfg_Disk_Settings.ReadClipboard ), NULL },
{ "AutoStart",				LT_Config_u8, offsetof( struct Config, cfg_Disk_Settings2.AutoStart ), NULL },
{ "EncRichCursor",			LT_Config_u8, offsetof( struct Config, cfg_Disk_Settings.RichCursor ), NULL },
{ "TileSize",				LT_Config_s32, offsetof( struct Config, cfg_TileSize ), NULL },
{ "ScanMethod",				LT_Config_s32, offsetof( struct Config, GfxRead_BufferScanMethod ), NULL },
{ "DelayFrequency",			LT_Config_s32, offsetof( struct Config, GfxRead_DelayFrequency ), NULL },
{ "PointerFile",			LT_Config_strptr, offsetof( struct Config, cfg_PointerFileName ), NULL },
{ NULL, 0, 0, NULL },
};

static const struct myGroup myGroups[] =
{
{ "Program",	myProgram },
{ "Actions",	myActions },
{ "Server",		myServer },
{ "Log",		myLog },
{ NULL,			NULL }
};

// --

static int myTest( struct Config *cfg, BPTR h );

// --

static int Config_Save_Whitelist( struct Config *cfg, char *buffer, BPTR h )
{
struct IPNode *node;
char *str;
int error;
int stat;
int len;

	error = TRUE;

	#if 0

	[WhiteList]
	 IP = 192.168.1.211 [2]
	 IP = 192.168.*.*
	 IP = 10.0.*.*

	#endif

	str = "\n[Whitelist]\n";
	stat = IDOS->Write( h, str, strlen( str ));

	if ( stat <= 0 )
	{
		goto bailout;
	}

	node = (APTR) IExec->GetHead( & cfg->WhiteList );

	while( node )
	{
		sprintf( buffer, " IP = " );

		// -- A

		len = strlen( buffer );

		if ( node->ipn_A == -1 )
		{
			sprintf( & buffer[len], "*" );
		}
		else
		{
			sprintf( & buffer[len], "%d", node->ipn_A );
		}

		// -- B

		len = strlen( buffer );

		if ( node->ipn_B == -1 )
		{
			sprintf( & buffer[len], ".*" );
		}
		else
		{
			sprintf( & buffer[len], ".%d", node->ipn_B );
		}

		// -- C

		len = strlen( buffer );

		if ( node->ipn_C == -1 )
		{
			sprintf( & buffer[len], ".*" );
		}
		else
		{
			sprintf( & buffer[len], ".%d", node->ipn_C );
		}

		// -- D

		len = strlen( buffer );

		if ( node->ipn_D == -1 )
		{
			sprintf( & buffer[len], ".*" );
		}
		else
		{
			sprintf( & buffer[len], ".%d", node->ipn_D );
		}

		// -- Config nr

		len = strlen( buffer );

		if ( node->ipn_CfgNr >= 0 )
		{
			sprintf( & buffer[len], " [%d]\n", node->ipn_CfgNr );
		}
		else
		{
			sprintf( & buffer[len], "\n" );
		}

		// -- Write

		stat = IDOS->Write( h, buffer, strlen( buffer ));

		if ( stat <= 0 )
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

static int Config_Save_Blacklist( struct Config *cfg, char *buffer, BPTR h )
{
struct IPNode *node;
char *str;
int error;
int stat;
int len;

	error = TRUE;

	#if 0

	[BlackList]
	 IP = *.*.*.*

	#endif

	str = "\n[Blacklist]\n";
	stat = IDOS->Write( h, str, strlen( str ));

	if ( stat <= 0 )
	{
		goto bailout;
	}

	node = (APTR) IExec->GetHead( & cfg->BlackList );

	while( node )
	{
		sprintf( buffer, " IP = " );

		// -- A

		len = strlen( buffer );

		if ( node->ipn_A == -1 )
		{
			sprintf( & buffer[len], "*" );
		}
		else
		{
			sprintf( & buffer[len], "%d", node->ipn_A );
		}

		// -- B

		len = strlen( buffer );

		if ( node->ipn_B == -1 )
		{
			sprintf( & buffer[len], ".*" );
		}
		else
		{
			sprintf( & buffer[len], ".%d", node->ipn_B );
		}

		// -- C

		len = strlen( buffer );

		if ( node->ipn_C == -1 )
		{
			sprintf( & buffer[len], ".*" );
		}
		else
		{
			sprintf( & buffer[len], ".%d", node->ipn_C );
		}

		// -- D

		len = strlen( buffer );

		if ( node->ipn_D == -1 )
		{
			sprintf( & buffer[len], ".*\n" );
		}
		else
		{
			sprintf( & buffer[len], ".%d\n", node->ipn_D );
		}

		// -- Write

		stat = IDOS->Write( h, buffer, strlen( buffer ));

		if ( stat <= 0 )
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

void Config_Save( struct Config *cfg, char *filename )
{
char *tmpname;
char *str;
char *buf;
BPTR h;
int error;
int stat;

	h = 0;

	error = TRUE;

	tmpname = NULL;


	if ( filename == NULL )
	{
		filename = "RVNCd.config";
	}

	buf = myMalloc( 1024 );

	if ( buf == NULL )
	{
		goto bailout;
	}

	tmpname = myASPrintF( "%S.tmp", filename );

	if ( tmpname == NULL )
	{
		goto bailout;
	}

	h = IDOS->Open( tmpname, MODE_NEWFILE );

	if ( h == 0 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error opening file '%s'", filename );
		goto bailout;
	}

	/*
	** File Header
	*/

	str = (APTR) ConfigHeaderStr;
	stat = IDOS->Write( h, str, strlen( str ));

	if ( stat <= 0 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error writing to file" );
		goto bailout;
	}

	/*
	** Groups
	*/

	if ( myTest( cfg, h ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error writing to file" );
		goto bailout;
	}

	if ( Config_Save_Whitelist( cfg, buf, h ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error writing to file" );
		goto bailout;
	}

	if ( Config_Save_Blacklist( cfg, buf, h ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error writing to file" );
		goto bailout;
	}

	#if 0
	[ServerCfg:2]
	#endif

	// --

	if ( cfg->cfg_Config_FileName != filename )
	{
		if ( cfg->cfg_Config_FileName )
		{
			myFree( cfg->cfg_Config_FileName );
		}

		cfg->cfg_Config_FileName = myStrdup( filename );
	}

	// --

	Log_PrintF( cfg, LOGTYPE_Info, "Config saved to %s", filename );

	// --

	error = FALSE;

bailout:

	if ( h )
	{
		IDOS->Close( h );

		// --

		if ( ! error )
		{
			IDOS->Delete( filename );
			IDOS->Rename( tmpname, filename );
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
}

// --

static int myTest2( struct Config *cfg, BPTR h, const char *grpname, const struct myLabel *labels )
{
static char buffer[1024];
char **strptr;
char *gstr;
char *buf;
char *str;
int32 *s32;
uint8 *u8;
int stat;
int err;
int pos;

	pos = 0;
	err = TRUE;

	// -- Write Header

	sprintf( buffer, "\n[%s]\n", grpname );

	stat = IDOS->Write( h, buffer, strlen( buffer ));

	if ( stat <= 0 )
	{
		goto bailout;
	}

	// --

	while( labels[pos].Name )
	{
		switch( labels[pos].Type )
		{
			case LT_Config_vb:
			{
				if ( DoVerbose )
				{
					sprintf( buffer, " Verbose = 1\n" );
				}
				else
				{
					// Writing 0 will over write the -v shell option
					// so we do not set it by default
					sprintf( buffer, "; Verbose = 1\n" );
				}

				stat = IDOS->Write( h, buffer, strlen( buffer ));
				break;
			}

			case LT_Config_u8: // uint8 type
			{
				u8 = (void *) ( (uint32) cfg + labels[pos].Offset );

				sprintf( buffer, " %s = %d\n", labels[pos].Name, *u8 );

				stat = IDOS->Write( h, buffer, strlen( buffer ));
				break;
			}

			case LT_Config_s32: // int32 type
			{
				s32 = (void *) ( (uint32) cfg + labels[pos].Offset );

				sprintf( buffer, " %s = %ld\n", labels[pos].Name, *s32 );

				stat = IDOS->Write( h, buffer, strlen( buffer ));
				break;
			}

			case LT_Config_str: // char []
			{
				str = (void *) ( (uint32) cfg + labels[pos].Offset );

				buf = myASPrintF( " %s = \"%s\"\n", labels[pos].Name, str );

				if ( buf == NULL )
				{
					stat = -1;
					break;
				}

				stat = IDOS->Write( h, buf, strlen( buf ));

				myFree( buf );
				break;
			}

			case LT_Config_strptr: // char *
			{
				strptr = (void *) ( (uint32) cfg + labels[pos].Offset );

				str = ( *strptr ) ? *strptr : "";

				buf = myASPrintF( " %s = \"%s\"\n", labels[pos].Name, str );

				if ( buf == NULL )
				{
					stat = -1;
					break;
				}

				stat = IDOS->Write( h, buf, strlen( buf ));

				myFree( buf );
				break;
			}

			case LT_Config_gstr: // Global String
			{
				gstr = (void *) labels[pos].Pointer;

				buf = myASPrintF( " %s = \"%s\"\n", labels[pos].Name, gstr );

				if ( buf == NULL )
				{
					stat = -1;
					break;
				}

				stat = IDOS->Write( h, buf, strlen( buf ));

				myFree( buf );
				break;
			}

			default:
			{
				printf( "Illegal Label Type\n" );
				goto bailout;
			}
		}

		if ( stat <= 0 )
		{
			printf( "myTest2 : Write error\n" );
			goto bailout;
		}

		pos++;
	}

	// --

	err = FALSE;

bailout:

	return( err );
}

// --

static int myTest( struct Config *cfg, BPTR h )
{
int err;
int pos;

	err = TRUE;
	pos = 0;

	while( myGroups[pos].Name )
	{
		if ( myTest2( cfg, h, myGroups[pos].Name, myGroups[pos].Labels ))
		{
			printf( "myTest error\n" );
			goto bailout;
		}
		else
		{
			pos++;
		}
	}

	err = FALSE;

bailout:

	return( err );
}

// --
