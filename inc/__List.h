
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__LIST_H__
#define __INC__LIST_H__

// --

struct VNCList
{
	struct List		vl_List;
	U32			vl_StructID;
	S32			vl_Entries;
};

// --

S32		List_Init(		struct VNCList *l );
void	List_Free(		struct VNCList *l );
PTR 	List_GetHead(	struct VNCList *l );
PTR 	List_GetNext(	PTR n );
PTR 	List_GetPrev(	PTR n );
void	List_AddTail(	struct VNCList *l, PTR n );
PTR 	List_RemHead(	struct VNCList *l );
void	List_Insert(	struct VNCList *l, PTR n1, PTR n2 );

// --

#endif // __INC__LIST_H__
