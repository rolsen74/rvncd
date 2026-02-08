
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

#define MAX_BUFFER		1024*64
#define MAX_TEMP		64

// --

static struct SignalSemaphore ShellBuf_Buffer_Sema;
static U32 ShellBuf_Buffer_Start;
static U32 ShellBuf_Buffer_End;
static U32 ShellBuf_Buffer_Size;
static STR ShellBuf_Buffer;
U32 ShellBuf_Enable;
U32 ShellBuf_Inited;

// -- 

S32 _ShellBuf_Init( void )
{
S32 retval;

	retval = FALSE;

	if ( ! ShellBuf_Inited )
	{
		ShellBuf_Buffer = mem_Alloc( MAX_BUFFER );

		if ( ! ShellBuf_Buffer )
		{
			printf( "Failed too alloc memory\n" );
			goto bailout;
		}

		InitSemaphore( & ShellBuf_Buffer_Sema );

		ShellBuf_Enable = 0;
		ShellBuf_Buffer_End = 0;
		ShellBuf_Buffer_Size = MAX_BUFFER;
		ShellBuf_Buffer_Start = 0;
		ShellBuf_Inited = TRUE;
	}

	retval = TRUE;

bailout:

	return( retval );
}

// -- 

void _ShellBuf_Free( void )
{
	if ( ! ShellBuf_Inited )
	{
		printf( "SHELLBUF_FREE : Not Inited :\n" );
		return;
	}

	if ( ShellBuf_Buffer )
	{
		SHELLBUF_FLUSH();
		mem_Free( ShellBuf_Buffer );
		ShellBuf_Buffer_Size = 0;
		ShellBuf_Buffer = NULL;
	}
}

// -- 

void _ShellBuf_Flush( void )
{
char buf[256];
int doflush;

	if ( ! ShellBuf_Inited )
	{
		printf( "SHELLBUF_FLUSH : Not Inited :\n" );
		return;
	}

	SHELLBUF_LOCK();

	doflush = 0;

	// Flush buffer
	while( SHELLBUF_GETBYTES( buf, 256 ))
	{
		doflush = TRUE;
		printf( "%s", buf );
	}

	if ( doflush )
	{
		fflush( stdout );
	}

	SHELLBUF_UNLOCK();
}

// --

void _ShellBuf_Lock( void )
{
	if ( ! ShellBuf_Inited )
	{
		printf( "SHELLBUF_LOCK : Not Inited :\n" );
	}
	else
	{
		ObtainSemaphore( & ShellBuf_Buffer_Sema );
	}
}

// --

void _ShellBuf_Unlock( void )
{
	if ( ! ShellBuf_Inited )
	{
		printf( "SHELLBUF_UNLOCK : Not Inited :\n" );
	}
	else
	{
		ReleaseSemaphore( & ShellBuf_Buffer_Sema );
	}
}

// --

#ifdef DEBUG

void _ShellBuf_Insert( STR data, S32 len, STR file UNUSED )

#else

void _ShellBuf_Insert( STR data, S32 len )

#endif

{
char buf[MAX_TEMP];
S32 space;
S32 write;
S32 l;

	if ( ! ShellBuf_Inited )
	{
		printf( "ShellBuf_Insert : Not Inited :\n" );
		return;
	}

	if ( len <= 0 )
	{
		return;
	}

	if ( ! ShellBuf_Enable )
	{
		while( len > 0 )
		{
			l = MIN( len, MAX_TEMP-1 );
			memcpy( buf, data, l );
			buf[l] = 0;
			data += l;
			len -= l;
			printf( "%s", buf );
		}
	}
	else
	{
		SHELLBUF_LOCK();

		/**/ if ( ShellBuf_Buffer_Start == ShellBuf_Buffer_End )
		{
			// Empty buffer
			space = MAX_BUFFER - 1 ;
		}
		else if ( ShellBuf_Buffer_Start < ShellBuf_Buffer_End )
		{
			// Buffer can wrap
			space = MAX_BUFFER - ShellBuf_Buffer_End - 1;
		}
		else
		{
			// Buffer linar
			space = ShellBuf_Buffer_Start - ShellBuf_Buffer_End - 1;
		}

		// Only write as much as fits before wrapping
		write = MIN( space, len );

		while( write > 0 )
		{
			if ( ShellBuf_Buffer_End + write > MAX_BUFFER )
			{
				// Wrap
				l = MAX_BUFFER - ShellBuf_Buffer_End;
			}
			else
			{
				// Fits in buffer
				l = write;
			}

			if ( l <= 0 )
			{
				break;
			}

			memcpy( & ShellBuf_Buffer[ ShellBuf_Buffer_End ], data, l );

			ShellBuf_Buffer_End += l;

			if ( ShellBuf_Buffer_End >= MAX_BUFFER )
			{
				ShellBuf_Buffer_End = 0;
			}

			write -= l;
			data += l;
		}

		SHELLBUF_UNLOCK();
	}
}

// --

#ifdef DEBUG

void _ShellBuf_PrintF( STR file UNUSED, STR fmt, ... )

#else

void _ShellBuf_PrintF( STR fmt, ... )

#endif

{
va_list args;
S32 size;
STR buf;

	if ( ! ShellBuf_Inited )
	{
		printf( "SHELLBUF_PRINTF : Not Inited :\n" );
		goto bailout;
	}

	// ----
	// Get buffer size

	va_start( args, fmt );

	size = vsnprintf( NULL, 0, fmt, args );

	va_end( args );

	if ( size <= 0 )
	{
		printf( "127 : error fmt %s\n", fmt );
		goto bailout;
	}

	// ----
	// Allocate buffer

	size += 1;
	buf = mem_Alloc( size );

	if ( ! buf )
	{
		printf( "128\n" );
		goto bailout;
	}

	// ----
	// Fill buffer

	va_start( args, fmt );
	vsnprintf( buf, size, fmt, args );
	va_end( args );

	// ----
	// Insert buffer

	SHELLBUF_INSERT( buf, size-1 );

	// ----
	// Free buffer

	mem_Free( buf );

bailout:

	return;
}

// --

U32 _ShellBuf_GetBytes( STR buf, U32 max )
{
U32 available;
U32 len;

	if ( ! ShellBuf_Inited )
	{
		len = 0;
		printf( "SHELLBUF_GETBYTES : Not Inited :\n" );
		goto bailout;
	}

	max--;	// NUL byte

	if ( ShellBuf_Buffer_Start == ShellBuf_Buffer_End )
	{
		len = 0;
	}
	else
	{
		SHELLBUF_LOCK();

		if ( ShellBuf_Buffer_Start < ShellBuf_Buffer_End )
		{
			// Buffer linear
			available = ShellBuf_Buffer_End - ShellBuf_Buffer_Start;
		}
		else
		{
			// Buffer wraps .. only get to end of buffer
			available = MAX_BUFFER - ShellBuf_Buffer_Start;
		}

		len = MIN( available, max );

		memcpy( buf, & ShellBuf_Buffer[ ShellBuf_Buffer_Start ], len );

		ShellBuf_Buffer_Start += len;

		if ( ShellBuf_Buffer_Start >= MAX_BUFFER )
		{
			ShellBuf_Buffer_Start = 0;
		}

		SHELLBUF_UNLOCK();
	}

	buf[len] = 0;

bailout:

	return( len );
}

// --
