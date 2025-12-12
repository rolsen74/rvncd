
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

void Send_Blanker_Enable( struct Config *cfg )
{
	Log_PrintF( cfg, LOGTYPE_Info, "Enabling Blanker" );

	IApplication->SetApplicationAttrs( AppID,
		APPATTR_AllowsBlanker, TRUE,
		TAG_END
	);
}

// --

void Send_Blanker_Disable( struct Config *cfg )
{
	Log_PrintF( cfg, LOGTYPE_Info, "Disabling Blanker" );

	IApplication->SendApplicationMsg( AppID, 0, NULL, APPLIBMT_BlankerUnBlank );

	IApplication->SetApplicationAttrs( AppID,
		APPATTR_AllowsBlanker, FALSE,
		TAG_END
	);
}

// --

