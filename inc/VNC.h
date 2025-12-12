
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__VNC_H__
#define __INC__VNC_H__

// --
// -- Tile Scanning for Updates in a .. method

enum
{
	SCANMethod_Linear,
	SCANMethod_Random,
	SCANMethod_Last
};

// --

enum
{
	VIEWMODE_View,
	VIEWMODE_Page,
};

// -- VNC Protocol supported

enum
{
	VNCProtocol_Unknown,
	VNCProtocol_33,				// Ver 3.3
	VNCProtocol_37,				// Ver 3.7
};

// --

enum IPType
{
	IPT_Unknown,
	IPT_Allow,
	IPT_Block
};

struct IPNode
{
	struct Node		ipn_Node;
	enum IPType		ipn_Type;
	S16				ipn_A;
	S16				ipn_B;
	S16				ipn_C;
	S16				ipn_D;
};

// --

#pragma pack(1)

// -- Server to Client - Messages --

struct ClientInitMessage
{
	U8	cim_Shared;
};

struct ServerInitMessage
{
	U16	sim_Width;
	U16	sim_Height;
	U8	sim_BitsPerPixel;
	U8	sim_Depth;
	U8	sim_BigEndian;
	U8	sim_TrueColor;
	U16	sim_RedMax;
	U16	sim_GreenMax;
	U16	sim_BlueMax;
	U8	sim_RedShift;
	U8	sim_GreenShift;
	U8	sim_BlueShift;
	U8	sim_Pad;
	U8	sim_Pad2;
	U8	sim_Pad3;
	U32	sim_NameLength;
	// Name String
};

// Gfx Rect Header
struct GfxRectHeader
{
	U8	bm_Type;
	U8	bm_Pad;
	U16	bm_Rects;
	// Rects
};

// -- Client to Server - Messages --

struct PixelMessage
{
	U8	pm_Type;			// Type 0 : Set Pixel Format
	U8	pm_Pad;
	U8	pm_Pad2;
	U8	pm_Pad3;
	U8	pm_BitsPerPixel;
	U8	pm_Depth;
	U8	pm_BigEndian;
	U8	pm_TrueColor;
	U16	pm_RedMax;
	U16	pm_GreenMax;
	U16	pm_BlueMax;
	U8	pm_RedShift;
	U8	pm_GreenShift;
	U8	pm_BlueShift;
	U8	pm_Pad4;
	U8	pm_Pad5;
	U8	pm_Pad6;
};

struct EncodingMessage
{
	U8	em_Type;			// Type 2 : Set Encoding
	U8	em_Pad;
	U16	em_Encodings;
};

struct UpdateRequestMessage
{
	U8	urm_Type;			// Type 3 : Update Request
	U8	urm_Incremental;
	U16	urm_XPos;
	U16	urm_YPos;
	U16	urm_Width;
	U16	urm_Height;
};

struct KeyMessage
{
	U8	km_Type;			// Type 4 : Key
	U8	km_Down;
	U8	km_Pad;
	U8	km_Pad2;
	U32	km_Key;
};

struct MouseMessage
{
	U8	mm_Type;			// Type 5 : Mouse
	U8	mm_Buttons;
	S16	mm_X;
	S16	mm_Y;
};

struct ClipboardMessage
{
	U8	cm_Type;			// Type 6 : Clipboard
	U8	cm_Pad;
	U8	cm_Pad2;
	U8	cm_Pad3;
	U32	cm_Length;
	// Text String
};

struct BellMessage
{
	U8	bm_Type;			// Type 2 : Bell
};

#pragma pack(0)

// --

enum UpdateType
{
	UT_Unknown,
	UT_UpdateRequest,
	UT_PixelMessage,
	UT_EncodingMsg,
	UT_Beep,
	UT_Last
};

enum UpdateStat
{
	US_Unknown,
	US_NoChange,	// Do a Wait
	US_Handled,
	US_Error,
};

#define US_Okay		US_Handled
#define US_Wait		US_NoChange

struct UpdateNode
{
	struct Node		un_Node;
	enum UpdateType	un_Type;
	PTR				un_Buffer;

	union
	{
		struct UpdateRequestMessage	update;
		struct EncodingMessage encoding;
		struct PixelMessage pixel;

	} un_Data;
};

// --

S32				TileRender_Copy(			struct Config *cfg, PTR buf, U32 tilenr );
S32				TileRender_Generic_8(		struct Config *cfg, PTR buf, U32 tilenr );
S32				TileRender_Generic_16BE(	struct Config *cfg, PTR buf, U32 tilenr );
S32				TileRender_Generic_16LE(	struct Config *cfg, PTR buf, U32 tilenr );
S32				TileRender_Generic_32BE(	struct Config *cfg, PTR buf, U32 tilenr );
S32				TileRender_Generic_32LE(	struct Config *cfg, PTR buf, U32 tilenr );

S32				VNC_Authenticate_33(		struct Config *cfg );
S32				VNC_Authenticate_37(		struct Config *cfg );
//S32				VNC_BufferUpdate(			struct Config *cfg, S32 xx, S32 yy, S32 ww, S32 hh );
S32				VNC_ClientInit(				struct Config *cfg );
S32				VNC_Clipboard(				struct Config *cfg );
S32				VNC_Keyboard(				struct Config *cfg );
S32				VNC_Mouse(					struct Config *cfg );
S32				VNC_ServerInit(				struct Config *cfg );
S32				VNC_SetEncoding(			struct Config *cfg );
S32				VNC_SetPixelFormat(			struct Config *cfg );
//S32				VNC_TileUpdate(				struct Config *cfg, U32 tilenr );
S32				VNC_UpdateRequest(			struct Config *cfg );
S32				VNC_Version(				struct Config *cfg );

S32				VNC_HandleCmds_33(			struct Config *cfg );
S32				VNC_HandleCmds_37(			struct Config *cfg );

S32				Func_NetRead(				struct Config *cfg, PTR buf, S32 len, S32 flag );
S32				Func_NetSend(				struct Config *cfg, PTR buf, S32 len );

// --

extern struct CommandEncoding *ActiveEncoding;
extern struct GUIFunctions ActiveGUI;

// --

#endif // __VNC__VNC_H__
