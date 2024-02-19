 
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

int myGetFileSize( char *name, int *size )
{
BPTR h;
int64 filesize;
int retval;

	h = IDOS->Open( name, MODE_OLDFILE );

	if ( h == 0 )
	{
		retval = 0; // Not found
		goto bailout;
	}

	filesize = IDOS->GetFileSize( h );

	if ( filesize < 0 )
	{
		retval = -1; // IO Error
		goto bailout;
	}

	*size = (int) filesize;

	retval = 1; // Found

bailout:

	if ( h )
	{
		IDOS->Close( h );
	}

	return( retval );
}

// --
// Well building my own strerror()
// as newlib or clib2 do not have those

struct ErrString
{
	int		errcode;
	char *	errstring;
};

static struct ErrString myStrings[] =
{
{ EPERM,			"Operation not permitted" },
{ ENOENT,			"No such file or directory" },
{ ESRCH,			"No such process" },
{ EINTR,			"Interrupted system call" },
{ EIO,				"Input/output error" },
{ ENXIO,			"Device not configured" },
{ E2BIG,			"Argument list too long" },
{ ENOEXEC,			"Exec format error" },
{ EBADF,			"Bad file descriptor" },
{ ECHILD,			"No child processes" },
{ EDEADLK,			"Resource deadlock avoided" },
{ ENOMEM,			"Cannot allocate memory" },
{ EACCES,			"Permission denied" },
{ EFAULT,			"Bad address" },
{ ENOTBLK,			"Block device required" },
{ EBUSY,			"Device busy" },
{ EEXIST,			"File exists" },
{ EXDEV,			"Cross-device link" },
{ ENODEV,			"Operation not supported by device" },
{ ENOTDIR,			"Not a directory" },
{ EISDIR,			"Is a directory" },
{ EINVAL,			"Invalid argument" },
{ ENFILE,			"Too many open files in system" },
{ EMFILE,			"Too many open files" },
{ ENOTTY,			"Inappropriate ioctl for device" },
{ ETXTBSY,			"Text file busy" },
{ EFBIG,			"File too large" },
{ ENOSPC,			"No space left on device" },
{ ESPIPE,			"Illegal seek" },
{ EROFS,			"Read-only file system" },
{ EMLINK,			"Too many links" },
{ EPIPE,			"Broken pipe" },
{ EDOM,				"Numerical argument out of domain" },
{ ERANGE,			"Result too large" },
{ EAGAIN,			"Resource temporarily unavailable" },
{ EINPROGRESS,		"Operation now in progress" },
{ EALREADY,			"Operation already in progress" },
{ ENOTSOCK,			"Socket operation on non-socket" },
{ EDESTADDRREQ,		"Destination address required" },
{ EMSGSIZE,			"Message too long" },
{ EPROTOTYPE,		"Protocol wrong type for socket" },
{ ENOPROTOOPT,		"Protocol not available" },
{ EPROTONOSUPPORT,	"Protocol not supported" },
{ ESOCKTNOSUPPORT,	"Socket type not supported" },
{ EOPNOTSUPP,		"Operation not supported on socket" },
{ EPFNOSUPPORT,		"Protocol family not supported" },
{ EAFNOSUPPORT,		"Address family not supported by protocol family" },
{ EADDRINUSE,		"Address already in use" },
{ EADDRNOTAVAIL,	"Can't assign requested address" },
{ ENETDOWN,			"Network is down" },
{ ENETUNREACH,		"Network is unreachable" },
{ ENETRESET,		"Network dropped connection on reset" },
{ ECONNABORTED,		"Software caused connection abort" },
{ ECONNRESET,		"Connection reset by peer" },
{ ENOBUFS,			"No buffer space available" },
{ EISCONN,			"Socket is already connected" },
{ ENOTCONN,			"Socket is not connected" },
{ ESHUTDOWN,		"Can't send after socket shutdown" },
{ ETOOMANYREFS,		"Too many references: can't splice" },
{ ETIMEDOUT,		"Connection timed out" },
{ ECONNREFUSED,		"Connection refused" },
{ ELOOP,			"Too many levels of symbolic links" },
{ ENAMETOOLONG,		"File name too long" },
{ EHOSTDOWN,		"Host is down" },
{ EHOSTUNREACH,		"No route to host" },
{ ENOTEMPTY,		"Directory not empty" },
{ EPROCLIM,			"Too many processes" },
{ EUSERS,			"Too many users" },
{ EDQUOT,			"Disc quota exceeded" },
{ ESTALE,			"Stale NFS file handle" },
{ EREMOTE, 			"Too many levels of remote in path" },
{ EBADRPC, 			"RPC struct is bad" },
{ ERPCMISMATCH,		"RPC version wrong" },
{ EPROGUNAVAIL, 	"RPC prog. not avail" },
{ EPROGMISMATCH,	"Program version wrong" },
{ EPROCUNAVAIL,		"Bad procedure for program" },
{ ENOLCK,			"No locks available" },
{ ENOSYS,			"Function not implemented" },
{ EFTYPE,			"Inappropriate file type or format" },
{ EAUTH,			"Authentication error" },
{ ENEEDAUTH,		"Need authenticator" },
{ EIDRM,			"Identifier removed" },
{ ENOMSG,			"No message of the desired type" },
{ EOVERFLOW,		"Value too large to be stored in data type" },
{ EILSEQ,			"Encoding error detected" },
{ ENOTSUP,			"Not supported" },
{ 0,				"" },
};

char *myStrError( int err )
{
int pos;

	pos = 0;

	while( myStrings[pos].errcode )
	{
		if ( myStrings[pos].errcode == err )
		{
			break;
		}
		else
		{
			pos++;
		}
	}

	return( myStrings[pos].errstring );
}

// --

void myZLib_Cleanup( struct Config *cfg )
{
	if ( cfg->cfg_ServerZLibAvailable )
	{
		IZ->DeflateEnd( & cfg->cfg_zStream );
		cfg->cfg_ServerZLibAvailable = FALSE;
	}
}

// --

void myZLib_Setup( struct Config *cfg )
{
int32 stat;

	if ( IZ == NULL )
	{
		// ZLib library not available
		goto bailout;
	}

	if ( ! cfg->cfg_Disk_Settings.ZLib )
	{
		// ZLib not enabled
		goto bailout;
	}

	if ( cfg->cfg_ServerZLibAvailable )
	{
		// Allready setup
		goto bailout;
	}

//	zs->cfg_zStream.data_type = Z_BINARY;

	stat = IZ->DeflateInit( & cfg->cfg_zStream, Z_DEFAULT_COMPRESSION );

	if ( stat != Z_OK )
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Failed to initalize zlib stream, disabling support" );
		goto bailout;
	}

	cfg->cfg_ServerZLibAvailable = TRUE;

bailout:

	if ( ! cfg->cfg_ServerZLibAvailable )
	{
		myZLib_Cleanup( cfg );
	}
};

// --

static uint8 WALLDATA[] = 
{
	0x00,0x11,0x22,0x33,
	0x44,0x55,0x66,0x77,
	0x88,0x99,0xaa,0xbb,
	0xcc,0xdd,0xee,0xff,

	0x0f,0x1e,0x2d,0x3c,
	0x4b,0x5a,0x69,0x78,
	0x87,0x96,0xa5,0xb4,
	0xc3,0xd2,0xe1,0xf0
};

void *myMalloc( int size )
{
char *data;
void *mem;
int newsize;
int pos;

// IExec->DebugPrintF( "myMalloc 11 : size: %d\n", size );

	newsize = size;

	if ( DoMugWall )
	{
		newsize += 4; // Size
		newsize += sizeof( WALLDATA ); // Front Wall
		newsize += sizeof( WALLDATA ); // Rear Wall
	}

	#ifdef DO_CLIB

	mem = malloc( newsize );

	#else

	mem = IExec->AllocVecTags( newsize,
		TAG_END
	);

	#endif

	if ( DoMugWall )
	{
		pos = 0;
		data = mem;

		data[pos++] = ( newsize & 0xff000000 ) >> 24;
		data[pos++] = ( newsize & 0x00ff0000 ) >> 16;
		data[pos++] = ( newsize & 0x0000ff00 ) >> 8;
		data[pos++] = ( newsize & 0x000000ff ) >> 0;

		memcpy( & data[pos], WALLDATA, sizeof( WALLDATA ));

		pos += sizeof( WALLDATA );
		pos += size;

		memcpy( & data[pos], WALLDATA, sizeof( WALLDATA ));

// IExec->DebugPrintF( "myMalloc 22 : mem: %p\n", & data[ 4 + sizeof( WALLDATA ) ] );

		return( & data[ 4 + sizeof( WALLDATA ) ] );
	}
	else
	{
// IExec->DebugPrintF( "myMalloc 33 : mem: %p\n", mem );

		return( mem );
	}
}

// --

void *myCalloc( int size )
{
void *mem;
char *data;
int newsize;
int pos;

// IExec->DebugPrintF( "myCalloc 11 : size: %d\n", size );

	newsize = size;

	if ( DoMugWall )
	{
		newsize += 4; // Size
		newsize += sizeof( WALLDATA ); // Front Wall
		newsize += sizeof( WALLDATA ); // Rear Wall

// IExec->DebugPrintF( "myCalloc 11.2 : newsize: %d\n", newsize );
	}

	#ifdef DO_CLIB

	mem = calloc( 1, newsize );

	#else

	mem = IExec->AllocVecTags( newsize,
		AVT_Clear, 0,
		TAG_END
	);

	#endif

	if ( DoMugWall )
	{
		pos = 0;
		data = mem;

		data[pos++] = ( newsize & 0xff000000 ) >> 24;
		data[pos++] = ( newsize & 0x00ff0000 ) >> 16;
		data[pos++] = ( newsize & 0x0000ff00 ) >> 8;
		data[pos++] = ( newsize & 0x000000ff ) >> 0;

		memcpy( & data[pos], WALLDATA, sizeof( WALLDATA ));

		pos += sizeof( WALLDATA );
		pos += size;

		memcpy( & data[pos], WALLDATA, sizeof( WALLDATA ));

// IExec->DebugPrintF( "myCalloc 22 : mem: %p\n", & data[ 4 + sizeof( WALLDATA ) ] );

		return( & data[ 4 + sizeof( WALLDATA ) ] );
	}
	else
	{
// IExec->DebugPrintF( "myCalloc 33 : mem: %p\n", mem );

		return( mem );
	}
}

// --

char *myStrdup( char *str )
{
char *copy;

// IExec->DebugPrintF( "myStrdup 11 : str %s\n", str );

	if ( str )
	{
		int len = strlen( str );

		copy = myMalloc( len + 1 );

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

// IExec->DebugPrintF( "myStrdup 22 : mem %p, str %s\n", copy, copy );

	return( copy );
}

// --

#ifdef DO_CLIB
char *myASPrintF( char *fmt, ... )
#else
char * VARARGS68K myASPrintF( char *fmt, ... )
#endif
{
va_list ap;
char *str;
char *s;

	va_start( ap, fmt );

// IExec->DebugPrintF( "myASPrintF 11 : fmt %s\n", fmt );

	if ( fmt )
	{
		#ifdef DO_CLIB

		if ( vasprintf( & str, fmt, ap ) == -1 )
		{
			str = NULL;
		}

		if (( DoMugWall ) && ( str ))
		{
			s = str;

			str = myStrdup( str );

			free( s );
		}

		#else

		str = IUtility->VASPrintf( fmt, (APTR) va_getlinearva( ap, uint32 ));

		if (( DoMugWall ) && ( str ))
		{
			s = str;

			str = myStrdup( str );

			IExec->FreeVec( s );
		}

		#endif
	}
	else
	{
		str = NULL;
	}

	va_end( ap );

// IExec->DebugPrintF( "myASPrintF 22 : mem %p, str %s\n", str, str );

	return( str );
}

// --

void myFree( void *mem )
{
uint32 cnt;
char *data;
int size;

// IExec->DebugPrintF( "myFree 11 : mem %p\n", mem );

	if ( mem == NULL )
	{
		goto bailout;
	}

	if ( DoMugWall )
	{
		data = mem;
		data -= 4;
		data -= sizeof( WALLDATA );
		size = ( data[0] << 24 ) | ( data[1] << 16 ) | ( data[2] << 8 ) | ( data[3] << 0 );

		if ( memcmp( & data[4], WALLDATA, sizeof( WALLDATA )))
		{
			printf( "Front Wall Damaged, Mem %p, Size %d\n", mem, size );

			for( cnt=0 ; cnt<sizeof(WALLDATA) ; cnt++ )
			{
				printf( "%02X", data[4+cnt] );
			}

			printf( "\n" );

			for( cnt=0 ; cnt<sizeof(WALLDATA) ; cnt++ )
			{
				printf( "%02X", WALLDATA[cnt] );
			}

			printf( "\n" );
		}

		if ( memcmp( & data[ size - sizeof( WALLDATA ) ], WALLDATA, sizeof( WALLDATA )))
		{
			printf( "Rear Wall Damaged, Mem %p, Size %d\n", mem, size );

			for( cnt=0 ; cnt<sizeof(WALLDATA) ; cnt++ )
			{
				printf( "%02X", data[size-16+cnt] );
			}

			printf( "\n" );

			for( cnt=0 ; cnt<sizeof(WALLDATA) ; cnt++ )
			{
				printf( "%02X", WALLDATA[cnt] );
			}

			printf( "\n" );
		}

		mem = data;
	}

	#ifdef DO_CLIB

	free( mem );

	#else

	IExec->FreeVec( mem );

	#endif

bailout:

// IExec->DebugPrintF( "myFree 22 :\n" );

	return;
}

// --
