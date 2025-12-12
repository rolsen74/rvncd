
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

S32 VNC_Version( struct Config *cfg )
{
STR buf;
S32 rejected;
S32 ver;
S32 rev;
S32 rc;

	buf = (STR) cfg->NetSend_SendBuffer;

	rejected = TRUE;

	if ( DoVerbose > 0 )
	{
		Log_PrintF( cfg, LOGTYPE_Info, "Negociating protocol version" );
	}

	// --- Version Exchange

	/**/ if ( cfg->cfg_Disk_Settings.Protocol37 )
	{
		ver = 3;
		rev = 7;
	}
	else // ( cfg->cfg_Disk_Settings.Protocol33 )
	{
		ver = 3;
		rev = 3;
	}

	// "RFB 003.003\n" - 12 bytes exactly
	sprintf( buf, "RFB %03d.%03d\n", ver, rev );

	if ( DoVerbose > 0 )
	{
		Log_PrintF( cfg, LOGTYPE_Info, ". Maximum version supported by server v%ld.%ld", ver, rev );
	}

	rc = Func_NetSend( cfg, buf, 12 );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	rc = Func_NetRead( cfg, buf, 12, MSG_WAITALL );

	if ( rc <= 0 )
	{
		goto bailout;
	}

	buf[12] = 0;

	ver = 0;
	rev = 0;

	sscanf( buf, "RFB %d.%d\n", &ver, &rev );

	if ( DoVerbose > 0 )
	{
		Log_PrintF( cfg, LOGTYPE_Info, ". Client requested version v%ld.%ld", ver, rev );
	}

	/**/ if (( ver == 3 ) && ( rev == 3 ))
	{
		if ( ! cfg->cfg_Disk_Settings.Protocol33 )
		{
			Log_PrintF( cfg, LOGTYPE_Warning, "Protocol v3.3 not enabled" );
			goto bailout;
		}

		if ( DoVerbose > 0 )
		{
			Log_PrintF( cfg, LOGTYPE_Info, ". Protocol version v%ld.%ld selected", ver, rev );
		}

		cfg->NetSend_ClientProtocol = VNCProtocol_33;
	}
	else if (( ver == 3 ) && ( rev == 7 ))
	{
		if ( ! cfg->cfg_Disk_Settings.Protocol37 )
		{
			Log_PrintF( cfg, LOGTYPE_Warning, "Protocol v3.7 not enabled" );
			goto bailout;
		}

		if ( DoVerbose > 0 )
		{
			Log_PrintF( cfg, LOGTYPE_Info, ". Protocol version v%ld.%ld selected", ver, rev );
		}

		cfg->NetSend_ClientProtocol = VNCProtocol_37;
	}
	else
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Unsupported version v%ld.%ld", ver, rev );
		goto bailout;
	}

	rejected = FALSE;

bailout:

	return( rejected );
}

// --
