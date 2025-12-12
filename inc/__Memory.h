
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__MEMORY_H__
#define __INC__MEMORY_H__

// --

PTR 		mem_Alloc(				S32 size );
PTR 		mem_AllocClr(			S32 size );
STR			mem_ASPrintF(			STR fmt, ... );
STR			mem_Strdup(				STR str );
void		mem_Free(				PTR mem );

// --

#endif // __INC__MEMORY_H__
