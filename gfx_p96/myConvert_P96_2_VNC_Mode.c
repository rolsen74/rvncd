
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

enum VNCPix myConvert_P96_2_VNC_Mode( RGBFTYPE p96_mode )
{
enum VNCPix vnc_mode;

	switch( p96_mode )
	{
		case RGBFB_R5G6B5: // x
		{
			vnc_mode = VNCPix_R5G6B5;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "P96 RGBFB_R5G6B5 (%" PRId32 ") modeid -> VNCPix_R5G6B5 (%" PRId32 ")\n", p96_mode, vnc_mode );
			}
			break;
		}

		case RGBFB_R5G6B5PC: // x
		{
			vnc_mode = VNCPix_R5G6B5PC;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "P96 RGBFB_R5G6B5PC (%" PRId32 ") modeid -> VNCPix_R5G6B5PC (%" PRId32 ")\n", p96_mode, vnc_mode );
			}
			break;
		}

		case RGBFB_A8R8G8B8: // 6
		{
			vnc_mode = VNCPix_A8R8G8B8;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "P96 RGBFB_A8R8G8B8 (%" PRId32 ") modeid -> VNCPix_A8R8G8B8 (%" PRId32 ")\n", p96_mode, vnc_mode );
			}
			break;
		}

		case RGBFB_B8G8R8A8: // x
		{
			vnc_mode = VNCPix_B8G8R8A8;

			if ( DoVerbose > 2 )
			{
				SHELLBUF_PRINTF2( "P96 RGBFB_B8G8R8A8 (%" PRId32 ") modeid -> VNCPix_B8G8R8A8 (%" PRId32 ")\n", p96_mode, vnc_mode );
			}
			break;
		}

		default:
		{
			vnc_mode = VNCPix_Unknown;

			if ( DoVerbose > 0 )
			{
				SHELLBUF_PRINTF2( "Unsupported P96 Format ?? (%" PRIu32 ") mode (%" PRId32 ")\n", p96_mode, vnc_mode );
			}
			break;
		}
	}

	return( vnc_mode );
}

// --
