
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

RGBFTYPE myConvert_VNC_2_P96_Mode( enum VNCPix vnc_mode )
{
RGBFTYPE p96_mode;

	switch( vnc_mode )
	{
		case VNCPix_R5G6B5:
		{
			p96_mode = RGBFB_R5G6B5;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_R5G6B5 (%" PRId32 ") modeid -> RGBFB_R5G6B5 (%" PRId32 ")\n", vnc_mode, p96_mode );
			}
			break;
		}

		case VNCPix_R5G6B5PC:
		{
			p96_mode = RGBFB_R5G6B5PC;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_R5G6B5PC (%" PRId32 ") modeid -> RGBFB_R5G6B5PC (%" PRId32 ")\n", vnc_mode, p96_mode );
			}
			break;
		}

		case VNCPix_A8R8G8B8:
		{
			p96_mode = RGBFB_A8R8G8B8;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_A8R8G8B8 (%" PRId32 ") modeid -> RGBFB_A8R8G8B8 (%" PRId32 ")\n", vnc_mode, p96_mode );
			}
			break;
		}

		case VNCPix_B8G8R8A8:
		{
			p96_mode = RGBFB_B8G8R8A8;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "VNC VNCPix_B8G8R8A8 (%" PRId32 ") modeid -> RGBFB_B8G8R8A8 (%" PRId32 ")\n", vnc_mode, p96_mode );
			}
			break;
		}

		default:
		{
			p96_mode = -1;

			if ( DoVerbose > 0 )
			{
				SHELLBUF_PRINTF2( "Unsupported VNC Format ?? (%" PRIu32 ") mode (%" PRId32 ")\n", vnc_mode, p96_mode );
			}
			break;
		}
	}

	return( p96_mode );
}
