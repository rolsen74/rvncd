
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#define __VNC__RVNCD_H__

// --

#include <proto/datatypes.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/graphics.h>
#include <proto/input.h>
#include <proto/intuition.h>
#include <proto/keymap.h>
#include <proto/timer.h>

#include <clib/alib_protos.h>
#include <datatypes/textclass.h>
#include <datatypes/pictureclass.h>
#include <devices/clipboard.h>
#include <devices/input.h>
#include <devices/inputevent.h>
#include <dos/dostags.h>

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
//#include <strings.h>
#include <signal.h>
#include <inttypes.h>
#include <time.h>
//#include <unistd.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <sys/select.h>

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
//#include <proto/z.h>
//#include "rVNCd_ZLib.h"
#endif // HAVE_ZLIB

#ifdef GUI_RA
#include "rVNCd_GUI_RA.h"
#endif // GUI_RA

#ifdef GUI_MUI
#include "rVNCd_GUI_MUI.h"
#endif // GUI_MUI

// --

#define MAX_KeyListNodes		250
#define MAX_LogListNodes		250
#define MAX_SERVERS				1
#define MAX_SERVERNAME			256
#define MAX_SERVERPASSWORD		8
#define MAX_USERS				1
#define MAX_ACTIONBUFFER		1024

#include "rVNCd_Config.h"

// --

#define DELETEFILE				DeleteFile
#define SETCURRENTDIR			CurrentDir
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

#ifndef USED
#define USED
#endif

#ifndef UNUSED
#define UNUSED
#endif

#ifndef EIDRM
#define EIDRM 82
#endif

#ifndef ENOMSG
#define ENOMSG 83
#endif

#ifndef EOVERFLOW
#define EOVERFLOW 84
#endif

#ifndef EILSEQ
#define EILSEQ 85
#endif

#ifndef ENOTSUP
#define ENOTSUP 86
#endif

// --

extern struct VNCList KeyLoggerList;
extern struct VNCList LogStringList;
extern struct Task *ProgramTask;
extern S32 ProgramRunning;
extern S32 DoVerboseLocked;
extern S32 DoVerbose;

// --

#endif // __VNC__RVNCD_H__
