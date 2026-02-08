
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_MISC_H__
#define __INC__RVNCD_MISC_H__

// --

// -- Window Status

#if 1

enum WinStat
{
	WINSTAT_Close,
	WINSTAT_Open,
	WINSTAT_Iconifyed,
	WINSTAT_LAST
};

#else

#define WINSTAT_Close		0
#define WINSTAT_Open		1
#define	WINSTAT_Iconifyed	2
#define	WINSTAT_LAST		3

#endif

// --

enum VNCEncoding
{
	ENCODE_Raw,			// Encoding : 0
	ENCODE_RRE,			// Encoding : 2
	ENCODE_ZLib,		// Encoding : 6
	ENCODE_RichCursor,
	ENCODE_LAST
};

struct myEncNode
{
	S16	Pos;
	S16	Type;
	S16	Enabled;
	S16	EncType;		// enum VNCEncoding
};

// --

struct TileInfo
{
	S16	X;
	S16	Y;
	S16	W;
	S16	H;
	S16	C;	// Clear
};

enum VNCPix
{
	VNCPix_Unknown = 0,
	VNCPix_R5G6B5,
	VNCPix_R5G6B5PC,
	VNCPix_A8R8G8B8,
	VNCPix_B8G8R8A8,
	VNCPix_Last
};

// -- Copy of Amiga's TimeRequest structs
// but without Name conflicts

#pragma pack(1)

struct VNCTimeVal
{
	U32	Seconds;
	U32	Microseconds;
};

struct VNCTimeRequest
{
	struct IORequest	Request;
	struct VNCTimeVal	Time;	
};

#pragma pack(0)


// --
// Shell Buffer
extern U32 ShellBuf_Enable;

S32		_ShellBuf_Init(			void );
void	_ShellBuf_Free(			void );
void	_ShellBuf_Flush(		void );
void	_ShellBuf_PrintF(		STR fmt, ... );
U32		_ShellBuf_GetBytes(		STR buf, U32 max );
void	_ShellBuf_Lock(			void );
void	_ShellBuf_Unlock(		void );
void	_ShellBuf_Insert(		STR data, S32 len );

#define	SHELLBUF_INIT()			_ShellBuf_Init()
#define	SHELLBUF_FREE()			_ShellBuf_Free()
#define	SHELLBUF_FLUSH()		_ShellBuf_Flush()
#define	SHELLBUF_GETBYTES(x,y)	_ShellBuf_GetBytes(x,y)
#define	SHELLBUF_LOCK()			_ShellBuf_Lock()
#define	SHELLBUF_UNLOCK()		_ShellBuf_Unlock()
#define SHELLBUF_INSERT(x,y)	_ShellBuf_Insert(x,y)

#define	SHELLBUF_PRINTF(fmt)				_ShellBuf_PrintF(fmt)
#define	SHELLBUF_PRINTF1(fmt,x)				_ShellBuf_PrintF(fmt,x)
#define	SHELLBUF_PRINTF2(fmt,x,y)			_ShellBuf_PrintF(fmt,x,y)
#define	SHELLBUF_PRINTF3(fmt,x,y,z)			_ShellBuf_PrintF(fmt,x,y,z)
#define	SHELLBUF_PRINTF4(fmt,x,y,z,aa)		_ShellBuf_PrintF(fmt,x,y,z,aa)
#define	SHELLBUF_PRINTF5(fmt,x,y,z,aa,bb)	_ShellBuf_PrintF(fmt,x,y,z,aa,bb)
//#define	SHELLBUF_PRINTF(fmt,...)		_ShellBuf_PrintF(fmt,##__VA_ARGS__)

// --

// Shell Buffer Process
S32		myStart_ShellBuf(			struct Config *cfg );
void	myStop_ShellBuf(			struct Config *cfg );
// --

S32		StartServer(				struct Config *cfg );
S32		StopServer(					struct Config *cfg );
S32		RestartServer(				struct Config *cfg );

void	PrintProgramHeader(			void );

void	Func_Blanker_Enable(		struct Config *cfg );
void	Func_Blanker_Disable(		struct Config *cfg );
void 	Func_Quit(					struct Config *cfg );
void 	Func_ForceQuit(				struct Config *cfg );
S32		Func_GetFileSize(			STR name, S32 *size );
STR		Func_GetTempFilename(		STR path );
S32		Func_SameString(			STR str1, STR str2 );
S32		Func_Strnicmp(				const STR s1, const STR s2, size_t n );
STR		Func_StrError(				S32 err );
S32		Func_NetRead(				struct Config *cfg, PTR buf, S32 len, S32 flag );
S32		Func_NetSend(				struct Config *cfg, PTR buf, S32 len );

S32		Send_Open_Socket(			struct Config *cfg );
void	Send_Close_Socket(			struct Config *cfg );
S32		Send_Install_ClipHook(		struct Config *cfg );
void	Send_Remove_ClipHook(		struct Config *cfg );
void	Send_Handle_ClipBoard(		struct Config *cfg );
S32		Send_Load_Pointer(			struct Config *cfg );
void	Send_Free_Pointer(			struct Config *cfg );

S32		NewBuffer_Cursor_Rich(			struct Config *cfg );
void	NewBuffer_Cursor_Soft(			struct Config *cfg, U8 *data, U32 tilenr );
S32		NewBuffer_Cursor_Update(		struct Config *cfg );

S32		NewBuffer_LastRect(			struct Config *cfg );


enum UpdateStat NewUpdateReq_Msg(				struct Config *cfg, struct UpdateNode *un );
U32				NewBuffer_Update_Tile_Count(	struct Config *cfg, struct UpdateNode *un );
enum UpdateStat NewBuffer_Update_Tile_Check(	struct Config *cfg, struct UpdateNode *un, U32 tilenr, S32 hardcursor, U32 *tilesend );
S32				NewBuffer_Update_Tile_Fill(		struct Config *cfg, struct UpdateNode *un, U32 tilenr, S32 hardcursor );

enum UpdateStat	NewBeep_Msg(			struct Config *cfg, struct UpdateNode *un );
enum UpdateStat NewEncoding_Msg(		struct Config *cfg, struct UpdateNode *un );
enum UpdateStat NewPixelFmt_Msg(		struct Config *cfg, struct UpdateNode *un );

void	mySetEncoding_Message(		struct Config *cfg, struct PixelMessage *msg, S32 User );
void	mySetEncoding_Format(		struct Config *cfg, enum VNCPix Format );

S32		myEnc_Raw(					struct Config *cfg, struct UpdateNode *un, U32 tilenr );
S32		myEnc_Raw_8(				struct Config *cfg, struct UpdateNode *un, U32 tilenr );
S32		myEnc_Raw_16(				struct Config *cfg, struct UpdateNode *un, U32 tilenr );
S32		myEnc_Raw_32(				struct Config *cfg, struct UpdateNode *un, U32 tilenr );

S32		myEnc_RRE(					struct Config *cfg, struct UpdateNode *un, U32 tilenr );
S32		myEnc_RRE_8(				struct Config *cfg, struct UpdateNode *un, U32 tilenr );
S32		myEnc_RRE_16(				struct Config *cfg, struct UpdateNode *un, U32 tilenr );
S32		myEnc_RRE_32(				struct Config *cfg, struct UpdateNode *un, U32 tilenr );

#ifdef HAVE_ZLIB
S32		myEnc_ZLib(					struct Config *cfg, struct UpdateNode *un, U32 tilenr );
S32		myEnc_ZLib_8(				struct Config *cfg, struct UpdateNode *un, U32 tilenr );
S32		myEnc_ZLib_16(				struct Config *cfg, struct UpdateNode *un, U32 tilenr );
S32		myEnc_ZLib_32(				struct Config *cfg, struct UpdateNode *un, U32 tilenr );
#endif // HAVE_ZLIB


void Beep_Patch_Install( struct Config *cfg );
void Beep_Patch_Remove( struct Config *cfg );

U32 myMarkDirtyXY( struct Config *cfg, S32 x, S32 y );

// --

S32		System_Init( struct Config *cfg );
void	System_Free( void );
S32		GFX_Init( struct Config *cfg );
void	GFX_Free( struct Config *cfg );

// --

#endif // __INC__RVNCD_MISC_H__
