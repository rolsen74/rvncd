
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if 0

struct BellMessage
{
	U8	bm_Type;			// Type 2 : Bell
};

#endif

// --

enum UpdateStat NewBeep_Msg( struct Config *cfg, struct UpdateNode *un UNUSED )
{
struct BellMessage *header;
enum UpdateStat stat;
S32 rc;

	stat = US_Error;

	if ( cfg->cfg_Active_Settings.SendBell )
	{
		header = (PTR) cfg->NetSend_SendBuffer;
		header->bm_Type		= 2; // Bell

		rc = Func_NetSend( cfg, header, sizeof( struct BellMessage ));

		if ( rc <= 0 )
		{
			goto bailout;
		}
	}

	stat = US_Okay;

bailout:

	return( stat );
}

// --
