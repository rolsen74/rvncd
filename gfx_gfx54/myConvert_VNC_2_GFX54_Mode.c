
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if 0

enum VNCPix
{
	VNCPix_Unknown,
	VNCPix_R5G6B5,
	VNCPix_R5G6B5PC,
	VNCPix_A8R8G8B8,
	VNCPix_B8G8R8A8,
	VNCPix_Last
};

#endif

// --

enum enPixelFormat myConvert_VNC_2_GFX54_Mode( enum VNCPix vnc_mode )
{
enum enPixelFormat gfx_mode;

	switch( vnc_mode )
	{
		case VNCPix_R5G6B5:
		{
			gfx_mode = PIXF_R5G6B5;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_R5G6B5 (%" PRId32 ") modeid -> PIXF_R5G6B5 (%" PRId32 ")\n", vnc_mode, gfx_mode );
			}
			break;
		}
	
		case VNCPix_R5G6B5PC:
		{
			gfx_mode = PIXF_R5G6B5PC;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_R5G6B5PC (%" PRId32 ") modeid -> PIXF_R5G6B5PC (%" PRId32 ")\n", vnc_mode, gfx_mode );
			}
			break;
		}
	
		case VNCPix_A8R8G8B8:
		{
			gfx_mode = PIXF_A8R8G8B8;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_A8R8G8B8 (%" PRId32 ") modeid -> PIXF_A8R8G8B8 (%" PRId32 ")\n", vnc_mode, gfx_mode );
			}
			break;
		}

		case VNCPix_B8G8R8A8:
		{
			gfx_mode = PIXF_B8G8R8A8;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_B8G8R8A8 (%" PRId32 ") modeid -> PIXF_B8G8R8A8 (%" PRId32 ")\n", vnc_mode, gfx_mode );
			}
			break;
		}

		default:
		{
			gfx_mode = -1;

			if ( DoVerbose > 0 )
			{
				SHELLBUF_PRINTF2( "Unsupported VNC Format ?? (%" PRIu32 ") mode (%" PRId32 ")\n", vnc_mode, gfx_mode );
			}
			break;
		}
	}

	return( gfx_mode );
}

