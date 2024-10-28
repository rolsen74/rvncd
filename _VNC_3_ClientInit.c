
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

#if 0

struct ClientInitMessage
{
	uint8	cim_Shared;
};

#endif

// --

int VNC_ClientInit( struct Config *cfg )
{
struct ClientInitMessage *cim;
int rejected;
int rc;

	rejected = TRUE;

	cim = cfg->NetSend_SendBuffer;

	// -- Get Shared Flag
	// We dont use this

	rc = myNetRead( cfg, cim, sizeof( struct ClientInitMessage ), MSG_WAITALL );

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
