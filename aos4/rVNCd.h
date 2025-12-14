
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#define __VNC__RVNCD_H__

// --

#define __USE_INLINE__
#define __USE_BASETYPE__

#include <proto/asl.h>
#include <proto/datatypes.h>
#include <proto/diskfont.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/icon.h>
#include <proto/input.h>
#include <proto/intuition.h>
#include <proto/keymap.h>
#include <proto/timer.h>
#include <proto/utility.h>

#include <datatypes/textclass.h>
#include <datatypes/pictureclass.h>
#include <devices/clipboard.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <graphics/gfx.h>
#include <libraries/iffparse.h>

#ifdef HAVE_CGFX
#include <proto/cybergraphics.h>
#include <cybergraphx/cybergraphics.h>
#endif // HAVE_CGFX

#ifdef HAVE_P96
#include <proto/Picasso96API.h>
#endif // HAVE_P96

#define __NOLIBBASE__
#define __NOGLOBALIFACE__
#include <proto/bsdsocket.h>
#undef  __NOGLOBALIFACE__
#undef  __NOLIBBASE__

// --

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <strings.h>
#include <signal.h>
#include <inttypes.h>
#include <time.h>
//#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/time.h>

#include <amiga_compiler.h>

// --
#if 1

#ifndef U8
typedef uint8_t U8;
#endif

#ifndef U16
typedef uint16_t U16;
#endif

#ifndef U32
typedef uint32_t U32;
#endif

#ifndef U64
typedef uint64_t U64;
#endif

#ifndef S8
typedef int8_t S8;
#endif

#ifndef S16
typedef int16_t S16;
#endif

#ifndef S32
typedef int32_t S32;
#endif

#ifndef S64
typedef int64_t S64;
#endif

#ifndef PTR
typedef void* PTR;
#endif

#ifndef STR
typedef char* STR;
#endif

#endif
// --

#define PRI_STARTUP				8		// Used for all starting processes
#define PRI_SHUTDOWN			9		// Used for all stopping processes
#define PRI_GFX					-5
#define PRI_NETSEND				1
#define PRI_NETREAD				5
#define PRI_WATCHDOG			1
#define PRI_SERVER				1
#define PRI_SHELL				0

#define MAX_KeyListNodes		250
#define MAX_LogListNodes		250
#define MAX_SERVERS				1
#define MAX_SERVERNAME			256
#define MAX_SERVERPASSWORD		8
#define MAX_USERS				1
#define MAX_ACTIONBUFFER		1024

// --

struct Config;
#include "../rVNCd_rev.h"

#include "IDs.h"
#include "d3des.h"

#include "VNC.h"
#include "VNC_Server.h"

#include "__List.h"
#include "__Memory.h"
#include "__Port.h"

#include "rVNCd_Action.h"
#include "rVNCd_Commands.h"
#include "rVNCd_Process.h"	// Before GUI
#include "rVNCd_Misc.h"		// Before LogTime.h
#include "rVNCd_GUI.h"
#include "rVNCd_Log.h"
#include "rVNCd_LogTime.h"
#include "rVNCd_Session.h"
#include "rVNCd_System.h"
#include "rVNCd_Timer.h"
#include "rVNCd_Tile_SendBuffer.h"

#ifdef HAVE_APPLIB
#include "rVNCd_Application.h"
#endif // HAVE_APPLIB

#ifdef HAVE_AREXX
#include "rVNCd_ARexx.h"
#endif // HAVE_AREXX

#ifdef HAVE_CXBROKER
#include "rVNCd_CxBroker.h"
#endif // HAVE_CXBROKER

#ifdef HAVE_ZLIB
#include "rVNCd_ZLib.h"
#endif // HAVE_ZLIB

#ifdef GUI_RA
#include "rVNCd_GUI_RA.h"
#endif // GUI_RA

#ifdef GUI_MUI
#include "rVNCd_GUI_MUI.h"
#endif // GUI_MUI

#include "rVNCd_Config.h"

// --

#define DELETEFILE				Delete
#define SETCURRENTDIR			SetCurrentDir
#define NET_EXIT_SIGNAL			SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E

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

// --




// -- IP Edit Status

enum
{
	IPES_Unused = 0,
	IPES_Add,
	IPES_Edit,
};

// -- Windows

struct ParseCommand
{
	S32		Len;
	STR		Name;
	S32	  (*Func)( struct Config *cfg, STR buf, S32 pos, S32 argpos, S32 arglen, U32 nr );
};

// --

typedef struct
{
	void ( *Function )( struct Config * );

} GUIFunc;

// --



void			Log_Setup(				struct Config *cfg );

// Misc
STR				Func_StrError(			S32 err );
void			mySetFormat(			struct Config *cfg, S32 Format );



// --

void			ZLib_Setup(			struct Config *cfg );
void			ZLib_Cleanup(			struct Config *cfg );
S32				Send_Open_Net(			struct Config *cfg );
void			Send_Close_Net(			struct Config *cfg );
S32				Send_Open_Screen(		struct Config *cfg );
void			Send_Close_Screen(		struct Config *cfg );

// --

void			RAGUI_BusyMainWindow(			struct Config *cfg, S32 val );
S32				RAGUI_OpenMainWindow(			struct Config *cfg );
void			RAGUI_CloseMainWindow(			struct Config *cfg );
void			RAGUI_HandleMainWindow(			struct Config *cfg );
//void			myGUI_Main_SessionMessage(		struct Config *cfg, struct CommandSession *msg );
void			RAGUI_TimerTick_Main(			struct Config *cfg );

void			myGUI_Main_TimerTick(			struct Config *cfg );
void			myGUI_Main_UpdateIPList(		struct Config *cfg );
void			Page_Refresh_IP(				struct Config *cfg );

void			RAGUI_BusyKeyWindow(			struct Config *cfg, S32 val );
S32				RAGUI_OpenKeyWindow(			struct Config *cfg );
void			RAGUI_CloseKeyWindow(			struct Config *cfg );
void			RAGUI_HandleKeyWindow(			struct Config *cfg );
void			myGUI_AddKeyMessage(			struct Config *cfg, struct CommandKey *msg );

void			RAGUI_BusyEncodingsWindow(		struct Config *cfg, S32 val );
S32				RAGUI_OpenEncodingsWindow(		struct Config *cfg );
void			RAGUI_CloseEncodingsWindow(		struct Config *cfg );
void			RAGUI_HandleEncodingsWindow(	struct Config *cfg );
void			myGUI_EncodingMessage(			struct Config *cfg, struct CommandEncoding *msg );

void			RAGUI_BusyAboutWindow(			struct Config *cfg, S32 val );
S32				RAGUI_OpenAboutWindow(			struct Config *cfg );
void			RAGUI_CloseAboutWindow(			struct Config *cfg );
void			RAGUI_HandleAboutWindow(		struct Config *cfg );

void			RAGUI_BusySessionWindow(		struct Config *cfg, S32 val );
S32				RAGUI_OpenSessionWindow(		struct Config *cfg );
void			RAGUI_CloseSessionWindow(		struct Config *cfg );
void			RAGUI_HandleSessionWindow(		struct Config *cfg );
//void			myGUI_SessionMessage(			struct Config *cfg, struct CommandSession *msg );
void			RAGUI_TimerTick_Session(		struct Config *cfg );

void			RAGUI_BusyPixelWindow(			struct Config *cfg, S32 val );
S32				RAGUI_OpenPixelWindow(			struct Config *cfg );
void			RAGUI_ClosePixelWindow(			struct Config *cfg );
void			RAGUI_HandlePixelWindow(		struct Config *cfg );
//void			myGUI_PxlFmtMessage(			struct Config *cfg, struct CommandPxlFmt *msg );
void			myGUI_PxlFmtRefresh(			struct Config *cfg );

void			RAGUI_BusyIPEditWindow(			struct Config *cfg, S32 val );
S32				RAGUI_OpenIPEditWindow(			struct Config *cfg );
void			RAGUI_CloseIPEditWindow(		struct Config *cfg );
void			RAGUI_HandleIPEditWindow(		struct Config *cfg );

void			RAGUI_BusyQuitWindow(			struct Config *cfg, S32 val );
S32				RAGUI_OpenQuitWindow(			struct Config *cfg );
void			RAGUI_CloseQuitWindow(			struct Config *cfg );
void			RAGUI_HandleQuitWindow(			struct Config *cfg );

//void			myGUI_RefreshMessage(			struct Config *cfg, struct CommandRefresh *msg );
//void			myGUI_RefreshStats(				struct Config *cfg, struct CommandRefresh *msg );
void			myGUI_TimerTick(				struct Config *cfg );

// --

S32 TileRender_Copy(			struct Config *cfg, PTR buf, U32 tilenr );
S32 TileRender_Generic_8(		struct Config *cfg, PTR buf, U32 tilenr );
S32 TileRender_Generic_16BE(	struct Config *cfg, PTR buf, U32 tilenr );
S32 TileRender_Generic_16LE(	struct Config *cfg, PTR buf, U32 tilenr );
S32 TileRender_Generic_32BE(	struct Config *cfg, PTR buf, U32 tilenr );
S32 TileRender_Generic_32LE(	struct Config *cfg, PTR buf, U32 tilenr );

void GUIFunc_StartServer(		struct Config *cfg );
void GUIFunc_StopServer(		struct Config *cfg );

// --

extern struct VNCList KeyLoggerList;
extern struct VNCList LogStringList;
extern struct Task *ProgramTask;
extern S32 ProgramRunning;
extern S32 DoVerboseLocked;
extern S32 DoVerbose;

// --

#endif // __VNC__RVNCD_H__
