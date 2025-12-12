
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__MPORT_H__
#define __INC__MPORT_H__

// --

struct VNCMsgPort
{
	struct MsgPort				vmp_MsgPort;
	U16							vmp_Pad;
	U32							vmp_StructID;
	U32							vmp_SignalBit;
};

// --

S32		MsgPort_Init(		struct VNCMsgPort *mp );
void	MsgPort_Free(		struct VNCMsgPort *mp );
PTR 	MsgPort_GetMsg( 	struct VNCMsgPort *mp );
void	MsgPort_PutMsg(		struct VNCMsgPort *mp, PTR msg );
void	MsgPort_WaitPort(	struct VNCMsgPort *mp );

// --

#endif // __INC__MPORT_H__
