 
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

static void mySetGeneric( struct Config *cfg, struct PixelMessage *msg )
{
int x;
int y;

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
	cfg->GfxRead_Encode_Format = PIXF_A8R8G8B8;
	cfg->GfxRead_Encode_FormatSize = 4;

	// --

	x = 8;
	y = 1;

	while( y < msg->pm_RedMax )
	{
		y = y << 1;
		x--;
	}

	cfg->GfxRead_Enocde_RedShift = x;

	// --

	x = 8;
	y = 1;

	while( y < msg->pm_GreenMax )
	{
		y = y << 1;
		x--;
	}

	cfg->GfxRead_Enocde_GreenShift = x;

	// --

	x = 8;
	y = 1;

	while( y < msg->pm_BlueMax )
	{
		y = y << 1;
		x--;
	}

	cfg->GfxRead_Enocde_BlueShift = x;

	// --

bailout:

	return;
}

// --

static void mySetEncoding_RGB888( struct Config *cfg, struct PixelMessage *msg UNUSED )
{
//	IExec->DebugPrintF( "mySetEncoding_RGB888\n" );
//	printf( "mySetEncoding_RGB888\n" );

	cfg->GfxRead_Encode_FuncName	= "Copy";
	cfg->GfxRead_Encode_Format		= PIXF_A8R8G8B8;
	cfg->GfxRead_Encode_FormatSize	= 4;
	cfg->GfxRead_Encode_RenderTile	= TileRender_Copy;
//	cfg->Enc_BufferRender	= BufferRender_RGB888;
}

static void mySetEncoding_RGB888PC( struct Config *cfg, struct PixelMessage *msg UNUSED )
{
//	IExec->DebugPrintF( "mySetEncoding_RGB888PC\n" );
//	printf( "mySetEncoding_RGB888PC\n" );

	cfg->GfxRead_Encode_FuncName	= "Copy";
	cfg->GfxRead_Encode_Format		= PIXF_B8G8R8A8;
	cfg->GfxRead_Encode_FormatSize	= 4;
	cfg->GfxRead_Encode_RenderTile	= TileRender_Copy;
//	cfg->Enc_BufferRender	= BufferRender_RGB888PC;
}

static void mySetEncoding_RGB565PC( struct Config *cfg, struct PixelMessage *msg UNUSED )
{
//	IExec->DebugPrintF( "mySetEncoding_RGB565PC\n" );
//	printf( "mySetEncoding_RGB565PC\n" );

	cfg->GfxRead_Encode_FuncName	= "Copy";
	cfg->GfxRead_Encode_Format		= PIXF_R5G6B5PC;
	cfg->GfxRead_Encode_FormatSize	= 2;
	cfg->GfxRead_Encode_RenderTile	= TileRender_Copy;
//	cfg->Enc_BufferRender	= BufferRender_RGB565PC;
}

// --

struct myFormatClient
{
	struct PixelMessage *Pixel;
	void (*Function)( struct Config *cfg, struct PixelMessage *msg );
};

struct myFormatAmiga
{
	int Format;
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
/* pm_BigEndian		*/ 1,
/* pm_TrueColor		*/ 1,
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

static struct PixelMessage myRGB888PC =
{
/* pm_Type			*/ 0,
/* pm_Pad			*/ 0,
/* pm_Pad2			*/ 0,
/* pm_Pad3			*/ 0,
/* pm_BitsPerPixel	*/ 32,
/* pm_Depth			*/ 24,
/* pm_BigEndian		*/ 0,
/* pm_TrueColor		*/ 1,
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

static struct PixelMessage myRGB565PC =
{
/* pm_Type			*/ 0,
/* pm_Pad			*/ 0,
/* pm_Pad2			*/ 0,
/* pm_Pad3			*/ 0,
/* pm_BitsPerPixel	*/ 16,
/* pm_Depth			*/ 16,
/* pm_BigEndian		*/ 0,
/* pm_TrueColor		*/ 1,
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
{ & myRGB888,		mySetEncoding_RGB888 },
{ & myRGB888PC, 	mySetEncoding_RGB888PC },
{ NULL,				mySetGeneric }
};

// -- Screen is 16Bit PC -> xx
// -- Any Optimization ? or fallback to Generic
static struct myFormatClient my_R5G6B5PC[] =
{
{ & myRGB565PC,		mySetEncoding_RGB565PC },
{ NULL,				mySetGeneric }
};

// -- Amiga ScreenModes
// -- Screen modes with some Optimization
static struct myFormatAmiga my_Modes[] =
{
{ PIXF_A8R8G8B8,	my_A8R8G8B8 },
{ PIXF_R5G6B5PC,	my_R5G6B5PC },
{ 0, NULL }
};

// --

void mySetEncoding_Message( struct Config *cfg, struct PixelMessage *msg, int User )
{
struct myFormatClient *format;
struct CommandPxlFmt *pfmsg;
int pos;

	// --

//	cfg->cfg_ServerDoFullUpdate = TRUE;

	if ( User )
	{
		cfg->GfxRead_Enocde_ActivePixelSet = TRUE;
	}

//	printf( "Setting GfxRead_Enocde_ActivePixel 22\n" );
	memcpy( & cfg->GfxRead_Enocde_ActivePixel, msg, sizeof( struct PixelMessage ));

	// -- Update GUI

	pfmsg = myCalloc( sizeof( struct CommandPxlFmt ) );

	if ( pfmsg )
	{
		pfmsg->cpf_Command = CMD_PxlFmt;
		pfmsg->cpf_Format = *msg;
		pfmsg->cpf_Client = TRUE;

		// --

		IExec->PutMsg( CmdMsgPort, & pfmsg->cpf_Message );
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

	IExec->ObtainSemaphore( & cfg->GfxRead_Screen_Sema );

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

	IExec->ReleaseSemaphore( & cfg->GfxRead_Screen_Sema );

	// --

}

// --

void mySetEncoding_Format( struct Config *cfg, int Format )
{
	// This Stops Reading from Screen
	cfg->GfxRead_Encode_Format = 0;
	cfg->GfxRead_Encode_FormatSize = 0;
	cfg->GfxRead_Encode_RenderTile = NULL;

	if ( cfg->GfxRead_Enocde_ActivePixelSet )
	{
		mySetEncoding_Message( cfg, & cfg->GfxRead_Enocde_ActivePixel, FALSE );
	}
	else
	{
		switch( Format )
		{
			case PIXF_A8R8G8B8:
			{
				mySetEncoding_Message( cfg, & myRGB888, FALSE );
				break;
			}

			case PIXF_R5G6B5PC:
			{
				mySetEncoding_Message( cfg, & myRGB565PC, FALSE );
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

