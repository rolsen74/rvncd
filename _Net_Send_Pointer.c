
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// --

#define IMG_WIDTH	16
#define IMG_HEIGHT	12

static const uint8 PointerData[] =
	"2300000000000000"	// 1
	"1233000000000000"	// 2
	"0122330000000000"	// 3
	"0122223300000000"	// 4
	"0012222233000000"	// 5
	"0012222222000000"	// 6
	"0001222300000000"	// 7
	"0001221230000000"	// 8
	"0000120123000000"	// 9
	"0000120012300000"	// 10
	"0000000001000000"	// 11
	"0000000000000000";	// 12

// --

static void myBuildMask( struct Config *cfg )
{
uint8 *chunky;
uint8 *mask;
int pos;
int x;
int y;

	chunky	= (APTR) cfg->cfg_PointerChunky;
	mask	= (APTR) cfg->cfg_PointerMask2;

	memset( mask, 0, 64*64*1 );

	for( y=0 ; y<cfg->cfg_PointerHeight ; y++ )
	{
		for( x=0 ; x<cfg->cfg_PointerWidth ; x++ )
		{
			pos = (( y * 64 * 4 ) + ( x * 4 ));

			if ( chunky[ pos + 0 ] > 25 ) // Check Alpha Channel
			{
				mask[ ( y * 64 ) + x ] = 1;
			}
		}
	}
}

// --

static int myDisk( struct Config *cfg )
{
struct BitMapHeader *bmh;
Object *obj;
uint32 ret;
int error;

	error = TRUE;

	obj = IDataTypes->NewDTObject( cfg->cfg_PointerFilename2,
		DTA_GroupID, GID_PICTURE,
		TAG_END
	);

	if ( obj == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Pointer file '%s' not found", cfg->cfg_PointerFilename2 );
		goto bailout;
	}

	ret = IDataTypes->GetDTAttrs( obj,
		PDTA_BitMapHeader, & bmh,
		TAG_END
	);

	if ( ret != 1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed getting Bitmap Header Info" );
		goto bailout;
	}

	if (( bmh->bmh_Width  > 64 )
	||	( bmh->bmh_Height > 64 ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Pointer Image exceed 64x64 size (%ldx%ld)", bmh->bmh_Width, bmh->bmh_Height );
		goto bailout;
	}

	ret = IIntuition->IDoMethod( obj,
		PDTM_READPIXELARRAY,
		cfg->cfg_PointerChunky,
		PBPAFMT_ARGB,
		64 * 4,
		0,
		0,
		bmh->bmh_Width,
		bmh->bmh_Height
	);

	if ( ret != 1 )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to read Image graphic data" );
		goto bailout;
	}

	cfg->cfg_PointerWidth = bmh->bmh_Width;
	cfg->cfg_PointerHeight = bmh->bmh_Height;

	error = FALSE;

bailout:

	if ( obj )
	{
		IIntuition->DisposeObject( obj );
	}

	return( error );
}

// --

static int myInternal( struct Config *cfg )
{
const uint8 *ptr;
uint32 *chunky;
int error;
int col;
int pos;
int x;
int y;

	error = TRUE;

	chunky = (APTR) cfg->cfg_PointerChunky;

	ptr = & PointerData[0];
	pos = 0;

	for( y=0 ; y<IMG_HEIGHT ; y++ )
	{
		for( x=0 ; x<IMG_WIDTH ; x++ )
		{
			pos = ( y * 64 ) + x ;

			switch( *ptr++ )
			{
				case '1':
				{
					col = 0xff000000;
					break;
				}

				case '2':
				{
					col = 0xffff0000;
					break;
				}

				case '3':
				{
					col = 0xffffffff;
					break;
				}

				default:
				{
					col = 0; // Transparent;
					break;
				}
			}

			chunky[pos] = col;
		}
	}

	cfg->cfg_PointerWidth	= IMG_WIDTH;
	cfg->cfg_PointerHeight	= IMG_HEIGHT;

	error = FALSE;

// bailout:

	return( error );
}

// --

int Send_Load_Pointer( struct Config *cfg )
{
int error;

	error = TRUE;

	// always 32bit ARGB
	cfg->cfg_PointerChunky = myMalloc( 64 * 64 * 4 );

	if ( cfg->cfg_PointerChunky == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocting Pointer memory" );
		goto bailout;
	}

	cfg->cfg_PointerBuffer = myMalloc( 64 * 64 * 4 );

	if ( cfg->cfg_PointerBuffer == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocting Pointer memory" );
		goto bailout;
	}

	cfg->cfg_PointerMask = myMalloc( 64 * 64 * 4 );

	if ( cfg->cfg_PointerMask == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocting Pointer memory" );
		goto bailout;
	}

	cfg->cfg_PointerMask2 = myMalloc( 64 * 64 * 1 );

	if ( cfg->cfg_PointerMask2 == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocting Pointer memory" );
		goto bailout;
	}

	if ( cfg->cfg_PointerFilename2 )
	{
		myFree( cfg->cfg_PointerFilename2 );
		cfg->cfg_PointerFilename2 = NULL;
	}

	if ( cfg->cfg_Active_Settings.PointerType )
	{
		if (( cfg->cfg_PointerFilename ) && ( cfg->cfg_PointerFilename[0] ))
		{
			cfg->cfg_PointerFilename2 = myStrdup( cfg->cfg_PointerFilename );

			if ( cfg->cfg_PointerFilename2 == NULL )
			{
				Log_PrintF( cfg, LOGTYPE_Error, "Error allocting Pointer memory" );
				goto bailout;
			}

			error = myDisk( cfg );

			if ( error )
			{
				goto bailout;
			}
		}
		else
		{
			Log_PrintF( cfg, LOGTYPE_Error, "Error missing Pointer Filename" );
			goto bailout;
		}

		Log_PrintF( cfg, LOGTYPE_Info, "Using Custom Mouse Pointer Image (%ldx%ld)", cfg->cfg_PointerWidth, cfg->cfg_PointerHeight );
	}
	else
	{
		error = myInternal( cfg );

		if ( error )
		{
			goto bailout;
		}

		Log_PrintF( cfg, LOGTYPE_Info, "Using Built-in Mouse Pointer Image (%ldx%ld)", cfg->cfg_PointerWidth, cfg->cfg_PointerHeight );
	}

	myBuildMask( cfg );

bailout:

	cfg->cfg_PointerFormatID = -1;
	cfg->cfg_ServerSupportsCursor = ( error ) ? FALSE : TRUE ;
	memset( & cfg->cfg_PointerFormat, -1, sizeof( struct PixelMessage ));

	return( error );
}

// --

void Send_Free_Pointer( struct Config *cfg )
{
	if ( cfg->cfg_PointerChunky )
	{
		myFree( cfg->cfg_PointerChunky );
		cfg->cfg_PointerChunky = NULL;
	}

	if ( cfg->cfg_PointerBuffer )
	{
		myFree( cfg->cfg_PointerBuffer );
		cfg->cfg_PointerBuffer = NULL;
	}

	if ( cfg->cfg_PointerMask2 )
	{
		myFree( cfg->cfg_PointerMask2 );
		cfg->cfg_PointerMask2 = NULL;
	}

	if ( cfg->cfg_PointerMask )
	{
		myFree( cfg->cfg_PointerMask );
		cfg->cfg_PointerMask = NULL;
	}

	if ( cfg->cfg_PointerFilename2 )
	{
		myFree( cfg->cfg_PointerFilename2 );
		cfg->cfg_PointerFilename2 = NULL;
	}

	cfg->cfg_ServerSupportsCursor = FALSE;
}

// --
