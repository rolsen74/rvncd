 
/*
 * Copyright (c) 2023-2024 Rene W. Olsen < renewolsen @ gmail . com >
 *
 * This software is released under the GNU General Public License, version 3.
 * For the full text of the license, please visit:
 * https://www.gnu.org/licenses/gpl-3.0.html
 *
 * You can also find a copy of the license in the LICENSE file included with this software.
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

