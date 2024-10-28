
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2024 Rene W. Olsen <renewolsen@gmail.com>
*/

// --

#include "RVNCd.h"

// -- Same String
// TRUE  = Same String (excluding NULL,NULL)
// FALSE = Diffrent Strings

int mySameString( char *str1, char *str2 )
{
	if (( ! str1 ) && ( ! str2 ))
	{
		return( FALSE );
	}

	if (( ! str1 ) && ( str2 ))
	{
		return( FALSE );
	}

	if (( str1 ) && ( ! str2 ))
	{
		return( FALSE );
	}

	if ( strcmp( str1, str2 ))
	{
		return( FALSE );
	}

	return( TRUE );
}

// -- Get a Temp Filename
// Get a temp filenamw in same path,
// that is not in use.

char *myGetTempFilename( char *path )
{
char buf[10];
char *str;
char *dup;
char *tmp;
int cnt;
int i;
int h;

	str = NULL;

	if ( path )
	{
		dup = myStrdup( path );
	}
	else
	{
		// Use progdir: or just "" ??
		dup = myStrdup( "progdir:" );
	}

	if ( dup )
	{
		// -- Remove Filename

		 tmp = (APTR) IDOS->FilePart( dup );
		*tmp = 0; 

		// -- Find Unique Path+Filename

		for( cnt=0 ; cnt<15 ; cnt++ )
		{
			// -- Init Random Buffer

			for( i=0 ; i<8 ; i++ )
			{
				buf[i] = ( rand() % 26 ) + 'a';
			}	buf[i] = 0;

			// -- Build Filename String

			str = myASPrintF( "%stmp_%s", dup, buf );

			if ( ! str )
			{
//				printf( "%s:%04d: Error out of mem\n", __FILE__, __LINE__ );
				break;
			}

			// -- Check if the File Exists

			h = IDOS->Lock( str, ACCESS_READ );

			if ( ! h )
			{
				// Found a Unique Filename
				break;
			}

			// -- No its in use, loop and try again

			IDOS->UnLock( h );
			myFree( str );
			str = NULL;
		}

		myFree( dup );
	}
//	else
//	{
//		printf( "%s:%04d: Error out of mem\n", __FILE__, __LINE__ );
//	}

	return( str );
}

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
}

// --

void *myMalloc( int size )
{
void *mem;
int newsize;

	newsize = size;

	#ifdef DO_CLIB

	mem = malloc( newsize );

	#else

	mem = IExec->AllocVecTags( newsize,
		TAG_END
	);

	#endif

	return( mem );
}

// --

void *myCalloc( int size )
{
void *mem;
//char *data;
int newsize;
//int pos;

	newsize = size;

	#ifdef DO_CLIB

	mem = calloc( 1, newsize );

	#else

	mem = IExec->AllocVecTags( newsize,
		AVT_Clear, 0,
		TAG_END
	);

	#endif

	return( mem );
}

// --

char *myStrdup( char *str )
{
char *copy;

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
//char *s;

	va_start( ap, fmt );

	if ( fmt )
	{
		#ifdef DO_CLIB

		if ( vasprintf( & str, fmt, ap ) == -1 )
		{
			str = NULL;
		}

		#else

		str = IUtility->VASPrintf( fmt, (APTR) va_getlinearva( ap, uint32 ));

		#endif
	}
	else
	{
		str = NULL;
	}

	va_end( ap );

	return( str );
}

// --

void myFree( void *mem )
{
	if ( mem == NULL )
	{
		goto bailout;
	}

	#ifdef DO_CLIB

	free( mem );

	#else

	IExec->FreeVec( mem );

	#endif

bailout:

	return;
}

// --

int myNetRead( struct Config *cfg, void *buf, int len, int flag )
{
struct SocketIFace *ISocket;
int err;
int max;
int cnt;
int rc;
int s;

	// During startup we read From Send socket

	if ( cfg->NetRead_Idle )
	{
		s = cfg->NetSend_ClientSocket;

		ISocket = cfg->NetSend_ISocket;
	}
	else
	{
		s = cfg->NetRead_ClientSocket;

		ISocket = cfg->NetRead_ISocket;
	}

	cnt = 0;
	max = 3;

	while( TRUE )
	{
		rc = ISocket->recv( s, buf, len, flag );

		if ( rc == -1 )
		{
			err = ISocket->Errno();

			if (( err == EINTR ) && ( cnt++ < max ))
			{
				Log_PrintF( cfg, LOGTYPE_Warning, "Failed to read data '%s' (%ld) Retyring %ld/%ld", myStrError(err), err, cnt, max );
				continue;
			}
			else
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = myASPrintF( "Failed to read data (%ld)", err );
				}

				Log_PrintF( cfg, LOGTYPE_Error, "Failed to read data '%s' (%ld)", myStrError(err), err );
				goto bailout;
			}
		}

		break;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}

		goto bailout;
	}

	if (( rc != len ) && ( flag & MSG_WAITALL ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "myNetRead : Invalid data length (%ld != %ld)", rc, len );
		rc = -2;
		goto bailout;
	}

	IExec->ObtainSemaphore( & cfg->Server_UpdateSema );
	cfg->SessionStatus.si_Read_Bytes += rc;
	IExec->ReleaseSemaphore( & cfg->Server_UpdateSema );

bailout:

	return( rc );
}

// --

int myNetSend( struct Config *cfg, void *buf, int len )
{
struct SocketIFace *ISocket;
int rc;
int max;
int cnt;
int err;

	ISocket = cfg->NetSend_ISocket;

	cnt = 0;
	max = 3;

	while( TRUE )
	{
		rc = ISocket->send( cfg->NetSend_ClientSocket, buf, len, 0 );

		if ( rc == -1 )
		{
			err = ISocket->Errno();

			if (( err == EINTR ) && ( cnt++ < max ))
			{
				Log_PrintF( cfg, LOGTYPE_Warning, "Failed to send data '%s' (%ld) Retyring %ld/%ld", myStrError(err), err, cnt, max );
				continue;
			}
			else
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = myASPrintF( "Failed to send data (%ld)", err );
				}

				Log_PrintF( cfg, LOGTYPE_Error, "Failed to send data '%s' (%ld)", myStrError(err), err );
				goto bailout;
			}
		}

		break;
	}


	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = myASPrintF( "Client closed connection" );
		}

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}

		goto bailout;
	}

	IExec->ObtainSemaphore( & cfg->Server_UpdateSema );
	cfg->SessionStatus.si_Send_Bytes += rc;
	IExec->ReleaseSemaphore( & cfg->Server_UpdateSema );

bailout:

	return( rc );
}

// --
