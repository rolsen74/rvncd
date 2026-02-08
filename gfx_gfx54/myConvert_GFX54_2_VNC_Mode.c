
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

enum VNCPix myConvert_GFX54_2_VNC_Mode( enum enPixelFormat gfx_mode )
{
enum VNCPix vnc_mode;

	switch( gfx_mode )
	{
		case PIXF_R5G6B5PC: // 5
		{
			vnc_mode = VNCPix_R5G6B5PC;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "GFX54 PIXF_R5G6B5PC (%" PRId32 ") modeid -> VNCPix_R5G6B5PC (%" PRId32 ")\n", gfx_mode, vnc_mode );
			}
			break;
		}

		case PIXF_R5G6B5: // 10
		{
			vnc_mode = VNCPix_R5G6B5;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "GFX54 PIXF_R5G6B5 (%" PRId32 ") modeid -> VNCPix_R5G6B5 (%" PRId32 ")\n", gfx_mode, vnc_mode );
			}
			break;
		}

		case PIXF_A8R8G8B8: // 11
		{
			vnc_mode = VNCPix_A8R8G8B8;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "GFX54 PIXF_A8R8G8B8 (%" PRId32 ") modeid -> VNCPix_A8R8G8B8 (%" PRId32 ")\n", gfx_mode, vnc_mode );
			}
			break;
		}

		case PIXF_B8G8R8A8: // 12
		{
			vnc_mode = VNCPix_B8G8R8A8;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "GFX54 PIXF_B8G8R8A8 (%" PRId32 ") modeid -> VNCPix_B8G8R8A8 (%" PRId32 ")\n", gfx_mode, vnc_mode );
			}
			break;
		}

		default:
		{
			vnc_mode = VNCPix_Unknown;

			if ( DoVerbose > 0 )
			{
				SHELLBUF_PRINTF2( "Unsupported GFX54 Format ?? (%" PRIu32 ") mode (%" PRId32 ")\n", gfx_mode, vnc_mode );
			}
			break;
		}
	}

	return( vnc_mode );
}

// --
