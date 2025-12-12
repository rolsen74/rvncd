
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_AREXX_H__
#define __INC__RVNCD_AREXX_H__

// --

#include <proto/arexx.h>
#include <classes/arexx.h>

// --

S32		ARexx_Init(		struct Config *cfg );
void	ARexx_Free(		struct Config *cfg );
void	ARexx_Handle(	struct Config *cfg );

// --

extern U32 ARexxBit;

// --

#endif // __INC__RVNCD_AREXX_H__
