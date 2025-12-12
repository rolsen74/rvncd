
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_COMMANDS_H__
#define __INC__RVNCD_COMMANDS_H__

// --

enum Commands
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
	enum Commands		cm_Command;
};

struct CommandKey
{
	struct Message		ck_Message;
	enum Commands		ck_Command;
	// --
	char				ck_Client[64];
	char				ck_Amiga[64];
};

struct CommandPxlFmt
{
	struct Message		cpf_Message;
	enum Commands		cpf_Command;
	// --
	S32					cpf_Client;
	struct PixelMessage	cpf_Format;
};			

struct CommandSession
{
	struct Message		cs_Message;
	enum Commands		cs_Command;
	// --
	U32					cs_Starting;
};

struct CommandEncoding
{
	struct Message		ce_Message;
	enum Commands		ce_Command;
	// --
	S32					ce_Encodings;
	U32					ce_Data[1];
};

struct CommandRefresh
{
	struct Message		cr_Message;
	enum Commands		cr_Command;
	// --
};

struct CommandLogString
{
	struct Message		cl_Message;
	enum Commands		cl_Command;
	// --
	U8					cl_Type;
	char				cl_Time[12];
	char				cl_String[1];
};

// --

S32		Cmd_Init(		struct Config *cfg );
void	Cmd_Free(		void );
void	Cmd_Handle(		struct Config *cfg );

// --

extern struct VNCMsgPort CmdMsgPort;

// --

#endif // __INC__RVNCD_COMMANDS_H__
