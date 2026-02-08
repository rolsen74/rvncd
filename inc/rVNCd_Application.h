
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_APPLICATION_H__
#define __INC__RVNCD_APPLICATION_H__

#ifdef HAVE_APPLIB

// --

#include <proto/application.h>

// --

S32		App_Init(		struct Config *cfg, struct WBStartup *wbmsg );
void	App_Free(		void );
void	App_Handle(		struct Config *cfg );

// --

extern U32 AppBit;
extern U32 AppID;

// --

#endif // HAVE_APPLIB
#endif // __INC__RVNCD_APPLICATION_H__
