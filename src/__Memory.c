
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --
// Don't call SHELLBUF_PRINTF .. it will Busy loop

PTR mem_Alloc( S32 size )
{
PTR mem;

	if ( size > 0 )
	{
		#ifdef _AOS4_

		mem = AllocVecTags( size,
			TAG_END
		);

		#else

		mem = AllocVec( size, MEMF_ANY );

		#endif

//		if ( DoVerbose > 2 )
//		{
//			SHELLBUF_PRINTF( "mem_Alloc( Size: %" PRId32 ", Mem: $%08" PRIX32 " )\n", size, (U32) mem );
//		}
	}
	else
	{
//		SHELLBUF_PRINTF( "Error: mem_Alloc( Size: %" PRId32 " )\n", size );
		mem = NULL;
	}

	return( mem );
}

// --

PTR mem_AllocClr( S32 size )
{
PTR mem;

	if ( size > 0 )
	{
		#ifdef _AOS4_

		mem = AllocVecTags( size,
			AVT_Clear, 0,
			TAG_END
		);

		#else

		mem = AllocVec( size, MEMF_ANY | MEMF_CLEAR );

		#endif

//		if ( DoVerbose > 2 )
//		{
//			SHELLBUF_PRINTF( "mem_AllocClr( Size: %" PRId32 ", Mem: $%08" PRIX32 " )\n", size, (U32) mem );
//		}
	}
	else
	{
//		SHELLBUF_PRINTF( "Error: mem_AllocClr( Size: %" PRId32 " )\n", size );
		mem = NULL;
	}

	return( mem );
}

// --

STR mem_Strdup( STR str )
{
STR copy;
S32 len;

	if ( str )
	{
		len = strlen( str );

		copy = mem_Alloc( len + 1 );

		if ( copy )
		{
			if ( len )
			{ 
				memcpy( copy, str, len );
			}

			copy[len] = 0;
		}
	}
	else
	{
		copy = NULL;
	}

	return( copy );
}

// --

STR mem_ASPrintF( STR fmt, ... )
{
va_list args;
STR buf;
S32 size;

	buf = NULL;

	// ----

	va_start( args, fmt );
	size = vsnprintf( NULL, 0, fmt, args );
	va_end( args );

	if ( size <= 0 )
	{
//		SHELLBUF_PRINTF( "Error calculating Log buffer size\n" );
		goto bailout;
	}

	#if 1
	size += 1;
	buf = mem_Alloc( size );
	#else
	buf = mem_Alloc( size + 1 );
	#endif

	if ( ! buf )
	{
//		SHELLBUF_PRINTF( "Error allocating Log buffer\n" );
		goto bailout;
	}

	va_start( args, fmt );
	vsnprintf( buf, size, fmt, args );
	va_end( args );

	// ----

bailout:

	return( buf );
}

// --

void mem_Free( PTR mem )
{
	if ( ! mem )
	{
		goto bailout;
	}

//	if ( DoVerbose > 2 )
//	{
//		SHELLBUF_PRINTF( "mem_Free( Mem: $%08" PRIX32 " )\n", (U32) mem );
//	}

	FreeVec( mem );

bailout:

	return;
}

// --


