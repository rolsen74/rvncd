
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_CONFIG_H__
#define __INC__RVNCD_CONFIG_H__

// --

enum CFGTYPE
{
	CFGTYPE_Dummy,
	CFGTYPE_IP,
	CFGTYPE_VAL,
	CFGTYPE_STR,
	CFGTYPE_Last
};

enum CFGID
{
	CFGID_Dummy,
	CFGID_Verbose,
	CFGID_Program_DisableGUI,
	CFGID_Program_DisableARexx,
	CFGID_Program_DisableCxBroker,

	// Server
	CFGID_Server_Port,
	CFGID_Server_Name,
	CFGID_Server_EncRRE,
	CFGID_Server_EncZLIB,
	CFGID_Server_SendBell,
	CFGID_Server_TileSize,
	CFGID_Server_Password,
	CFGID_Server_AutoStart,
	CFGID_Server_Protocol33,
	CFGID_Server_Protocol37,
	CFGID_Server_ScanMethod,
	CFGID_Server_PointerType,
	CFGID_Server_PointerFile,
	CFGID_Server_SendWatchDog,
	CFGID_Server_SendClipboard,
	CFGID_Server_ReadClipboard,
	CFGID_Server_EncRichCursor,
	CFGID_Server_SendTileBuffer,
	CFGID_Server_DisableBlanker,
	CFGID_Server_DelayFrequency,

	// Log
	CFGID_Log_Enable,
	CFGID_Log_File,
	CFGID_Log_Infos,
	CFGID_Log_Errors,
	CFGID_Log_Warnings,
	CFGID_Log_ProgramStart,
	CFGID_Log_ProgramStop,
	CFGID_Log_ServerStart,
	CFGID_Log_ServerStop,
	CFGID_Log_UserConnect,
	CFGID_Log_UserDisconnect,

	// Actions
	CFGID_Action_ProgramStopEnable,
	CFGID_Action_ProgramStartEnable,
	CFGID_Action_ServerStartEnable,
	CFGID_Action_ServerStopEnable,
	CFGID_Action_UserConnectEnable,
	CFGID_Action_UserDisconnectEnable,
	CFGID_Action_ProgramStop,
	CFGID_Action_ProgramStart,
	CFGID_Action_ServerStart,
	CFGID_Action_ServerStop,
	CFGID_Action_UserConnect,
	CFGID_Action_UserDisconnect,

	// IP
	CFGID_IP_Allow,
	CFGID_IP_Block,

	CFGID_Last
};

struct CFGNODE
{
	STR				Name;
	S32				NameLen;
	enum CFGTYPE	Type;
	enum CFGID		ID;
	S32				MinVal;
	S32				MaxVal;
	STR				String;
};

struct GPRNODE
{
	STR				Name;
	S32				NameLen;
	const struct CFGNODE *Cmds;
};

#define CFG_VAL(name,id,min,max)		{ name, sizeof(name)-1, CFGTYPE_VAL, id, min, max, NULL }
#define CFG_STR(name,id,max)			{ name, sizeof(name)-1, CFGTYPE_STR, id, 0, max, NULL }
#define CFG_IP( name,id)				{ name, sizeof(name)-1, CFGTYPE_IP, id, 0, 0, NULL }
#define CFG_END()						{ NULL, 0, CFGTYPE_Dummy, CFGID_Dummy, 0, 0, NULL }

#define GPR_STR(name,grp)				{ name, sizeof(name)-1, grp }
#define GPR_END()						{ NULL, 0, NULL }

// --
// -- Config

//#define CLT_Config_ip		0	//  0 : IP
#define CLT_Config_vb		1	//  1 : Verbose
#define CLT_Config_bstr		2	//  2 : String (Buffer) - snprintf
#define CLT_Config_gstr		3	//  3 : String (Global) - snprintf
#define CLT_Config_pstr		4	//  4 : String (Pointer) - strdup
#define CLT_Config_u8		5	//  5 : U8  - With range check
#define CLT_Config_u32		6	//  6 : U32 - With range check
#define CLT_Config_s32		7	//  7 : S32 - With range check
#define CLT_Config_in_ip4	8	//  8 : Include IP4 addr (allow)
#define CLT_Config_ex_ip4	9	//  9 : Exclude IP4 addr (block)

struct Cfg_Label
{
	const S32		Len;
	const STR		Name;
	const S32		Type;
	const S32		Arg1;
	const S32		Arg2;
	const S32		Offset;
	const PTR 		Pointer;
};

struct Cfg_Group
{
	const S32		Len;
	const STR		Name;
	const struct Cfg_Label *Cmds;
};

// --

enum ServerMode
{
	SM_Strict,		// One message, one reply, running in sync
	SM_Fast,		
};

// --

struct ClipData
{
	struct Config * cd_Config;
	struct Task *	cd_Task;
	S32				cd_ID;
	S32				cd_Ignore;		// Its our own
};

// -- Settings that just need a Server Restart

struct Settings
{
	U32			Port;								// Save
	U32			SendWatchDog;						// Save
	S32			TileSize;							// Save : Tile Size for Graphics Ripper
	U32			TileBufferKB;						// Save : Sending Tile Buffer in KB
	// --
	U8			RRE;								// Save
	U8			ZLib;								// Save
	U8			RichCursor;							// Save
	U8			DisableBlanker;						// Save
	U8			SendBell;							// Save
	U8			SendClipboard;						// Save
	U8			ReadClipboard;						// Save
	U8			Protocol33;							// Save
	U8			Protocol37;							// Save
	U8			ServerMode;							// Save
	U8			ScreenViewMode;
	U8			PointerType;						// Save : 0 = Built in, 1 = Custom
	U8			TileScanMethod;						// Save : 
	char		Name[MAX_SERVERNAME+1];				// Save
	char		Password[MAX_SERVERPASSWORD+1];		// Save
};

// --

struct Config
{
	// -- Program

	U32						cfg_StructID;

	S32						UserCount;

	STR						cfg_Startup_Config_Filename;
	STR						cfg_Config_Filename;
	struct Node *			cfg_IPEditNode;

	S32						cfg_LogFileNumber;
	S32						cfg_LogFileLimit;
	STR						cfg_LogFilename;					// Save
	struct SignalSemaphore	cfg_LogPrintFSema;

	STR						cfg_PointerFilename;				// Save
	STR						cfg_PointerFilename2;				// Internal
	U8 *					cfg_PointerChunky;
	U8 *					cfg_PointerBuffer;
	S32						cfg_PointerBufferSize;
	S32						cfg_PointerBufferMod;
	U8 *					cfg_PointerMask;
	U8 *					cfg_PointerMask2;
	S32						cfg_PointerMaskSize;
	S32						cfg_PointerMaskMod;
	S32						cfg_PointerWidth;
	S32						cfg_PointerHeight;
	struct PixelMessage		cfg_PointerFormat;
	S32						cfg_PointerFormatID;				// should match GfxRead_Encode_ActivePixelID

	#ifdef HAVE_ZLIB
	z_stream				cfg_zStream;
	#endif // HAVE_ZLIB

	STR						cfg_NetReason;

	S32						cfg_MouseDrawn;
	S32						cfg_OldMouseX;
	S32						cfg_OldMouseY;
	S32						cfg_CurMouseX;
	S32						cfg_CurMouseY;
	S32						cfg_RenderMouseX;
	S32						cfg_RenderMouseY;

	// -- Stats
	U32						Connect_Accecpted;
	U32						Connect_Allowed_IPs;
	U32						Connect_Rejected;
	U32						Connect_Blocked_IPs;

	// -- Settings
	struct Settings			cfg_Disk_Settings;
	struct Settings			cfg_Active_Settings;

	// -- User Config on/off settings
	U8						cfg_LogEnable;						// Save
	U8						cfg_LogProgramStart;				// Save
	U8						cfg_LogProgramStop;					// Save
	U8						cfg_LogServerStart;					// Save
	U8						cfg_LogServerStop;					// Save
	U8						cfg_LogUserConnect;					// Save
	U8						cfg_LogUserDisconnect;				// Save
	U8						cfg_Program_DisableARexx;			// Save
	U8						cfg_Program_DisableGUI;				// Save
	U8						cfg_Program_DisableCxBroker;		// Save
	U8						cfg_ProgramLogInfos;				// Save
	U8						cfg_ProgramLogErrors;				// Save
	U8						cfg_ProgramLogWarnings;				// Save
	U8						cfg_ActionsProgramStartEnable;		// Save
	U8						cfg_ActionsProgramStopEnable;		// Save
	U8						cfg_ActionsServerStartEnable;		// Save
	U8						cfg_ActionsServerStopEnable;		// Save
	U8						cfg_ActionsUserConnectEnable;		// Save
	U8						cfg_ActionsUserDisconnectEnable;	// Save
	U8						AutoStart;							// Save
	U8						MainWinState;						// Save

	// Internal Settings
	U8						cfg_LogInit;
	U8						cfg_ServerStatus;					// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	U8						cfg_ServerShutdown;
	U8						cfg_ClientRunning;
	U8						cfg_ServerGotSetEncoding;
	U8						cfg_ServerZLibAvailable;
	U8						cfg_ServerNumLock;
	U8						cfg_ServerPrintSession;
	U8						cfg_ServerSupportsCursor;
	U8						cfg_UpdateCursor;
	U8						cfg_GfxReadStatus;
	U8						cfg_NetReadStatus;
	U8						cfg_NetSendStatus;
	U8						cfg_WatchDogStatus;
	U8						cfg_ShellBufStatus;
	U8						cfg_SessionUsed;
	U8						cfg_UserConnectSignal;
	U8						cfg_UserDisconnectSignal;
	U8						cfg_SettingChanged;
	U8						cfg_KickUser;
	U8						cfg_ServerCursorStat;				// Internal Status
	U8						cfg_IPEditStat;						// 0 = Free, 1 = Create New, 2 = Edit Old
	U8						cfg_LastRect;

	// --

	struct VNCList			IPList;
	struct SignalSemaphore	IPSema;
	struct WinNode			cfg_WinData[ WIN_LAST ];

	struct SessionInfo		SessionStatus;						// Status about User connections

	// -- Gfx Read Task
	struct Task *			GfxRead_Task;
	struct Task *			GfxRead_Exit;
	S32						GfxRead_DelayFrequency;				// Save
	struct Screen *			GfxRead_PubScreenAdr;
	S32						GfxRead_Screen_SupportChange;
	S32						GfxRead_Screen_TileSize;			// Tile Pixel size .. ie 64x64
	S32						GfxRead_Encode_ActivePixelSet;
	// -- Gfx Read Task -- Screen Info with Sema Lock
	struct Screen *			GfxRead_Screen_Adr;
	struct SignalSemaphore	GfxRead_Screen_Sema;
	S32						GfxRead_Screen_ModeID;
	S32						GfxRead_Screen_ViewMode;
	S32						GfxRead_Screen_PageWidth;			// Pixel Width 
	S32						GfxRead_Screen_PageHeight;			// Pixel Height
	S32						GfxRead_Screen_ViewWidth;			// Visual Pixel Width 
	S32						GfxRead_Screen_ViewHeight;			// Visual Pixel Height
	enum VNCPix				GfxRead_Screen_Format;				// Pixel Format
	U32						GfxRead_Screen_Tiles;				// Total number of Tiles
	S32						GfxRead_Screen_TileWidth;			// Tile : Number of Columns
	S32						GfxRead_Screen_TileHeight;			// Tile : Number of Rows
	S32 *					GfxRead_Screen_TileRandomBuffer;	// Random Buffer
	struct TileInfo *		GfxRead_Screen_TileInfoBuffer;		// Holds X, Y, W, H, Clr info
	S8 *					GfxRead_Screen_TileArrayBuffer;		// Array Buffer
	S32						GfxRead_Screen_TileCheckSize;
	U8 *					GfxRead_Screen_TileCheckBuffer;		// Tile Check Buffer
	S32						GfxRead_Screen_ChunkySize;			// Chunk size in bytes
	U8 *					GfxRead_Screen_ChunkyBuffer;		// Chunky w * h * 4
	enum VNCPix				GfxRead_Encode_Format;				// 
	enum VNCPix				GfxRead_Encode_OldFormat;			// 
	S32						GfxRead_Encode_Format2;				// VNCPix -> x ()
	S32						GfxRead_Encode_FormatSize;			// 1, 2 or 4 bytes
	struct PixelMessage		GfxRead_Encode_ActivePixel;
	S32						GfxRead_Encode_ActivePixelID;		// Counter, inc. when Format change
	S32						GfxRead_Encode_RedShift;
	S32						GfxRead_Encode_GreenShift;
	S32						GfxRead_Encode_BlueShift;
	STR						GfxRead_Encode_FuncName;
	S32						(*GfxRead_Encode_RenderTile)	( struct Config *cfg, PTR buf, U32 tilenr );

	// -- Net Send Task
	struct Task *			NetSend_Task;
	struct Task *			NetSend_Exit;
	U8 * 					NetSend_SendBuffer;							// Send Tile/Encoding buffer
	S32						NetSend_SendBufferSize;						// max 256x256x4 + 4096 + (4095 rounding) bytes
	U8 *					NetSend_EncBuffer;							// Copy of Tile buffer
	S32						NetSend_EncBufferSize;						// max 256x256x4 + (4095 rounding) bytes
	S32						NetSend_EncBufferUsed;						// How many bytes we have filled
	U8 *					NetSend_TileBuffer;
	U32						NetSend_TileBufferUsed;						// How many bytes we have used/filled
	U32						NetSend_TileBufferSize;						// How many bytes that was allocated
	S32						NetSend_ClientSocket;
	struct Library *		NetSend_SocketBase;
	struct SocketIFace *	NetSend_ISocket;
	S32						NetSend_ScreenCurrentTile;
	S32						NetSend_ClientProtocol;
	U8						NetSend_Encodings1[ ENCODE_LAST ];			// Fast 'enum VNCEncoding' Enable test
	struct myEncNode		NetSend_Encodings2[ ENCODE_LAST ];			// Sorted Encoding list
	struct IOClipReq *		NetSend_ClipBoardIOReq;
	struct VNCMsgPort		NetSend_ClipBoardMsgPort;
	struct Library *		NetSend_ClipBoardBase;
	struct ClipData			NetSend_ClipBoardData;
	struct Hook				NetSend_ClipBoardHook;
	S32						NetSend_ClipBoardHookInstalled;
	S32						NetSend_ClipBoardSignalNr;
	U32						NetSend_ClipBoardSignal;

	// -- Net Read Task
	struct Task *			NetRead_Task;
	struct Task *			NetRead_Exit;
	PTR 					NetRead_ReadBuffer;
	S32						NetRead_ReadBufferSize;
	S32						NetRead_ClientSocket;
	struct Library *		NetRead_SocketBase;
	struct SocketIFace *	NetRead_ISocket;
	S32						NetRead_Idle;
	struct IOStdReq *		NetRead_InputIOReq;
	struct VNCMsgPort		NetRead_InputMsgPort;
	struct Library *		NetRead_InputBase;
	struct InputIFace *		NetRead_IInput;
	U16						NetRead_KeyboardPrevCode[2];
	U16						NetRead_KeyboardPrevQual[2];
	U32						NetRead_OldMousePos;
	S32						NetRead_OldMouseButtons;
	S32						NetRead_Qualifier;

	// -- Server
	struct SignalSemaphore	Server_UpdateSema;
	struct VNCList			Server_UpdateList;
	struct VNCList			Server_UpdateFree;
	struct Task *			Server_Task;
	struct Task *			Server_Exit;
	struct Library *		Server_SocketBase;
	struct SocketIFace *	Server_ISocket;
	S32						Server_ListenSocket;
	S32						Server_ClientSocket;
	S32						Server_DuplicateSendSocket;
	S32						Server_DuplicateReadSocket;
	struct LogTime			Server_LogTime;

	// -- Shell Buffer Task
	struct Task *			ShellBuf_Task;
	struct Task *			ShellBuf_Exit;

	// -- WatchDog
	struct Task *			WatchDog_Task;
	struct Task *			WatchDog_Exit;
	S32						WatchDog_TimerActive;
	struct VNCMsgPort		WatchDog_ReplyPort;
	U32						WatchDog_ReplyPortBit;
	struct VNCTimeRequest	WatchDog_TimerIOReq;
};

// --

struct Config *	Config_Init(			void );
void			Config_Free(			struct Config *cfg );
S32				Config_Parse(			struct Config *cfg, S32 argc, STR *argv );
STR				Config_CopyString(		STR buffer );
S32				Config_ParseArgs(		struct Config *cfg, S32 argc, STR *argv, S32 early );
void			Config_Reset(			struct Config *cfg );
S32				Config_Save(			struct Config *cfg, STR Filename );
S32				Config_Read(			struct Config *cfg, STR Filename, S32 CfgNeeded );

// --

extern const struct Cfg_Group ConfigGroups[];
extern const struct GPRNODE ConfigGroups2[];
extern const STR ConfigHeaderStr;

// --

#endif // __INC__RVNCD_CONFIG_H__
