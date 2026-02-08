
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

U32 myConvert_VNC_2_CGFX_Mode( enum VNCPix vnc_mode )
{
U32 cgfx_mode;

	switch( vnc_mode )
	{
		case VNCPix_R5G6B5:
		{
			cgfx_mode = PIXFMT_RGB16;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_R5G6B5 (%" PRId32 ") modeid -> PIXFMT_RGB16 (%" PRId32 ")\n", vnc_mode, cgfx_mode );
			}
			break;
		}

		case VNCPix_R5G6B5PC:
		{
			cgfx_mode = PIXFMT_RGB16PC;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_R5G6B5PC (%" PRId32 ") modeid -> PIXFMT_RGB16PC (%" PRId32 ")\n", vnc_mode, cgfx_mode );
			}
			break;
		}
	
		case VNCPix_A8R8G8B8:
		{
			cgfx_mode = PIXFMT_ARGB32;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_A8R8G8B8 (%" PRId32 ") modeid -> PIXFMT_ARGB32 (%" PRId32 ")\n", vnc_mode, cgfx_mode );
			}
			break;
		}

		case VNCPix_B8G8R8A8:
		{
			cgfx_mode = PIXFMT_BGRA32;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_B8G8R8A8 (%" PRId32 ") modeid -> PIXFMT_BGRA32 (%" PRId32 ")\n", vnc_mode, cgfx_mode );
			}
			break;
		}

		default:
		{
			cgfx_mode = -1;

			if ( DoVerbose > 0 )
			{
				SHELLBUF_PRINTF2( "Unsupported VNC Format ?? (%" PRIu32 ") mode (%" PRId32 ")\n", vnc_mode, cgfx_mode );
			}
			break;
		}
	}

	return( cgfx_mode );
}
