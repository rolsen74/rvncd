
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if 0

struct ClientInitMessage
{
	U8	cim_Shared;
};

#endif

// --

S32 VNC_ClientInit( struct Config *cfg )
{
struct ClientInitMessage *cim;
S32 rejected;
S32 rc;

	rejected = TRUE;

	cim = (PTR) cfg->NetSend_SendBuffer;

	// -- Get Shared Flag
	// We dont use this

	rc = Func_NetRead( cfg, cim, sizeof( struct ClientInitMessage ), MSG_WAITALL );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	// --

	rejected = FALSE;

bailout:

	return( rejected );
}

// --
