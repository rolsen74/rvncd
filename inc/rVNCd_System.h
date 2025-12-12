
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_SYSTEM_H__
#define __INC__RVNCD_SYSTEM_H__

// --

S32		System_Init(		struct Config *cfg );
void	System_Free(		struct Config *cfg );

// --

#endif // __INC__RVNCD_SYSTEM_H__
