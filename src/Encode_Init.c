
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#if defined(HAVE_CGFX)

#define CONV	myConvert_VNC_2_CGFX_Mode

#elif defined(HAVE_P96)

#define CONV	myConvert_VNC_2_P96_Mode

#elif defined(HAVE_GFX54)

#define CONV	myConvert_VNC_2_GFX54_Mode

#else

	#error Unknown Graphics System

#endif

// --

static void mySetGeneric( struct Config *cfg, struct PixelMessage *msg )
{
S32 x;
S32 y;

	/**/ if ( msg->pm_BitsPerPixel == 0 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Unsupported Bits width (%ld)", msg->pm_BitsPerPixel );
		goto bailout;
	}
	else if ( msg->pm_BitsPerPixel <= 8 )
	{
		cfg->GfxRead_Encode_RenderTile = TileRender_Generic_8;
	}
	else if ( msg->pm_BitsPerPixel <= 16 )
	{
		if ( msg->pm_BigEndian )
		{
			cfg->GfxRead_Encode_RenderTile = TileRender_Generic_16BE;
		}
		else
		{
			cfg->GfxRead_Encode_RenderTile = TileRender_Generic_16LE;
		}
	}
	else if ( msg->pm_BitsPerPixel <= 24 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Unsupported Bits width (%ld)", msg->pm_BitsPerPixel );
		goto bailout;
	}
	else if ( msg->pm_BitsPerPixel <= 32 )
	{
		if ( msg->pm_BigEndian )
		{
			cfg->GfxRead_Encode_RenderTile = TileRender_Generic_32BE;
		}
		else
		{
			cfg->GfxRead_Encode_RenderTile = TileRender_Generic_32LE;
		}
	}
	else
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Unsupported Bits width (%ld)", msg->pm_BitsPerPixel );
		goto bailout;
	}

	// --

	cfg->GfxRead_Encode_FuncName = "Generic";
	cfg->GfxRead_Encode_Format = VNCPix_A8R8G8B8;
	cfg->GfxRead_Encode_Format2 = CONV( VNCPix_A8R8G8B8 );
	cfg->GfxRead_Encode_FormatSize = 4;

	// --

	x = 8;
	y = 1;

	while( y < msg->pm_RedMax )
	{
		y = y << 1;
		x--;
	}

	cfg->GfxRead_Encode_RedShift = x;

	// --

	x = 8;
	y = 1;

	while( y < msg->pm_GreenMax )
	{
		y = y << 1;
		x--;
	}

	cfg->GfxRead_Encode_GreenShift = x;

	// --

	x = 8;
	y = 1;

	while( y < msg->pm_BlueMax )
	{
		y = y << 1;
		x--;
	}

	cfg->GfxRead_Encode_BlueShift = x;

	// --

bailout:

	return;
}

// --

static void mySetEncoding_RGB888( struct Config *cfg, struct PixelMessage *msg UNUSED )
{
	if ( DoVerbose > 1 )
	{
		SHELLBUF_PRINTF( "mySetEncoding_RGB888\n" );
	}

	cfg->GfxRead_Encode_FuncName	= "Copy";
	cfg->GfxRead_Encode_Format		= VNCPix_A8R8G8B8;		// Format we want
	cfg->GfxRead_Encode_Format2		= CONV( VNCPix_A8R8G8B8 );
	cfg->GfxRead_Encode_FormatSize	= 4;
	cfg->GfxRead_Encode_RenderTile	= TileRender_Copy;
}

static void mySetEncoding_RGB888PC( struct Config *cfg, struct PixelMessage *msg UNUSED )
{
	if ( DoVerbose > 1 )
	{
		SHELLBUF_PRINTF( "mySetEncoding_RGB888PC\n" );
	}

	cfg->GfxRead_Encode_FuncName	= "Copy";
	cfg->GfxRead_Encode_Format		= VNCPix_B8G8R8A8;		// Format we want
	cfg->GfxRead_Encode_Format2		= CONV( VNCPix_B8G8R8A8 );
	cfg->GfxRead_Encode_FormatSize	= 4;
	cfg->GfxRead_Encode_RenderTile	= TileRender_Copy;
}

static void mySetEncoding_RGB565PC( struct Config *cfg, struct PixelMessage *msg UNUSED )
{
	if ( DoVerbose > 1 )
	{
		SHELLBUF_PRINTF( "mySetEncoding_RGB565PC\n" );
	}

	cfg->GfxRead_Encode_FuncName	= "Copy";
	cfg->GfxRead_Encode_Format		= VNCPix_R5G6B5PC;		// Format we want
	cfg->GfxRead_Encode_Format2		= CONV( VNCPix_R5G6B5PC );
	cfg->GfxRead_Encode_FormatSize	= 2;
	cfg->GfxRead_Encode_RenderTile	= TileRender_Copy;
}

static void mySetEncoding_RGB565( struct Config *cfg, struct PixelMessage *msg UNUSED )
{
	if ( DoVerbose > 1 )
	{
		SHELLBUF_PRINTF( "mySetEncoding_RGB565\n" );
	}

	cfg->GfxRead_Encode_FuncName	= "Copy";
	cfg->GfxRead_Encode_Format		= VNCPix_R5G6B5;		// Format we want
	cfg->GfxRead_Encode_Format2		= CONV( VNCPix_R5G6B5 );
	cfg->GfxRead_Encode_FormatSize	= 2;
	cfg->GfxRead_Encode_RenderTile	= TileRender_Copy;
}

// --

struct myFormatClient
{
	struct PixelMessage *Pixel;
	void (*Function)( struct Config *cfg, struct PixelMessage *msg );
};

struct myFormatAmiga
{
	enum VNCPix Format;
	struct myFormatClient *Struct;
};

// --

static struct PixelMessage myRGB888 =
{
/* pm_Type			*/ 0,
/* pm_Pad			*/ 0,
/* pm_Pad2			*/ 0,
/* pm_Pad3			*/ 0,
/* pm_BitsPerPixel	*/ 32,
/* pm_Depth			*/ 24,
/* pm_BigEndian		*/ TRUE,
/* pm_TrueColor		*/ TRUE,
/* pm_RedMax		*/ 255,
/* pm_GreenMax		*/ 255,
/* pm_BlueMax		*/ 255,
/* pm_RedShift		*/ 16,
/* pm_GreenShift	*/ 8,
/* pm_BlueShift		*/ 0,
/* pm_Pad4			*/ 0,
/* pm_Pad5			*/ 0,
/* pm_Pad6			*/ 0
};

static struct PixelMessage myBGR888 =
{
/* pm_Type			*/ 0,
/* pm_Pad			*/ 0,
/* pm_Pad2			*/ 0,
/* pm_Pad3			*/ 0,
/* pm_BitsPerPixel	*/ 32,
/* pm_Depth			*/ 24,
/* pm_BigEndian		*/ TRUE,
/* pm_TrueColor		*/ TRUE,
/* pm_RedMax		*/ 255,
/* pm_GreenMax		*/ 255,
/* pm_BlueMax		*/ 255,
/* pm_RedShift		*/ 0,
/* pm_GreenShift	*/ 8,
/* pm_BlueShift		*/ 16,
/* pm_Pad4			*/ 0,
/* pm_Pad5			*/ 0,
/* pm_Pad6			*/ 0
};

static struct PixelMessage myRGB888PC =
{
/* pm_Type			*/ 0,
/* pm_Pad			*/ 0,
/* pm_Pad2			*/ 0,
/* pm_Pad3			*/ 0,
/* pm_BitsPerPixel	*/ 32,
/* pm_Depth			*/ 24,
/* pm_BigEndian		*/ FALSE,
/* pm_TrueColor		*/ TRUE,
/* pm_RedMax		*/ 255,
/* pm_GreenMax		*/ 255,
/* pm_BlueMax		*/ 255,
/* pm_RedShift		*/ 16,
/* pm_GreenShift	*/ 8,
/* pm_BlueShift		*/ 0,
/* pm_Pad4			*/ 0,
/* pm_Pad5			*/ 0,
/* pm_Pad6			*/ 0
};

static struct PixelMessage myRGB565 =
{
/* pm_Type			*/ 0,
/* pm_Pad			*/ 0,
/* pm_Pad2			*/ 0,
/* pm_Pad3			*/ 0,
/* pm_BitsPerPixel	*/ 16,
/* pm_Depth			*/ 16,
/* pm_BigEndian		*/ TRUE,
/* pm_TrueColor		*/ TRUE,
/* pm_RedMax		*/ 31,
/* pm_GreenMax		*/ 63,
/* pm_BlueMax		*/ 31,
/* pm_RedShift		*/ 11,
/* pm_GreenShift	*/ 5,
/* pm_BlueShift		*/ 0,
/* pm_Pad4			*/ 0,
/* pm_Pad5			*/ 0,
/* pm_Pad6			*/ 0
};

static struct PixelMessage myRGB565PC =
{
/* pm_Type			*/ 0,
/* pm_Pad			*/ 0,
/* pm_Pad2			*/ 0,
/* pm_Pad3			*/ 0,
/* pm_BitsPerPixel	*/ 16,
/* pm_Depth			*/ 16,
/* pm_BigEndian		*/ FALSE,
/* pm_TrueColor		*/ TRUE,
/* pm_RedMax		*/ 31,
/* pm_GreenMax		*/ 63,
/* pm_BlueMax		*/ 31,
/* pm_RedShift		*/ 11,
/* pm_GreenShift	*/ 5,
/* pm_BlueShift		*/ 0,
/* pm_Pad4			*/ 0,
/* pm_Pad5			*/ 0,
/* pm_Pad6			*/ 0
};

// -- Screen is 32Bit ARGB -> xx
// -- Any Optimization ? or fallback to Generic
static struct myFormatClient my_A8R8G8B8[] =
{
{ & myRGB565,		mySetEncoding_RGB565 },
{ & myRGB888,		mySetEncoding_RGB888 },
{ & myRGB565PC,		mySetEncoding_RGB565PC },
{ & myRGB888PC, 	mySetEncoding_RGB888PC },
{ NULL,				mySetGeneric }
};

// -- Screen is 16Bit -> xx
// -- Any Optimization ? or fallback to Generic
static struct myFormatClient my_R5G6B5[] =
{
{ & myRGB565,		mySetEncoding_RGB565 },
{ & myRGB565PC,		mySetEncoding_RGB565PC },
{ NULL,				mySetGeneric }
};

// -- Screen is 16Bit PC -> xx
// -- Any Optimization ? or fallback to Generic
static struct myFormatClient my_R5G6B5PC[] =
{
{ & myRGB565,		mySetEncoding_RGB565 },
{ & myRGB565PC,		mySetEncoding_RGB565PC },
{ NULL,				mySetGeneric }
};

// -- Amiga ScreenModes
// -- Screen modes with some Optimization

static struct myFormatAmiga my_Modes[] =
{
{ VNCPix_A8R8G8B8,	my_A8R8G8B8 },
{ VNCPix_R5G6B5PC,	my_R5G6B5PC },
{ VNCPix_R5G6B5,	my_R5G6B5 },
{ 0, NULL }
};

// --

void mySetEncoding_Message( struct Config *cfg, struct PixelMessage *msg, S32 User )
{
struct myFormatClient *format;
struct CommandPxlFmt *pfmsg;
S32 pos;

	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF( "mySetEncoding_Message\n" );
	}

	// --

	if ( User )
	{
		cfg->GfxRead_Encode_ActivePixelSet = TRUE;
	}

//	SHELLBUF_PRINTF( "Setting GfxRead_Encode_ActivePixel 22\n" );
	memcpy( & cfg->GfxRead_Encode_ActivePixel, msg, sizeof( struct PixelMessage ));
	cfg->GfxRead_Encode_ActivePixelID++;

	// -- Update GUI

	pfmsg = mem_AllocClr( sizeof( struct CommandPxlFmt ) );

	if ( pfmsg )
	{
		pfmsg->cpf_Command = CMD_PxlFmt;
		pfmsg->cpf_Format = *msg;
		pfmsg->cpf_Client = TRUE;

		// --

		MsgPort_PutMsg( & CmdMsgPort, & pfmsg->cpf_Message );
	}

	// --

	pos = 0;

	while( my_Modes[pos].Format )
	{
		if ( my_Modes[pos].Format == cfg->GfxRead_Screen_Format )		
		{
			break;
		}
		else
		{
			pos++;
		}
	}

	if ( ! my_Modes[pos].Format )
	{
		// No optimized functions, using fallback
		mySetGeneric( cfg, msg );
	}
	else
	{
		format = my_Modes[pos].Struct;

		pos = 0;

		while( format[pos].Pixel )
		{
			if ( ! memcmp( format[pos].Pixel, msg, sizeof( struct PixelMessage )))
			{
				break;
			}
			else
			{
				pos++;
			}
		}

		format[pos].Function( cfg, msg );
	}

	// --

	ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

	if ( cfg->GfxRead_Screen_ChunkyBuffer )
	{
		if ( cfg->GfxRead_Encode_OldFormat != cfg->GfxRead_Encode_Format )
		{
			// Clear Screen
			memset( cfg->GfxRead_Screen_ChunkyBuffer, 0, cfg->GfxRead_Screen_ChunkySize );

			// Only Send update when GfxRead have read it again
			memset( cfg->GfxRead_Screen_TileArrayBuffer, 0, cfg->GfxRead_Screen_Tiles );
		}
		else
		{
			// Force a resend updates
			memset( cfg->GfxRead_Screen_TileArrayBuffer, 0x80, cfg->GfxRead_Screen_Tiles );
		}
	}

	cfg->GfxRead_Encode_OldFormat = cfg->GfxRead_Encode_Format;

	ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );

	// --

}

// --

void mySetEncoding_Format( struct Config *cfg, enum VNCPix Format )
{
	if ( DoVerbose > 2 )
	{
		SHELLBUF_PRINTF1( "mySetEncoding_Format (VNCPix %ld)\n", Format );
	}

	// This Stops Reading from Screen
	cfg->GfxRead_Encode_Format = 0;
	cfg->GfxRead_Encode_FormatSize = 0;
	cfg->GfxRead_Encode_RenderTile = NULL;

	if ( cfg->GfxRead_Encode_ActivePixelSet )
	{
		mySetEncoding_Message( cfg, & cfg->GfxRead_Encode_ActivePixel, FALSE );
	}
	else
	{
		switch( Format )
		{
			case VNCPix_R5G6B5:
			{
				mySetEncoding_Message( cfg, & myRGB565, FALSE );
				break;
			}

			case VNCPix_R5G6B5PC:
			{
				mySetEncoding_Message( cfg, & myRGB565PC, FALSE );
				break;
			}

			case VNCPix_A8R8G8B8:
			{
				mySetEncoding_Message( cfg, & myRGB888, FALSE );
				break;
			}

			case VNCPix_B8G8R8A8:
			{
				mySetEncoding_Message( cfg, & myBGR888, FALSE );
				break;
			}

			default:
			{
				if ( Format )
				{
					Log_PrintF( cfg, LOGTYPE_Error, "Unsupported Amiga Pixel Format : %ld", Format );
				}
				break;
			}
		}
	}
}

// --

