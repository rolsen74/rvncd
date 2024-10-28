
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

#ifndef __RVNCD_H__
#define __RVNCD_H__

// --

#define __USE_BASETYPE__

#include <proto/application.h>
#include <proto/arexx.h>
#include <proto/asl.h>
#include <proto/bitmap.h>
#include <proto/button.h>
#include <proto/checkbox.h>
#include <proto/chooser.h>
#include <proto/clicktab.h>
#include <proto/commodities.h>
#include <proto/datatypes.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/getfile.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/input.h>
#include <proto/integer.h>
#include <proto/intuition.h>
#include <proto/keymap.h>
#include <proto/label.h>
#include <proto/layout.h>
#include <proto/listbrowser.h>
#include <proto/Picasso96API.h>
#include <proto/scroller.h>
#include <proto/slider.h>
#include <proto/string.h>
#include <proto/timer.h>
#include <proto/utility.h>
#include <proto/z.h>
#include <proto/window.h>

#include <classes/arexx.h>
#include <classes/window.h>
#include <datatypes/textclass.h>
#include <datatypes/pictureclass.h>
#include <devices/clipboard.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <gadgets/button.h>
#include <gadgets/checkbox.h>
#include <gadgets/chooser.h>
#include <gadgets/clicktab.h>
#include <gadgets/getfile.h>
#include <gadgets/layout.h>
#include <gadgets/listbrowser.h>
#include <gadgets/integer.h>
#include <gadgets/scroller.h>
#include <gadgets/slider.h>
#include <gadgets/string.h>
#include <graphics/gfx.h>
#include <images/bitmap.h>
#include <images/label.h>
#include <intuition/icclass.h>
#include <intuition/menuclass.h>
#include <libraries/iffparse.h>
#include <reaction/reaction_macros.h>

#define __NOLIBBASE__
#define __NOGLOBALIFACE__
#include <proto/bsdsocket.h>
#undef  __NOGLOBALIFACE__
#undef  __NOLIBBASE__

// --

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/select.h>

// --

#include "RVNCd_rev.h"
#include "d3des.h"
#include "VNC.h"

// --

#define ID_CFG					0x12345678

#define MAX_KeyListNodes		250
#define MAX_LogListNodes		250
#define MAX_SERVERS				1
#define MAX_SERVERNAME			256
#define MAX_SERVERPASSWORD		8
#define MAX_USERS				1
#define MAX_ACTIONBUFFER		1024

#ifndef MIN
#define MIN(a,b)				((( a ) < ( b )) ? ( a ) : ( b ))
#endif

#ifndef MAX
#define MAX(a,b)				((( a ) > ( b )) ? ( a ) : ( b ))
#endif

#define SWAP16(x)				(((x) & 0x00ffU ) << 8 | \
								 ((x) & 0xff00U ) >> 8 )

#define SWAP32(x)				(((x) & 0x000000ffUL ) << 24 | \
								 ((x) & 0x0000ff00UL ) <<  8 | \
								 ((x) & 0x00ff0000UL ) >>  8 | \
								 ((x) & 0xff000000UL ) >> 24 )

#ifndef INET_ADDRSTRLEN
#define INET_ADDRSTRLEN 		16
#endif

#ifndef End
#define End						TAG_END)
#endif

// -- Config

enum Cfg_LabelType
{
	CLT_Config_ip,		//  0 : IP
	CLT_Config_vb,		//  1 : Verbose
	CLT_Config_bstr,	//  2 : String (Buffer) - snprintf
	CLT_Config_gstr,	//  3 : String (Global) - snprintf
	CLT_Config_pstr,	//  4 : String (Pointer) - strdup
	CLT_Config_u8,		//  5 : uint8  - With range check
	CLT_Config_u32,		//  6 : uint32 - With range check
	CLT_Config_s32,		//  7 :  int32 - With range check
};

enum Cfg_ListType
{
	CLT_B,				// Black List
	CLT_W,				// White List
};

struct Cfg_Label
{
	const int		Len;
	const char *	Name;
	const int		Type;
	const int		Arg1;
	const int		Arg2;
	const int		Offset;
	const void *	Pointer;
};

struct Cfg_Group
{
	const int		Len;
	const char *	Name;
	const struct Cfg_Label *Cmds;
};

// -- IP Edit Status

enum
{
	IPES_Unused = 0,
	IPES_Black_Add,
	IPES_Black_Edit,
	IPES_White_Add,
	IPES_White_Edit,
};

// -- Windows

enum
{
	WIN_Main,		// Should be first
	WIN_About,
	WIN_Session,
	WIN_KeyLogger,
	WIN_Encodings,
	WIN_PixelFormat,
	WIN_IPEdit,
	WIN_Quit,
	WIN_LAST
};

// -- Window Status

enum WinStat
{
	WINSTAT_Close,
	WINSTAT_Open,
	WINSTAT_Iconifyed,
	WINSTAT_Last
};

// -- Process Status

enum
{
	PROCESS_Stopped,
	PROCESS_Starting,
	PROCESS_Running,
	PROCESS_Stopping,
};

// -- VNC Protocol supported

enum
{
	VNCProtocol_Unknown,
	VNCProtocol_33,				// Ver 3.3
	VNCProtocol_37,				// Ver 3.7
};

// -- Tile Scanning for Updates in a .. method

enum
{
	SCANMethod_Linear,
	SCANMethod_Random,
	SCANMethod_Last
};

// -- Buffer Update Stats

enum
{
	UPDATESTAT_Error,
	UPDATESTAT_Change,
	UPDATESTAT_NoChange
};

#define UPDATESTAT_Okay		UPDATESTAT_Change

// --

enum
{
	ENCODE_Raw,
	ENCODE_ZLib,
	ENCODE_RichCursor,
	ENCODE_LAST
};

enum
{
	VIEWMODE_View,
	VIEWMODE_Page,
};

// --

struct Config;

struct WinNode
{
	int16			XPos;
	int16			YPos;
	int16			Width;
	int16			Height;
	int16			Busy;
	int32			Status;			// Save
	struct Window *	WindowAdr;
	void			(*BusyWin)	( struct Config *cfg, int val );
	int				(*OpenWin)	( struct Config *cfg );
	void			(*CloseWin)	( struct Config *cfg );
	void			(*HandleWin)( struct Config *cfg );
};

// --

struct UpdateNode
{
	struct Node		un_Node;
	int				un_Incremental;
	int				un_XPos;
	int				un_YPos;
	int				un_Width;
	int				un_Height;
};

// --

enum
{
	CMD_Dummy,
	CMD_Key,
	CMD_PxlFmt,
	CMD_Session,		// Sesstion Start Stop 
	CMD_Encoding,
	CMD_LogString,
	CMD_RefreshGUI,
	CMD_ConnectStats,
};

struct CommandMessage
{
	struct Message		cm_Message;
	int					cm_Command;
};

struct CommandKey
{
	struct Message		ck_Message;
	int					ck_Command;
	// --
	char				ck_Client[64];
	char				ck_Amiga[64];
};

struct CommandPxlFmt
{
	struct Message		cpf_Message;
	int					cpf_Command;
	// --
	int					cpf_Client;
	struct PixelMessage	cpf_Format;
};			

struct CommandSession
{
	struct Message		cs_Message;
	int					cs_Command;
	// --
	uint32				cs_Starting;
};

struct CommandEncoding
{
	struct Message		ce_Message;
	int					ce_Command;
	// --
	int					ce_Encodings;
	uint32				ce_Data[1];
};

struct CommandRefresh
{
	struct Message		cr_Message;
	int					cr_Command;
	// --
};

enum
{
	LOGTYPE_Info = 0,
	LOGTYPE_Warning,
	LOGTYPE_Error,
	LOGTYPE_Event = 0x80,
};

struct CommandLogString
{
	struct Message		cl_Message;
	int					cl_Command;
	// --
	uint8				cl_Type;
	char				cl_Time[12];
	char				cl_String[1];
};

// --

struct myEncNode
{
	int16	Pos;
	int16	Type;
	int16	Enabled;
	int16	EncType;
};

// --

struct LogTime
{
	int					lt_Inited;			// Struct have been Initalized
	int					lt_Ended;			// Time Stopped
	struct TimeVal		lt_Logon;			// Start Tick
	struct TimeVal		lt_Logoff;			// End Tick
	struct DateStamp	lt_Connect;			// Connection Time
};

struct SessionInfo
{
	// -- Bytes Send/Read in total
	uint64				si_Read_Bytes;
	uint64				si_Send_Bytes;

	// -- Tiles Send
	uint64				si_Tiles_Raw;
	uint64				si_Tiles_ZLib;
	uint64				si_Tiles_Total;

	// -- Time
	struct LogTime		si_LogTime;

	// -- IP Addr
	int					si_IPAddr[4];	
};

// --

struct ClipData
{
	struct Config * cd_Config;
	struct Task *	cd_Task;
	int				cd_ID;
	int				cd_Ignore;		// Its our own
};

// -- Settings that just need a Server Restart

struct Settings
{
	uint32			Port;								// Save
	uint32			SendWatchDog;						// Save
	int32			TileSize;							// Save : Tile Size for Graphics Ripper
	// --
	uint8			ZLib;								// Save
	uint8			RichCursor;							// Save
	uint8			DisableBlanker;						// Save
	uint8			SendClipboard;						// Save
	uint8			ReadClipboard;						// Save
	uint8			Protocol33;							// Save
	uint8			Protocol37;							// Save
	uint8			ScreenViewMode;
	uint8			PointerType;						// Save : 0 = Built in, 1 = Custom
	uint8			TileScanMethod;						// Save : 
	char			Name[MAX_SERVERNAME+1];				// Save
	char			Password[MAX_SERVERPASSWORD+1];		// Save
};

// --

struct Config
{
	// -- Program

	uint32					cfg_ID;

	int						UserCount;

	char *					cfg_Startup_Config_Filename;
	char *					cfg_Config_Filename;
	struct Node *			cfg_IPEditNode;

	int						cfg_LogFileNumber;
	int						cfg_LogFileLimit;
	char *					cfg_LogFilename;					// Save
	struct SignalSemaphore	cfg_LogPrintFSema;

	char *					cfg_PointerFilename;				// Save
	char *					cfg_PointerFilename2;				// Internal
	uint8 *					cfg_PointerChunky;
	uint8 *					cfg_PointerBuffer;
	int						cfg_PointerBufferSize;
	int						cfg_PointerBufferMod;
	uint8 *					cfg_PointerMask;
	uint8 *					cfg_PointerMask2;
	int						cfg_PointerMaskSize;
	int						cfg_PointerMaskMod;
	int						cfg_PointerWidth;
	int						cfg_PointerHeight;
	struct PixelMessage		cfg_PointerFormat;
	int						cfg_PointerFormatID;				// should match GfxRead_Enocde_ActivePixelID

	z_stream				cfg_zStream;

	char *					cfg_NetReason;

	int						cfg_MouseDrawn;
	int						cfg_OldMouseX;
	int						cfg_OldMouseY;
	int						cfg_CurMouseX;
	int						cfg_CurMouseY;
	int						cfg_RenderMouseX;
	int						cfg_RenderMouseY;

	// -- Stats
	uint32					Connect_Accecpted;
	uint32					Connect_WhiteListed;
	uint32					Connect_Rejected;
	uint32					Connect_BlackListed;

	// -- Settings
	struct Settings			cfg_Disk_Settings;

	struct Settings			cfg_Active_Settings;

	// -- User Config on/off settings
	uint8					cfg_LogEnable;						// Save
	uint8					cfg_LogProgramStart;				// Save
	uint8					cfg_LogProgramStop;					// Save
	uint8					cfg_LogServerStart;					// Save
	uint8					cfg_LogServerStop;					// Save
	uint8					cfg_LogUserConnect;					// Save
	uint8					cfg_LogUserDisconnect;				// Save
	uint8					cfg_ProgramDisableARexx;			// Save
	uint8					cfg_ProgramDisableGUI;				// Save
	uint8					cfg_ProgramDisableCxBroker;			// Save
	uint8					cfg_ProgramLogInfos;				// Save
	uint8					cfg_ProgramLogErrors;				// Save
	uint8					cfg_ProgramLogWarnings;				// Save
	uint8					cfg_ActionsProgramStartEnable;		// Save
	uint8					cfg_ActionsProgramStopEnable;		// Save
	uint8					cfg_ActionsServerStartEnable;		// Save
	uint8					cfg_ActionsServerStopEnable;		// Save
	uint8					cfg_ActionsUserConnectEnable;		// Save
	uint8					cfg_ActionsUserDisconnectEnable;	// Save
	uint8					AutoStart;							// Save
	uint8					MainWinState;						// Save

	// Internal Settings
	uint8					cfg_LogInit;
	uint8					cfg_ServerStatus;					// 0 = off, 1 = starting, 2 = running, 3 = shutting down
	uint8					cfg_ServerShutdown;
	uint8					cfg_ClientRunning;
	uint8					cfg_ServerGotSetEncoding;
	uint8					cfg_ServerZLibAvailable;
	uint8					cfg_ServerNumLock;
	uint8					cfg_ServerPrintSession;
	uint8					cfg_ServerSupportsCursor;
	uint8					cfg_UpdateCursor;
	uint8					cfg_GfxReadStatus;
	uint8					cfg_NetReadStatus;
	uint8					cfg_NetSendStatus;
	uint8					cfg_WatchDogStatus;
	uint8					cfg_SessionUsed;
	uint8					cfg_UserConnectSignal;
	uint8					cfg_UserDisconnectSignal;
	uint8					cfg_SettingChanged;
	uint8					cfg_KickUser;
	uint8					cfg_ServerCursorStat;				// Internal Status
	uint8					cfg_IPEditStat;						// 0 = Free, 1 = Create New, 2 = Edit Old

	// --

	struct List				WhiteList;
	struct List				BlackList;
	struct WinNode			cfg_WinData[ WIN_LAST ];
	struct SessionInfo		SessionStatus;						// Status about User connections

	// -- Gfx Read Task
	struct Task *			GfxRead_Task;
	struct Task *			GfxRead_Exit;
	int32					GfxRead_DelayFrequency;				// Save
	struct Screen *			GfxRead_PubScreenAdr;
	int						GfxRead_Screen_SupportChange;
	int						GfxRead_Screen_TileSize;			// Tile Pixel size .. ie 64x64
	int						GfxRead_Enocde_ActivePixelSet;
	// -- Gfx Read Task -- Screen Info with Sema Lock
	struct Screen *			GfxRead_Screen_Adr;
	struct SignalSemaphore	GfxRead_Screen_Sema;
	int						GfxRead_Screen_ModeID;
	int						GfxRead_Screen_ViewMode;
	int						GfxRead_Screen_PageWidth;			// Pixel Width 
	int						GfxRead_Screen_PageHeight;			// Pixel Height
	int						GfxRead_Screen_ViewWidth;			// Visual Pixel Width 
	int						GfxRead_Screen_ViewHeight;			// Visual Pixel Height
	int						GfxRead_Screen_Format;				// Pixel Format
	int						GfxRead_Screen_Tiles;				// Total number of Tiles
	int						GfxRead_Screen_TileWidth;			// Tile : Number of Columns
	int						GfxRead_Screen_TileHeight;			// Tile : Number of Rows
	int *					GfxRead_Screen_TileRandomBuffer;	// Random Buffer
	struct TileInfo *		GfxRead_Screen_TileInfoBuffer;		// Holds X, Y, W, H, Clr info
	int8 *					GfxRead_Screen_TileArrayBuffer;		// Array Buffer
	int						GfxRead_Screen_TileCheckSize;
	uint8 *					GfxRead_Screen_TileCheckBuffer;		// Tile Check Buffer
	int						GfxRead_Screen_ChunkySize;			// Chunk size in bytes
	uint8 *					GfxRead_Screen_ChunkyBuffer;		// Chunky w * h * 4
	int						GfxRead_Encode_Format;				// PIX_FMT
	int						GfxRead_Encode_OldFormat;			// PIX_FMT
	int						GfxRead_Encode_FormatSize;			// 1, 2 or 4 bytes
	struct PixelMessage		GfxRead_Enocde_ActivePixel;
	int						GfxRead_Enocde_ActivePixelID;		// Counter, inc. when Format change
	int						GfxRead_Enocde_RedShift;
	int						GfxRead_Enocde_GreenShift;
	int						GfxRead_Enocde_BlueShift;
	char *					GfxRead_Encode_FuncName;
	int						(*GfxRead_Encode_RenderTile)	( struct Config *cfg, APTR buf, int tile );

	// -- Net Send Task
	struct Task *			NetSend_Task;
	struct Task *			NetSend_Exit;
	void *					NetSend_SendBuffer;
	int						NetSend_SendBufferSize;
	int						NetSend_ClientSocket;
	struct Library *		NetSend_SocketBase;
	struct SocketIFace *	NetSend_ISocket;
	int						NetSend_ScreenCurrentTile;
	int						NetSend_ClientProtocol;
	uint8					NetSend_Encodings1[ ENCODE_LAST ];			// Fast ENCODE_xx Enable test
	struct myEncNode		NetSend_Encodings2[ ENCODE_LAST ];			// Sorted Encoding list
	struct IOClipReq *		NetSend_ClipBoardIOReq;
	struct MsgPort *		NetSend_ClipBoardMsgPort;
	struct Library *		NetSend_ClipBoardBase;
	struct ClipData			NetSend_ClipBoardData;
	struct Hook				NetSend_ClipBoardHook;
	int						NetSend_ClipBoardHookInstalled;
	int						NetSend_ClipBoardSignalNr;
	uint32					NetSend_ClipBoardSignal;

	// -- Net Read Task
	struct Task *			NetRead_Task;
	struct Task *			NetRead_Exit;
	void *					NetRead_ReadBuffer;
	int						NetRead_ReadBufferSize;
	int						NetRead_ClientSocket;
	struct Library *		NetRead_SocketBase;
	struct SocketIFace *	NetRead_ISocket;
	int						NetRead_Idle;
	struct IOStdReq *		NetRead_InputIOReq;
	struct MsgPort *		NetRead_InputMsgPort;
	struct Library *		NetRead_InputBase;
	struct InputIFace *		NetRead_IInput;
	uint16					NetRead_KeyboardPrevCode[2];
	uint16					NetRead_KeyboardPrevQual[2];
	uint32					NetRead_OldMousePos;
	int						NetRead_OldMouseButtons;
	int						NetRead_Qualifier;

	// -- Server
	struct SignalSemaphore	Server_UpdateSema;
	struct List				Server_UpdateList;
	struct List				Server_UpdateFree;
	struct Task *			Server_Task;
	struct Task *			Server_Exit;
	struct Library *		Server_SocketBase;
	struct SocketIFace *	Server_ISocket;
	int						Server_ListenSocket;
	int						Server_ClientSocket;
	int32					Server_DuplicateSendSocket;
	int32					Server_DuplicateReadSocket;
	struct LogTime			Server_LogTime;

	// -- WatchDog
	struct Task *			WatchDog_Task;
	struct Task *			WatchDog_Exit;
	int						WatchDog_TimerActive;
	struct TimeRequest		WatchDog_TimerIOReq;
	struct MsgPort *		WatchDog_ReplyPort;
	uint32					WatchDog_ReplyPortBit;
};

// --

struct TileInfo
{
	int16				X;
	int16				Y;

	int16				W;
	int16				H;

	int16				C;	// Clear
};

struct StartMessage
{
	struct Task *	Parent;
	struct Config *	Config;
};

// --

struct ParseCommand
{
	int		Len;
	char *	Name;
	int	  (*Func)( struct Config *cfg, char *buf, int pos, int argpos, int arglen, uint32 nr );
};

// --

struct IPNode
{
	struct Node		ipn_Node;
	int				ipn_A;
	int				ipn_B;
	int				ipn_C;
	int				ipn_D;
};

// --

typedef struct
{
	void ( *Function )( struct Config * );

} GUIFunc;

// --

void			DoAction_ProgramStart(	struct Config *cfg );
void			DoAction_ProgramStop(	struct Config *cfg );
void			DoAction_ServerStart(	struct Config *cfg );
void			DoAction_ServerStop(	struct Config *cfg );
void			DoAction_UserConnect(	struct Config *cfg );
void			DoAction_UserDisconnect( struct Config *cfg );

int				StartServer(			struct Config *cfg );
int				StopServer(				struct Config *cfg );
int				RestartServer(			struct Config *cfg );
void			PrintProgramHeader(		void );

struct Config *	Config_Create(			void );
void			Config_Delete(			struct Config *cfg );
int				Config_Parse(			struct Config *cfg, int argc, char **argv );
char *			Config_CopyString(		char *buffer );
int				Config_ParseArgs(		struct Config *cfg, int argc, char **argv, int early );
void			Config_Reset(			struct Config *cfg );
int				Config_Save(			struct Config *cfg, char *Filename );
int				Config_Read(			struct Config *cfg, char *Filename, int CfgNeeded );

void			Log_Setup(				struct Config *cfg );

// Misc
int				myGetFileSize(			char *name, int *size );
char *			myStrError(				int err );
void			mySetFormat(			struct Config *cfg, int Format );

int				myStart_Server(			struct Config *cfg );
void			myStop_Server(			struct Config *cfg );

int				myStart_Net_Read(		struct Config *cfg );
void			myStop_Net_Read(		struct Config *cfg );

int				myStart_Net_Send(		struct Config *cfg );
void			myStop_Net_Send(		struct Config *cfg );

int				myStart_Gfx_Read(		struct Config *cfg );
void			myStop_Gfx_Read(		struct Config *cfg );

int				myStart_WatchDog(		struct Config *cfg );
void			myStop_WatchDog(		struct Config *cfg );
void			WatchDog_StartTimer(	struct Config *cfg );
void			WatchDog_StopTimer(		struct Config *cfg );

int				VNC_Authenticate_33(	struct Config *cfg );
int				VNC_Authenticate_37(	struct Config *cfg );
int				VNC_BufferUpdate(		struct Config *cfg, int xx, int yy, int ww, int hh );
int				VNC_ClientInit(			struct Config *cfg );
int				VNC_Clipboard(			struct Config *cfg );
int				VNC_Keyboard(			struct Config *cfg );
int				VNC_Mouse(				struct Config *cfg );
int				VNC_ServerInit(			struct Config *cfg );
int				VNC_SetEncoding(		struct Config *cfg );
int				VNC_SetPixelFormat(		struct Config *cfg );
int				VNC_TileUpdate(			struct Config *cfg, int tile );
int				VNC_UpdateRequest(		struct Config *cfg );
int				VNC_Version(			struct Config *cfg );

int				VNC_HandleCmds_33(		struct Config *cfg );
int				VNC_HandleCmds_37(		struct Config *cfg );

// --

void			myZLib_Setup(			struct Config *cfg );
void			myZLib_Cleanup(			struct Config *cfg );
void			Send_Blanker_Enable(	struct Config *cfg );
void			Send_Blanker_Disable(	struct Config *cfg );
int				Send_Install_ClipHook(	struct Config *cfg );
void			Send_Remove_ClipHook(	struct Config *cfg );
void			Send_Handle_ClipBoard(	struct Config *cfg );
int				Send_Open_Net(			struct Config *cfg );
void			Send_Close_Net(			struct Config *cfg );
int				Send_Open_Screen(		struct Config *cfg );
void			Send_Close_Screen(		struct Config *cfg );
int				Send_Open_Socket(		struct Config *cfg );
void			Send_Close_Socket(		struct Config *cfg );

// --

int				ARexx_Setup(			struct Config *cfg );
void			ARexx_Cleanup(			struct Config *cfg );
void			ARexx_Handle(			struct Config *cfg );

// --

void			myGUI_BusyMainWindow(			struct Config *cfg, int val );
int				myGUI_OpenMainWindow(			struct Config *cfg );
void			myGUI_CloseMainWindow(			struct Config *cfg );
void			myGUI_HandleMainWindow(			struct Config *cfg );
void			myGUI_Main_SessionMessage(		struct Config *cfg, struct CommandSession *msg );
void			myGUI_Main_CheckSettings(		struct Config *cfg );
void			myGUI_Main_RefreshSettings(		struct Config *cfg );
void			myGUI_Main_TimerTick(			struct Config *cfg );
void			myGUI_Main_UpdateBlackList(		struct Config *cfg );
void			myGUI_Main_UpdateWhiteList(		struct Config *cfg );
void			Page_Refresh_Black(				struct Config *cfg );
void			Page_Refresh_White(				struct Config *cfg );

void			myGUI_BusyKeyWindow(			struct Config *cfg, int val );
int				myGUI_OpenKeyWindow(			struct Config *cfg );
void			myGUI_CloseKeyWindow(			struct Config *cfg );
void			myGUI_HandleKeyWindow(			struct Config *cfg );
void			myGUI_AddKeyMessage(			struct Config *cfg, struct CommandKey *msg );

void			myGUI_BusyEncodingsWindow(		struct Config *cfg, int val );
int				myGUI_OpenEncodingsWindow(		struct Config *cfg );
void			myGUI_CloseEncodingsWindow(		struct Config *cfg );
void			myGUI_HandleEncodingsWindow(	struct Config *cfg );
void			myGUI_EncodingMessage(			struct Config *cfg, struct CommandEncoding *msg );

void			myGUI_BusyAboutWindow(			struct Config *cfg, int val );
int				myGUI_OpenAboutWindow(			struct Config *cfg );
void			myGUI_CloseAboutWindow(			struct Config *cfg );
void			myGUI_HandleAboutWindow(		struct Config *cfg );

void			myGUI_BusySessionWindow(		struct Config *cfg, int val );
int				myGUI_OpenSessionWindow(		struct Config *cfg );
void			myGUI_CloseSessionWindow(		struct Config *cfg );
void			myGUI_HandleSessionWindow(		struct Config *cfg );
void			myGUI_SessionMessage(			struct Config *cfg, struct CommandSession *msg );

void			myGUI_BusyPixelWindow(			struct Config *cfg, int val );
int				myGUI_OpenPixelWindow(			struct Config *cfg );
void			myGUI_ClosePixelWindow(			struct Config *cfg );
void			myGUI_HandlePixelWindow(		struct Config *cfg );
void			myGUI_PxlFmtMessage(			struct Config *cfg, struct CommandPxlFmt *msg );
void			myGUI_PxlFmtRefresh(			struct Config *cfg );

void			myGUI_BusyIPEditWindow(			struct Config *cfg, int val );
int				myGUI_OpenIPEditWindow(			struct Config *cfg );
void			myGUI_CloseIPEditWindow(		struct Config *cfg );
void			myGUI_HandleIPEditWindow(		struct Config *cfg );

void			myGUI_BusyQuitWindow(			struct Config *cfg, int val );
int				myGUI_OpenQuitWindow(			struct Config *cfg );
void			myGUI_CloseQuitWindow(			struct Config *cfg );
void			myGUI_HandleQuitWindow(			struct Config *cfg );

void			LogTime_Init(					struct LogTime *lt );
void			LogTime_Stop(					struct LogTime *lt );
void			LogTime_GetConnectTime(			struct LogTime *lt, char *buf, int len );
void			LogTime_GetDurationTime(		struct LogTime *lt, char *buf, int len );

void			myGUI_AddLogStringMessage(		struct Config *cfg, struct CommandLogString *msg );
void			myGUI_RefreshMessage(			struct Config *cfg, struct CommandRefresh *msg );
void			myGUI_RefreshStats(				struct Config *cfg, struct CommandRefresh *msg );
void			myGUI_TimerTick(				struct Config *cfg );

// --

void mySetEncoding_Message(		struct Config *cfg, struct PixelMessage *msg, int User );
void mySetEncoding_Format(		struct Config *cfg, int Format );

int TileRender_Copy(			struct Config *cfg, APTR buf, int tile );
int TileRender_Generic_8(		struct Config *cfg, APTR buf, int tile );
int TileRender_Generic_16BE(	struct Config *cfg, APTR buf, int tile );
int TileRender_Generic_16LE(	struct Config *cfg, APTR buf, int tile );
int TileRender_Generic_32BE(	struct Config *cfg, APTR buf, int tile );
int TileRender_Generic_32LE(	struct Config *cfg, APTR buf, int tile );

int NewBuffer_Cursor(			struct Config *cfg );
int	NewBufferUpdate(			struct Config *cfg );
void NewBuffer_AddCursor(		struct Config *cfg, uint8 *data, int tile );

int myEnc_Raw(					struct Config *cfg, struct UpdateNode *un, int tile, int hardcursor );
int myEnc_ZLib(					struct Config *cfg, struct UpdateNode *un, int tile, int hardcursor );

void	myInitSessionInfo(		struct Config *cfg, int a, int b, int c, int d );
void	myPrintSessionInfo(		struct Config *cfg );
void	Session_GetProcent(		char *buf, int len, uint64 total, uint64 val );
void	Session_GetBytes(		char *buf, int len, uint64 val );

int Send_Load_Pointer(			struct Config *cfg );
void Send_Free_Pointer(			struct Config *cfg );

void GUIFunc_UpdateServerStatus( struct Config *cfg );
void GUIFunc_StartServer(		struct Config *cfg );
void GUIFunc_StopServer(		struct Config *cfg );

void Func_Quit(					struct Config *cfg );
void Func_ForceQuit(			struct Config *cfg );

// --

char *	myGetTempFilename(		char *path );
int		mySameString(			char *str1, char *str2 );
int		myNetRead(				struct Config *cfg, void *buf, int len, int flag );
int		myNetSend(				struct Config *cfg, void *buf, int len );
void *	myMalloc(				int size );
void *	myCalloc(				int size );
char *	myStrdup(				char *str );
void	myFree(					void *mem );

#ifdef DO_CLIB

char *	myASPrintF( char *fmt, ... );
//void	Log_PrintF( struct Config *cfg, int type, const char *fmt, ... );
void	VARARGS68K Log_PrintF( struct Config *cfg, int type, const char *fmt, ... );

#else

char *	VARARGS68K myASPrintF( char *fmt, ... );
void	VARARGS68K Log_PrintF( struct Config *cfg, int type, const char *fmt, ... );

#endif

// --

extern const struct Cfg_Group ConfigGroups[];
extern const char *ConfigHeaderStr;
extern char ActionBuffer_UserDisconnect[MAX_ACTIONBUFFER];	// Save
extern char ActionBuffer_ProgramStart[MAX_ACTIONBUFFER];	// Save
extern char ActionBuffer_ProgramStop[MAX_ACTIONBUFFER];		// Save
extern char ActionBuffer_ServerStart[MAX_ACTIONBUFFER];		// Save
extern char ActionBuffer_UserConnect[MAX_ACTIONBUFFER];		// Save
extern char ActionBuffer_ServerStop[MAX_ACTIONBUFFER];		// Save
extern struct CommandEncoding *ActiveEncoding;
extern struct SignalSemaphore UpdateSema;
extern struct SignalSemaphore ActionSema;
extern struct SignalSemaphore UserSema;
extern struct ParseCommand ParseActionsCmds[];
extern struct ParseCommand ParseProgramCmds[];
extern struct ParseCommand ParseServerCmds[];
extern struct ParseCommand ParseBlackCmds[];
extern struct ParseCommand ParseWhiteCmds[];
extern struct ParseCommand ParseLogCmds[];
extern struct TimeRequest *TimerIOReq;
extern struct DiskObject *ProgramIcon;
extern struct MsgPort *CmdMsgPort;
extern struct MsgPort *WinMsgPort;
extern struct MsgPort *WinAppPort;
extern struct Task *ProgramTask;
extern struct List KeyLoggerList;
extern struct List LogStringList;
extern struct List UpdateList;
extern struct List UpdateFree;
extern Class *ListBrowserClass;
extern Class *ClickTabClass;
extern Class *IntegerClass;
extern Class *WindowClass;
extern Class *LayoutClass;
extern Class *ButtonClass;
extern Class *SttingClass;
extern Class *BitMapClass;
extern Class *PageClass;
extern uint32 ARexxBit;
extern uint32 AppID;
extern int ProgramRunning;
extern int ProgramInfo;
extern int DoVerbose;


// --

#endif
