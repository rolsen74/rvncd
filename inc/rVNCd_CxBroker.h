
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_CXBROKER_H__
#define __INC__RVNCD_CXBROKER_H__

// --

#include <proto/commodities.h>
#include <libraries/commodities.h>

// --

S32		CxBroker_Init(		struct Config *cfg );
void	CxBroker_Free(		struct Config *cfg );
void	CxBroker_Handle(	struct Config *cfg );

// --

extern struct VNCMsgPort CxBrokerMsgPort;

// --

#endif // __INC__RVNCD_CXBROKER_H__
