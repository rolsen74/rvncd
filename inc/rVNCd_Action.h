
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

#ifndef __VNC__RVNCD_H__
#error Include 'rVNCd.h' first
#endif

#ifndef __INC__RVNCD_ACTION_H__
#define __INC__RVNCD_ACTION_H__

// --

void			DoAction_ProgramStart(	struct Config *cfg );
void			DoAction_ProgramStop(	struct Config *cfg );
void			DoAction_ServerStart(	struct Config *cfg );
void			DoAction_ServerStop(	struct Config *cfg );
void			DoAction_UserConnect(	struct Config *cfg );
void			DoAction_UserDisconnect( struct Config *cfg );

// --

extern struct SignalSemaphore ActionSema;
extern char ActionBuffer_UserDisconnect[MAX_ACTIONBUFFER];	// Save
extern char ActionBuffer_ProgramStart[MAX_ACTIONBUFFER];	// Save
extern char ActionBuffer_ProgramStop[MAX_ACTIONBUFFER];		// Save
extern char ActionBuffer_ServerStart[MAX_ACTIONBUFFER];		// Save
extern char ActionBuffer_UserConnect[MAX_ACTIONBUFFER];		// Save
extern char ActionBuffer_ServerStop[MAX_ACTIONBUFFER];		// Save

// --

#endif // __INC__RVNCD_ACTION_H__
