
/*
** SPDX-License-Identifier: GPL-3.0-or-later
** Copyright (c) 2023-2025 Rene W. Olsen < renewolsen @ gmail . com >
*/

// --

#include "rVNCd.h"

// --

void PrintProgramHeader( void )
{
	static S32 ProgramInfo = FALSE;

	if ( ! ProgramInfo )
	{
		ProgramInfo = TRUE;
		SHELLBUF_PRINTF( "\n" );
		SHELLBUF_PRINTF( "  %s (%s)\n", VERS, DATE );
		SHELLBUF_PRINTF( "    by Rene W. Olsen\n" );
		SHELLBUF_PRINTF( "\n" );
	}
}

// --

S32 StopServer( struct Config *cfg )
{
S32 error;

	error = FALSE;

	myStop_Server( cfg );

	return( error );
}

// --

S32 StartServer( struct Config *cfg )
{
S32 error;

	error = myStart_Server( cfg );

	return( error );
}

// --

S32 RestartServer( struct Config *cfg )
{
S32 error;

	myStop_Server( cfg );

	error = myStart_Server( cfg );

	return( error );
}

// --

void Func_Quit( struct Config *cfg )
{
	// Todo .. started on Quit gui .. but never finished it
	//
	// Idea is to open Quit window with a 10s timer
	// also check if last user disconnect
	// but only if a User is connected

	if ( cfg->UserCount )
	{
		//
		//	if ( ! cfg->cfg_WinData[WIN_Quit].OpenWin( cfg ))
		//	{
		//		if ( TimedDosRequesterTags(
		//			TDR_Window, cfg->cfg_WinData[WIN_Main].WindowAdr,
		//			TDR_ImageType, TDRIMAGE_QUESTION,
		//			TDR_TitleString, "rVNCd",
		//			TDR_FormatString, "There are %ld user(s) connected\nAre you sure you want to Quit?",
		//			TDR_GadgetString, "_No|_Yes",
		//			TDR_Arg1, cfg->UserCount,
		//			TAG_END ) == 0 )
		//		{
		//			Signal( ProgramTask, SIGBREAKF_CTRL_C );
		//			ProgramRunning = FALSE;
		//		}
		//	}
		//

		#ifdef _AOS4_
		if ( TimedDosRequesterTags(
//			TDR_Window, cfg->cfg_WinData[WIN_Main].WindowAdr,
			TDR_ImageType, TDRIMAGE_QUESTION,
			TDR_TitleString, "rVNCd",
			TDR_FormatString, "There are %ld user(s) connected\nAre you sure you want to Quit?",
			TDR_GadgetString, "_No|_Yes",
			TDR_Arg1, cfg->UserCount,
			TAG_END ) == 0 )
		#else
		#warning fix me
		#endif
		{
			Signal( ProgramTask, SIGBREAKF_CTRL_C );
			ProgramRunning = FALSE;
		}
	}
	else
	{
		Signal( ProgramTask, SIGBREAKF_CTRL_C );
		ProgramRunning = FALSE;
	}

}

// --

void Func_ForceQuit( struct Config *cfg UNUSED )
{
	Signal( ProgramTask, SIGBREAKF_CTRL_C );
	ProgramRunning = FALSE;
}

// --
// Called from NetSend task

void Func_Blanker_Enable( struct Config *cfg )
{
	Log_PrintF( cfg, LOGTYPE_Info, "Enabling Blanker" );


	#ifdef HAVE_APPLIB

	SetApplicationAttrs( AppID,
		APPATTR_AllowsBlanker, TRUE,
		TAG_END
	);

	#endif // HAVE_APPLIB
}

// --
// Called from NetSend task

void Func_Blanker_Disable( struct Config *cfg )
{
	Log_PrintF( cfg, LOGTYPE_Info, "Disabling Blanker" );

	#ifdef HAVE_APPLIB

	SendApplicationMsg( AppID, 0, NULL, APPLIBMT_BlankerUnBlank );

	SetApplicationAttrs( AppID,
		APPATTR_AllowsBlanker, FALSE,
		TAG_END
	);

	#endif // HAVE_APPLIB
}

// -- Get a Temp Filename
// Get a temp filenamw in same path,
// that is not in use.

STR Func_GetTempFilename( STR path )
{
char buf[10];
STR str;
STR dup;
STR tmp;
S32 cnt;
S32 i;
S32 h;

	str = NULL;

	if ( path )
	{
		dup = mem_Strdup( path );
	}
	else
	{
		// Use progdir: or just "" ??
		dup = mem_Strdup( "progdir:" );
	}

	if ( dup )
	{
		// -- Remove Filename

		 tmp = (PTR) FilePart( dup );
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

			str = mem_ASPrintF( "%stmp_%s", dup, buf );

			if ( ! str )
			{
//				SHELLBUF_PRINTF( "%s:%04d: Error out of mem\n", __FILE__, __LINE__ );
				break;
			}

			// -- Check if the File Exists

			h = Lock( str, ACCESS_READ );

			if ( ! h )
			{
				// Found a Unique Filename
				break;
			}

			// -- No its in use, loop and try again

			UnLock( h );
			mem_Free( str );
			str = NULL;
		}

		mem_Free( dup );
	}
//	else
//	{
//		SHELLBUF_PRINTF( "%s:%04d: Error out of mem\n", __FILE__, __LINE__ );
//	}

	return( str );
}

// -- Same String
// TRUE  = Same String (excluding NULL,NULL)
// FALSE = Diffrent Strings

S32 Func_SameString( STR str1, STR str2 )
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

// --

S32 Func_GetFileSize( STR name, S32 *size )
{
FILE *file;
S32 retval;
S64 fs;

	fs = 0;

//	if ( DoVerbose > 1 )
//	{
//		SHELLBUF_PRINTF( "Func_GetFileSize : Name %s\n", name );
//	}

	SetIoErr( 0 );

	file = fopen( name, "rb" );

	if ( ! file )
	{
		retval = 0; // Not found
		goto bailout;
	}

	fseek( file, 0, SEEK_END );
	fs = ftell( file );
	*size = (S32) fs;
	fclose( file );

	retval = 1; // Found

bailout:

	if ( DoVerbose > 1 )
	{
		if ( retval )
		{
			SHELLBUF_PRINTF( "FileSize %s, %ld Bytes\n", name, (S32) fs );
		}
		else
		{
			SHELLBUF_PRINTF( "FileSize %s not found (%d)\n", name, (S32) fs, (S32) IoErr() );
		}
	}

	return( retval );
}

// --

S32 Func_Strnicmp( STR s1, STR s2, size_t n )
{
	while (n-- > 0) 
	{
		unsigned char c1 = (unsigned char)tolower((unsigned char)*s1);
		unsigned char c2 = (unsigned char)tolower((unsigned char)*s2);
		
		if ( c1 != c2 || c1 == '\0' || c2 == '\0' ) 
		{
			return c1 - c2;
		}

		s1++;
		s2++;
	}

	return 0;
}

// --
// Well building my own strerror()
// as newlib or clib2 do not have those

struct ErrString
{
	S32		errcode;
	STR	errstring;
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

STR Func_StrError( S32 err )
{
S32 pos;

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

S32 Func_NetRead( struct Config *cfg, PTR buf, S32 len, S32 flag )
{
//struct SocketIFace *ISocket;
S32 err;
S32 max;
S32 cnt;
S32 rc;
S32 s;

	// During startup we read From Send socket

	#ifdef _AOS4_

	struct SocketIFace *ISocket;

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

	#else

	struct Library *SocketBase;

	if ( cfg->NetRead_Idle )
	{
		s = cfg->NetSend_ClientSocket;

		SocketBase = cfg->NetSend_SocketBase;
	}
	else
	{
		s = cfg->NetRead_ClientSocket;

		SocketBase = cfg->NetRead_SocketBase;
	}

	#endif

	cnt = 0;
	max = 3;

	while( TRUE )
	{
		rc = recv( s, buf, len, flag );

		if ( rc == -1 )
		{
			err = Errno();

			if (( err == EINTR ) && ( cnt++ < max ))
			{
				Log_PrintF( cfg, LOGTYPE_Warning, "Failed to read data '%s' (%ld) Retyring %ld/%ld", Func_StrError(err), err, cnt, max );
				continue;
			}
			else
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = mem_ASPrintF( "Failed to read data (%ld)", err );
				}

				Log_PrintF( cfg, LOGTYPE_Error, "Failed to read data '%s' (%ld)", Func_StrError(err), err );
				goto bailout;
			}
		}

		break;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = mem_ASPrintF( "Client closed connection" );
		}

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}

		goto bailout;
	}

	if (( rc != len ) && ( flag & MSG_WAITALL ))
	{
		Log_PrintF( cfg, LOGTYPE_Error, "Func_NetRead : Invalid data length (%ld != %ld)", rc, len );
		rc = -2;
		goto bailout;
	}

	ObtainSemaphore( & cfg->Server_UpdateSema );
	cfg->SessionStatus.si_Read_Bytes += rc;
	ReleaseSemaphore( & cfg->Server_UpdateSema );

bailout:

	return( rc );
}

// --

S32 Func_NetSend( struct Config *cfg, PTR buf, S32 len )
{
//struct SocketIFace *ISocket;
S32 rc;
S32 max;
S32 cnt;
S32 err;

//	ISocket = cfg->NetSend_ISocket;

	#ifdef _AOS4_
	struct SocketIFace *ISocket = cfg->NetSend_ISocket;
	#else
	struct Library *SocketBase = cfg->NetSend_SocketBase;
	#endif

	cnt = 0;
	max = 3;

	while( TRUE )
	{
		rc = send( cfg->NetSend_ClientSocket, buf, len, 0 );

		if ( rc == -1 )
		{
			err = Errno();

			if (( err == EINTR ) && ( cnt++ < max ))
			{
				Log_PrintF( cfg, LOGTYPE_Warning, "Failed to send data '%s' (%ld) Retyring %ld/%ld", Func_StrError(err), err, cnt, max );
				continue;
			}
			else
			{
				if ( ! cfg->cfg_NetReason )
				{
					cfg->cfg_NetReason = mem_ASPrintF( "Failed to send data (%ld)", err );
				}

				Log_PrintF( cfg, LOGTYPE_Error, "Failed to send data '%s' (%ld)", Func_StrError(err), err );
				goto bailout;
			}
		}

		break;
	}

	if ( rc == 0 )
	{
		if ( ! cfg->cfg_NetReason )
		{
			cfg->cfg_NetReason = mem_ASPrintF( "Client closed connection" );
		}

		if ( cfg->cfg_LogUserDisconnect )
		{
			Log_PrintF( cfg, LOGTYPE_Info|LOGTYPE_Event, "User disconnect" );
		}

		goto bailout;
	}

	ObtainSemaphore( & cfg->Server_UpdateSema );
	cfg->SessionStatus.si_Send_Bytes += rc;
	ReleaseSemaphore( & cfg->Server_UpdateSema );

bailout:

	return( rc );
}

// --
