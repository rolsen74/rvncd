 
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

static int myDisk( struct Config *cfg )
{
struct BitMapHeader *bmh;
Object *obj;
uint32 ret;
int error;

	error = TRUE;

	obj = IDataTypes->NewDTObject( cfg->cfg_PointerFileName,
		DTA_GroupID, GID_PICTURE,
		TAG_END
	);

	if ( obj == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Pointer file '%s' not found", cfg->cfg_PointerFileName );
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

	if (( bmh->bmh_Width > 64 )
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
//	 1234567890123456

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

	cfg->cfg_PointerChunky = myMalloc( 64 * 64 * 4 );

	if ( cfg->cfg_PointerChunky == NULL )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Error allocting Pointer memory" );
		goto bailout;
	}

	if (( cfg->cfg_PointerFileName ) && ( cfg->cfg_PointerFileName[0] ))
	{
		error = myDisk( cfg );
	}

	if ( error )
	{
		error = myInternal( cfg );
	}

bailout:

	cfg->cfg_ServerSupportsCursor = ( error ) ? FALSE : TRUE ;

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

	cfg->cfg_ServerSupportsCursor = FALSE;
}

// --
