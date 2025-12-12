
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_SESSION_H__
#define __INC__RVNCD_SESSION_H__

// --

struct PxlInfo
{
	S32		Valid;
	U64		Normal;
};

#define PIXEL_Entries	16

struct SessionInfo
{
	// -- Pixel
	U64					si_Pixels_Send;		// Pixels
	U64					si_Pixels_Last;		// Pixels
	struct PxlInfo		si_Pixel_Data[PIXEL_Entries];
	struct VNCTimeVal	si_Pixel_Time_Wait;
	struct VNCTimeVal	si_Pixel_Time_Last;
	struct VNCTimeVal	si_Pixel_Time_Cur;
	S32					si_Pixels_Delay;
	S32					si_Pixels_Freq;
	U32					si_Pixel_Pos;
	U64					si_Pixel_Min;
	U64					si_Pixel_Avr;
	U64					si_Pixel_Max;
	U32					si_Load;			// 0-1000 -> 100.0%.. cpu load for NetSend process

	// -- Bytes Send/Read in total
	U64					si_Read_Bytes;
	U64					si_Send_Bytes;

	// -- Tiles Send
	U64					si_Tiles_Raw;
	U64					si_Tiles_RRE;
	U64					si_Tiles_ZLib;
	U64					si_Tiles_Rich;
	U64					si_Tiles_Total;
	U64					si_Tiles_Last;		// total Frames

	// -- Time
	struct LogTime		si_LogTime;

	// -- IP Addr
	S32					si_IPAddr[4];	
};

// --

void	myInitSessionInfo(		struct Config *cfg, S32 a, S32 b, S32 c, S32 d );
void	myPrintSessionInfo(		struct Config *cfg );
void	Session_GetProcent(		STR buf, S32 len, U64 total, U64 val );
void	Session_GetBytes(		STR buf, S32 len, U64 val );

// --

#endif // __INC__RVNCD_SESSION_H__
