
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

#ifndef __VNC_H__
#define __VNC_H__

// --

#pragma pack(1)

// -- Server to Client - Messages --

struct ClientInitMessage
{
	uint8	cim_Shared;
};

struct ServerInitMessage
{
	uint16	sim_Width;
	uint16	sim_Height;
	uint8	sim_BitsPerPixel;
	uint8	sim_Depth;
	uint8	sim_BigEndian;
	uint8	sim_TrueColor;
	uint16	sim_RedMax;
	uint16	sim_GreenMax;
	uint16	sim_BlueMax;
	uint8	sim_RedShift;
	uint8	sim_GreenShift;
	uint8	sim_BlueShift;
	uint8	sim_Pad;
	uint8	sim_Pad2;
	uint8	sim_Pad3;
	uint32	sim_NameLength;
	// Name String
};

// Gfx Rect Header
struct GfxRectHeader
{
	uint8	bm_Type;
	uint8	bm_Pad;
	uint16	bm_Rects;
	// Rects
};

// Gfx Rect Buffer
struct GfxRectBuffer
{
	uint16	br_XPos;
	uint16	br_YPos;
	uint16	br_Width;
	uint16	br_Height;
	uint32	br_Encoding;
};

// Gfx Rect : ZLib Encoding
struct GfxRectZLib
{
	// -- Rect Header
	uint16	br_XPos;
	uint16	br_YPos;
	uint16	br_Width;
	uint16	br_Height;
	uint32	br_Encoding;
	// -- ZLib Header
	uint32	bz_Length;
	// -- ZLib Data
};

// -- Client to Server - Messages --

struct PixelMessage
{
	uint8	pm_Type;			// Type 0 : Set Pixel Format
	uint8	pm_Pad;
	uint8	pm_Pad2;
	uint8	pm_Pad3;
	uint8	pm_BitsPerPixel;
	uint8	pm_Depth;
	uint8	pm_BigEndian;
	uint8	pm_TrueColor;
	uint16	pm_RedMax;
	uint16	pm_GreenMax;
	uint16	pm_BlueMax;
	uint8	pm_RedShift;
	uint8	pm_GreenShift;
	uint8	pm_BlueShift;
	uint8	pm_Pad4;
	uint8	pm_Pad5;
	uint8	pm_Pad6;
};

struct EncodingMessage
{
	uint8	em_Type;			// Type 2 : Set Encoding
	uint8	em_Pad;
	uint16	em_Encodings;
};

struct UpdateRequestMessage
{
	uint8	urm_Type;			// Type 3 : Update Request
	uint8	urm_Incremental;
	uint16	urm_XPos;
	uint16	urm_YPos;
	uint16	urm_Width;
	uint16	urm_Height;
};

struct KeyMessage
{
	uint8	km_Type;			// Type 4 : Key
	uint8	km_Down;
	uint8	km_Pad;
	uint8	km_Pad2;
	uint32	km_Key;
};

struct MouseMessage
{
	uint8	mm_Type;			// Type 5 : Mouse
	uint8	mm_Buttons;
	int16	mm_X;
	int16	mm_Y;
};

struct ClipboardMessage
{
	uint8	cm_Type;			// Type 6 : Clipboard
	uint8	cm_Pad;
	uint8	cm_Pad2;
	uint8	cm_Pad3;
	uint32	cm_Length;
	// Text String
};

#pragma pack(0)

// --

#endif
